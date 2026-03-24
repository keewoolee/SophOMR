#include "compress.h"
#include "global.h"

namespace {

std::vector<std::vector<uint64_t>> buildVandermondeMatrix(int num_rows, int num_cols)
{
    std::vector<std::vector<uint64_t>> V(num_rows, std::vector<uint64_t>(num_cols));
    for (int j = 0; j < num_cols; j++) {
        V[0][j] = j + 1;
    }
    for (int i = 1; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            V[i][j] = (V[i-1][j] * (j + 1)) % ptxt_modulus;
        }
    }
    return V;
}

void ringSwitch(lbcrypto::Ciphertext<lbcrypto::DCRTPoly>& digest,
                const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context_trace,
                const std::string& traceKeyTag)
{
    using namespace std;
    using namespace lbcrypto;

    auto context_comp = digest->GetCryptoContext();

    // Our slot packing layout prioritizes the n/2 axis in the (n/2)×2
    // rotation structure. This is for the sake of overall readability, at
    // the cost of this extra masking step.
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

    auto traceParams = context_trace->GetCryptoParameters()->GetElementParams();
    size_t numLimbs = digest->GetElements()[0].GetNumOfElements();

    auto poly_comp = digest->GetElements();
    for (int i = 0; i < 2; i++) poly_comp[i].SwitchFormat();

    std::vector<DCRTPoly> poly_trace(2);
    for (int i = 0; i < 2; i++) {
        poly_trace[i] = DCRTPoly(traceParams, Format::COEFFICIENT, true);
        for (size_t limb = 0; limb < numLimbs; limb++) {
            auto poly_comp_ = poly_comp[i].GetElementAtIndex(limb);
            auto poly_trace_ = poly_trace[i].GetElementAtIndex(limb);
            for (int k = 0; k < degree_trace; k++) {
                poly_trace_[k] = poly_comp_[dim_trace * k].Mod(poly_trace_.GetModulus());
            }
            poly_trace[i].SetElementAtIndex(limb, poly_trace_);
        }
        poly_trace[i].SwitchFormat();
    }

    auto ctxt_trace = std::make_shared<CiphertextImpl<DCRTPoly>>(context_trace);
    ctxt_trace->SetElements({poly_trace[0], poly_trace[1]});
    ctxt_trace->SetKeyTag(traceKeyTag);
    ctxt_trace->SetEncodingType(PACKED_ENCODING);
    digest = context_trace->Compress(ctxt_trace);
}

}  // namespace

lbcrypto::Ciphertext<lbcrypto::DCRTPoly> compress(
                std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>& PV,
                const std::vector<std::vector<uint32_t>>& payloads,
                const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context_comp,
                const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context_trace,
                const lbcrypto::EvalKey<lbcrypto::DCRTPoly>& swk,
                const std::string& traceKeyTag)
{
    using namespace std;
    using namespace lbcrypto;

    auto vandermonde = buildVandermondeMatrix(num_pertinent, payloads.size());

    Ciphertext<DCRTPoly> digest;

    auto context = PV[0]->GetCryptoContext();
    for(int i = 0; i < numctxt; i++){
        PV[i] = context->Compress(PV[i], 2);
        context->GetScheme()->KeySwitchInPlace(PV[i], swk);
        // Re-wrap in context_comp
        auto ctxt_temp = std::make_shared<CiphertextImpl<DCRTPoly>>(context_comp);
        ctxt_temp->SetElements(PV[i]->GetElements());
        ctxt_temp->SetKeyTag(swk->GetKeyTag());
        ctxt_temp->SetEncodingType(PACKED_ENCODING);
        PV[i] = ctxt_temp;
    }

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
    ringSwitch(digest, context_trace, traceKeyTag);

    return digest;
}
