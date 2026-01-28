#include "decode.h"
#include "global.h"

#include <NTL/ZZ_pXFactoring.h>

bool compare_ZZ_p(NTL::ZZ_p i, NTL::ZZ_p j) { return (NTL::conv<int>(i) < NTL::conv<int>(j)); }

void decodeIdx(std::vector<NTL::ZZ_p>& idx_ZZ_p, const std::vector<int64_t>& digest_idx)
{
    int s = digest_idx.size();

    NTL::ZZ_pX poly;
    poly.SetLength(s+1);

    poly[s] = 1;
    for (int i = 1; i <= s; i++) {
        poly[s-i] = 0;
        for (int j = 1; j <= i; j++) {
            if (j%2 == 1) {
                poly[s-i] += poly[s-i+j] * digest_idx[j-1];
            } else {
                poly[s-i] -= poly[s-i+j] * digest_idx[j-1];
            }
        }
        poly[s-i] = poly[s-i]/i;
    }

    for (int i = 1; i <= s; i++) {
        if (i%2 == 1) {
            poly[s-i] = -poly[s-i];
        }
    }

    while (poly[0] == 0) {
        poly >>= 1;
    }

    NTL::Vec<NTL::Pair<NTL::ZZ_pX, long>> factors;
    CanZass(factors, poly);

    int ss = factors.length();
    idx_ZZ_p.resize(ss);
    for (int i = 0; i < ss; i++) {
        idx_ZZ_p[i] = -factors[i].a[0];
    }
    std::sort(idx_ZZ_p.begin(), idx_ZZ_p.end(), compare_ZZ_p);
}

void decodePayload(std::vector<std::vector<uint32_t>>& decodedPayload,
                    const std::vector<std::vector<int64_t>>& digest_payload,
                    const std::vector<NTL::ZZ_p>& idx_ZZ_p)
{
    int payload_len = digest_payload.size();
    int ss = idx_ZZ_p.size();

    NTL::mat_ZZ_p vandermonde;
    vandermonde.SetDims(ss, ss);

    for (int j = 0; j < ss; j++) {
        vandermonde[0][j] = idx_ZZ_p[j];
    }
    for (int i = 1; i < ss; i++) {
        for (int j = 0; j < ss; j++) {
            vandermonde[i][j] = (vandermonde[i-1][j] * idx_ZZ_p[j]);
        }
    }

    NTL::mat_ZZ_p vandermonde_inv = inv(vandermonde);

    std::vector<NTL::vec_ZZ_p> payload_ZZ_p(payload_len);
    NTL::vec_ZZ_p temp;
    temp.SetLength(ss);

    for (int i = 0; i < payload_len; i++) {
        for (int j = 0; j < ss; j++) {
            temp[j] = digest_payload[i][j];
        }
        payload_ZZ_p[i] = vandermonde_inv * temp;
    }

    decodedPayload.resize(ss);
    for (int i = 0; i < ss; i++) {
        decodedPayload[i].resize(payload_len);
        for (int j = 0; j < payload_len; j++) {
            decodedPayload[i][j] = NTL::conv<int>(payload_ZZ_p[j][i]);
        }
    }
}

void decode(std::vector<int>& decodedIdx,
                std::vector<std::vector<uint32_t>>& decodedPayload,
                const lbcrypto::Ciphertext<lbcrypto::DCRTPoly>& digest,
                const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context,
                const lbcrypto::PrivateKey<lbcrypto::DCRTPoly>& HEsk)
{
    lbcrypto::Plaintext digest_pt;
    context->Decrypt(HEsk, digest, &digest_pt);
    std::vector<int64_t> digest_vec = digest_pt->GetPackedValue();

    if (trace_swap == 1) {
        for (int i = 0; i < degree_trace_half; i++) {
            std::swap(digest_vec[i], digest_vec[i + degree_trace_half]);
        }
    }
    rotate(digest_vec.begin(), digest_vec.begin() + degree_trace_half - trace_shift, digest_vec.begin() + degree_trace_half);
    rotate(digest_vec.begin() + degree_trace_half, digest_vec.begin() + degree_trace - trace_shift, digest_vec.begin() + degree_trace);

    std::vector<int64_t> digest_idx(num_pertinent);
    std::vector<std::vector<int64_t>> digest_payload(payload_len, std::vector<int64_t>(num_pertinent));
    for (int j = 0; j < num_pertinent; j++) {
            digest_idx[j] = digest_vec[j];
    }
    for (int i = 0; i < payload_len; i++) {
        for (int j = 0; j < num_pertinent; j++) {
            digest_payload[i][j] = digest_vec[(i+1) * num_pertinent + j];
        }
    }

    NTL::ZZ p(ptxt_modulus);
    NTL::ZZ_p::init(p);
    std::vector<NTL::ZZ_p> idx_ZZ_p;

    decodeIdx(idx_ZZ_p, digest_idx);
    decodePayload(decodedPayload, digest_payload, idx_ZZ_p);

    decodedIdx.resize(idx_ZZ_p.size());
    for (size_t i = 0; i < decodedIdx.size(); i++) {
        decodedIdx[i] = NTL::conv<int>(idx_ZZ_p[i]) - 1;
    }
}

void checkResult(const std::vector<int>& decodedIdx,
                    const std::vector<std::vector<uint32_t>>& decodedPayload,
                    const std::vector<int>& pertinentIdx,
                    const std::vector<std::vector<uint32_t>>& payloads)
{
    std::vector<std::vector<uint32_t>> pertinentPayload(pertinentIdx.size(), std::vector<uint32_t>(payload_len));
    for (size_t i = 0; i < pertinentIdx.size(); i++) {
        pertinentPayload[i] = payloads[pertinentIdx[i]];
    }

    if (pertinentIdx == decodedIdx && pertinentPayload == decodedPayload) {
        std::cout << "Result is Correct!" << std::endl;
    } else {
        std::cout << "Something went Wrong!" << std::endl;
    }
}
