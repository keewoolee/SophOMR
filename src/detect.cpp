#include "detect.h"
#include "global.h"

void affine(std::vector<std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>>& output,
            const std::vector<std::vector<uint64_t>>& signals_a,
            const std::vector<std::vector<uint64_t>>& signals_b,
            const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context,
            const lbcrypto::Ciphertext<lbcrypto::DCRTPoly>& PSsk_enc)
{
    // Baby-step
    std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>> PSsk_bs(b_tilde1);
    PSsk_bs[0] = PSsk_enc;
    for (int b = 0; b < b_tilde1 - 1; b++) {
        PSsk_bs[b+1] = context->EvalRotate(PSsk_bs[b], 1);
    }

    // Giant-step
    for (int i = 0; i < numctxt; i++) {
        int idx = i * degree;
        std::vector<int64_t> vec_ints(degree);

        for (int l = 0; l < PSparam.ell; l++) {
            for (int g_ = 0; g_ < g_tilde1 ; g_++) {
                int g = g_tilde1 - g_ - 1;
                lbcrypto::Ciphertext<lbcrypto::DCRTPoly> giant;

                for (int b = 0; b < b_tilde1; b++) {
                    int k = g * b_tilde1 + b;
                    if ( k >= PSparam.n ) {break;}

                    for(int m = 0; m < degree; m++){
                        int idxc = (k + m) % PSparam.n;
                        if (idxc <= l) {
                            vec_ints[m] = signals_a[idx + m][l - idxc];
                        } else {
                            vec_ints[m] = PSparam.q - signals_a[idx + m][PSparam.n + l - idxc];
                            if (vec_ints[m] == PSparam.q) { vec_ints[m] = 0; }
                        }
                    }

                    int idx_rot = g * b_tilde1;
                    rotate(vec_ints.begin(), vec_ints.begin() + degree_half - idx_rot, vec_ints.begin() + degree_half);
                    rotate(vec_ints.begin() + degree_half, vec_ints.begin() + degree - idx_rot, vec_ints.begin() + degree);

                    if ( b == 0 ) {
                        giant = context->EvalMult(PSsk_bs[0], context->MakePackedPlaintext(vec_ints));
                    } else {
                        auto temp = context->EvalMult(PSsk_bs[b], context->MakePackedPlaintext(vec_ints));
                        context->EvalAddInPlace(giant, temp);
                    }
                }

                if (g_ == 0) {
                    output[i][l] = giant;
                } else {
                    output[i][l] = context->EvalRotate(output[i][l], b_tilde1);
                    context->EvalAddInPlace(output[i][l], giant);
                }
            }
        }

        for (int l = 0; l < PSparam.ell; l++){
            for(int m = 0; m < degree; m++){
                vec_ints[m] = signals_b[idx + m][l];
            }
            auto pt = context->MakePackedPlaintext(vec_ints);
            output[i][l] = context->EvalSub(output[i][l], pt);
        }
    }
}

void powers(std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>& inplace, const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context)
{
    inplace[1] = inplace[0];
    for (size_t i = 2; i < inplace.size(); i++) {
        if(i % 2 == 0){
            inplace[i] = context->EvalSquare(inplace[i/2]);
        } else {
            inplace[i] = context->EvalMult(inplace[(i-1)/2], inplace[(i+1)/2]);
        }
    }
}

void patersonStockmeyer(lbcrypto::Ciphertext<lbcrypto::DCRTPoly>& inplace,
                            const std::vector<std::vector<uint32_t>>& coeff,
                            const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context)
{
    int numbs = coeff[0].size();
    int numgs = coeff.size();

    std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>> bs(numbs+1);
    bs[0] = inplace;
    powers(bs, context);

    std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>> gs(numgs);
    gs[0] = bs[numbs];
    powers(gs, context);

    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> hoist;

    for (int g = 0; g < numgs; g++) {
        lbcrypto::Ciphertext<lbcrypto::DCRTPoly> bsSum;
        for (int b = 1; b < numbs; b++) {
            std::vector<int64_t> vec(degree, coeff[g][b]);
            auto ptxt = context->MakePackedPlaintext(vec);

            auto temp = context->EvalMult(bs[b], ptxt);
            if (b == 1) {
                bsSum = temp;
            } else {
                context->EvalAddInPlace(bsSum, temp);
            }
        }

        std::vector<int64_t> vec(degree, coeff[g][0]);
        auto ptxt = context->MakePackedPlaintext(vec);
        context->EvalAddInPlace(bsSum, ptxt);

        if(g == 0) {
            inplace = bsSum;
        } else if (g == 1) {
            hoist = context->EvalMultNoRelin(bsSum, gs[g]);
        } else {
            bsSum = context->EvalMultNoRelin(bsSum, gs[g]);
            context->EvalAddInPlace(hoist, bsSum);
        }
    }

    context->RelinearizeInPlace(hoist);
    context->EvalAddInPlace(inplace, hoist);
}

void FLT(lbcrypto::Ciphertext<lbcrypto::DCRTPoly>& inplace, const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context)
{
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> curr = inplace;

    auto pow = ptxt_modulus - 1;
    bool flag_first = true;

    while(pow > 0){
        if(pow % 2 == 0){
            pow /= 2;
            context->EvalSquareInPlace(curr);
        } else {
            pow -= 1;
            if (flag_first) {
                inplace = curr;
                flag_first = false;
                continue;
            }
            inplace = context->EvalMult(inplace, curr);
        }
    }
}

lbcrypto::Ciphertext<lbcrypto::DCRTPoly> product(std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>& input,
                                                    const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context)
{
    while(input.size() != 1){
        for(size_t i = 0; i < input.size()/2; i++){
            input[i] = context->EvalMult(input[i], input[input.size()/2+i]);
        }
        if(input.size() % 2 == 0)
            input.resize(input.size()/2);
        else{
            input[input.size()/2] = input[input.size()-1];
            input.resize(input.size()/2+1);
        }
    }

    return input[0];
}

void rangeCheck(std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>& output,
                    std::vector<std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>>& input,
                    const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context,
                    const lbcrypto::EvalKey<lbcrypto::DCRTPoly>& swk)
{
    // One-vector for Logical Negation
    std::vector<int64_t> vec_one(degree, 1);
    auto ptxt_one = context->MakePackedPlaintext(vec_one);

    for (int i = 0; i < numctxt; i++){
        for(int j = 0; j < PSparam.ell; j++){
            context->EvalSquareInPlace(input[i][j]);

            // Zeroize
            patersonStockmeyer(input[i][j], coeff_rangeCheck, context);

            // Fermat's Little Theorem
            FLT(input[i][j], context);

            // Logical Negation
            input[i][j] = context->EvalSub(ptxt_one, input[i][j]);
        }
    }

    for (int i = 0; i < numctxt; i++){
        output[i] = product(input[i], context);
        output[i] = context->Compress(output[i], 2);
        context->GetScheme()->KeySwitchInPlace(output[i], swk);
    }
}

void detect(std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>& output,
                const std::vector<std::vector<uint64_t>>& signals_a,
                const std::vector<std::vector<uint64_t>>& signals_b,
                const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context,
                const lbcrypto::Ciphertext<lbcrypto::DCRTPoly>& PSsk_enc,
                const lbcrypto::EvalKey<lbcrypto::DCRTPoly>& swk)
{
    using namespace std;
    using namespace lbcrypto;

    chrono::high_resolution_clock::time_point clock_start, clock_end;

    vector<vector<Ciphertext<DCRTPoly>>> PV_ell(numctxt, vector<Ciphertext<DCRTPoly>>(PSparam.ell));

    cout << "\t\t Affine Transform Started!\n" << endl;
    clock_start = chrono::high_resolution_clock::now();

    affine(PV_ell, signals_a, signals_b, context, PSsk_enc);

    clock_end = chrono::high_resolution_clock::now();
    cout << "\t\t Affine Transform Finished!" << endl;
    cout << "\t\t Affine Transform time: " << chrono::duration<double>(clock_end - clock_start).count() << "sec\n" << endl;


    cout << "\t\t RangeCheck Started!\n" << endl;
    clock_start = chrono::high_resolution_clock::now();

    rangeCheck(output, PV_ell, context, swk);

    clock_end = chrono::high_resolution_clock::now();
    cout << "\t\t RangeCheck Finished!" << endl;
    cout << "\t\t RangeCheck time: " << chrono::duration<double>(clock_end - clock_start).count() << "sec\n" << endl;
}
