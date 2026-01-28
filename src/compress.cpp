#include "compress.h"
#include "global.h"

void computeVandermonde(std::vector<std::vector<uint64_t>>& vandermonde)
{
    for (size_t j = 0; j < vandermonde[0].size(); j++) {
        vandermonde[0][j] = j + 1;
    }
    for (size_t i = 1; i < vandermonde.size(); i++) {
        for (size_t j = 0; j < vandermonde[0].size(); j++) {
            vandermonde[i][j] = (vandermonde[i-1][j] * (j + 1)) % ptxt_modulus;
        }
    }
}

void ringSwitch(lbcrypto::Ciphertext<lbcrypto::DCRTPoly>& digest,
                const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context_comp,
                const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context_trace,
                const lbcrypto::PublicKey<lbcrypto::DCRTPoly>& publicKey_trace)
{
    using namespace std;
    using namespace lbcrypto;

    vector<int64_t> vec_mask1(degree,0);
    vector<int64_t> vec_mask2(degree,0);
    for (int i = 0; i < degree_trace_half; i++) {
        vec_mask1[i] = dim_trace;
        vec_mask2[i + degree_half] = dim_trace;
    }
    auto ptxt_mask1 = context_comp->MakePackedPlaintext(vec_mask1);
    auto ptxt_mask2 = context_comp->MakePackedPlaintext(vec_mask2);

    Ciphertext<DCRTPoly> temp;
    temp = context_comp->EvalRotate(digest, degree_trace_half);
    temp = context_comp->EvalMult(temp, ptxt_mask2);
    digest = context_comp->EvalMult(digest, ptxt_mask1);
    context_comp->EvalAddInPlace(digest, temp);

    digest = context_comp->Compress(digest);

    vector<int64_t> vec_null(degree_trace);
    auto ptxt_null = context_trace->MakePackedPlaintext(vec_null);
    auto ctxt_trace = context_trace->Encrypt(publicKey_trace, ptxt_null);
    ctxt_trace = context_trace->Compress(ctxt_trace);

    auto poly_comp = digest->GetElements();
    auto poly_trace = ctxt_trace->GetElements();
    for (int i = 0; i < 2; i++) {
        poly_comp[i].SwitchFormat();
        poly_trace[i].SwitchFormat();
    }
    for (int i = 0; i < 2; i++) {
        NativePoly poly_comp_ = poly_comp[i].GetElementAtIndex(0);
        NativePoly poly_trace_ = poly_trace[i].GetElementAtIndex(0);
        for (size_t k = 0; k < poly_trace_.GetLength(); k++) {
            poly_trace_[k] = poly_comp_[dim_trace * k].Mod(poly_trace_.GetModulus());
        }
        poly_trace[i].SetElementAtIndex(0, poly_trace_);
    }
    for (int i = 0; i < 2; i++) {
        poly_trace[i].SwitchFormat();
    }
    ctxt_trace->SetElements(poly_trace);

    digest = ctxt_trace;
}

void compress(lbcrypto::Ciphertext<lbcrypto::DCRTPoly>& digest,
                std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>& PV,
                const std::vector<std::vector<uint32_t>>& payloads,
                const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context_comp,
                const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context_trace,
                const lbcrypto::PublicKey<lbcrypto::DCRTPoly>& publicKey_comp,
                const lbcrypto::PublicKey<lbcrypto::DCRTPoly>& publicKey_trace,
                const lbcrypto::PrivateKey<lbcrypto::DCRTPoly>& privateKey_comp)
{
    using namespace std;
    using namespace lbcrypto;

    vector<int64_t> vec_null(degree);
    auto ptxt_null = context_comp->MakePackedPlaintext(vec_null);
    for(int i = 0; i < numctxt; i++){
        auto ctxt_temp = context_comp->Encrypt(publicKey_comp, ptxt_null);
        ctxt_temp->SetElements(PV[i]->GetElements());
        PV[i] = ctxt_temp;
    }

    // Precompute Vandermonde
    vector<vector<uint64_t>> vandermonde(num_pertinent, vector<uint64_t>(payloads.size()));
    computeVandermonde(vandermonde);

    // Baby-step
    vector<vector<Ciphertext<DCRTPoly>>> rotated_PV(numctxt, vector<Ciphertext<DCRTPoly>>(b_tilde2));
    for (int i = 0; i < numctxt; i++) {
        rotated_PV[i][0] = PV[i];
        for (int b = 0; b < b_tilde2-1; b++) {
            rotated_PV[i][b+1] = context_comp->EvalRotate(rotated_PV[i][b], 1);
        }
    }

    // Giant-step
    vector<Ciphertext<DCRTPoly>> giant(numctxt);
    vector<int64_t> ptxt_vec(degree);

    for (int g_ = 0; g_ < g_tilde2 ; g_++) {
        int g = g_tilde2 - g_ - 1;

        for (int i = 0; i < numctxt; i++) {
            int counter = i * degree;
            for (int b = 0; b < b_tilde2; b++) {
                if ( g * b_tilde2 + b >= numrow_po2 ) {break;}

                int k2 = degree_half;
                int idxr = (numrow_po2 - g * b_tilde2) % numrow_po2;
                int idxc1 = counter + b;
                int idxc2 = idxc1 + degree_half;

                for (int k1 = 0; k1 < degree_half; k1++) {

                    if (idxr >= numrow) {
                        ptxt_vec[k1] = 0;
                        ptxt_vec[k2] = 0;
                    } else if (idxr < num_pertinent) {
                        ptxt_vec[k1] = vandermonde[idxr][idxc1];
                        ptxt_vec[k2] = vandermonde[idxr][idxc2];
                    } else {
                        int idxr_q = idxr / num_pertinent - 1;
                        int idxr_r = idxr % num_pertinent;
                        ptxt_vec[k1] = (vandermonde[idxr_r][idxc1] * payloads[idxc1][idxr_q]) % ptxt_modulus;
                        ptxt_vec[k2] = (vandermonde[idxr_r][idxc2] * payloads[idxc2][idxr_q]) % ptxt_modulus;
                    }

                    k2++; idxr++; idxc1++; idxc2++;
                    if (idxr == numrow_po2) { idxr = 0; }
                    if (idxc2 == counter + degree) {
                        idxc1 -= degree_half;
                        idxc2 -= degree_half;
                    }
                }

                auto ptxt = context_comp->MakePackedPlaintext(ptxt_vec);

                if ( b == 0 ) {
                    giant[i] = context_comp->EvalMult(rotated_PV[i][0], ptxt);
                } else {
                    auto temp = context_comp->EvalMult(rotated_PV[i][b], ptxt);
                    context_comp->EvalAddInPlace(giant[i], temp);
                }
            }
        }

        Ciphertext<DCRTPoly> sum = giant[0];
        for (int i = 1; i < numctxt; i++) {
            context_comp->EvalAddInPlace(sum, giant[i]);
        }

        if (g_ == 0) {
            digest = sum;
        } else {
            digest = context_comp->EvalRotate(digest, b_tilde2);
            context_comp->EvalAddInPlace(digest, sum);
        }
    }

    // Block Summation
    Ciphertext<DCRTPoly> temp;
    for (int j = 1; j < degree_half / numrow_po2; j*=2) {
        temp = context_comp->EvalRotate(digest, numrow_po2 * j);
        context_comp->EvalAddInPlace(digest, temp);
    }
    temp = context_comp->EvalRotate(digest, degree_half);
    context_comp->EvalAddInPlace(digest, temp);

    // Ring-Switching
    ringSwitch(digest, context_comp, context_trace, publicKey_trace);
}
