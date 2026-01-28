#pragma once

#include "openfhe.h"
#include <NTL/ZZ_p.h>

void decodeIdx(std::vector<NTL::ZZ_p>& idx_ZZ_p, const std::vector<int64_t>& digest_idx);

void decodePayload(std::vector<std::vector<uint32_t>>& decodedPayload,
                    const std::vector<std::vector<int64_t>>& digest_payload,
                    const std::vector<NTL::ZZ_p>& idx_ZZ_p);

void decode(std::vector<int>& decodedIdx,
                std::vector<std::vector<uint32_t>>& decodedPayload,
                const lbcrypto::Ciphertext<lbcrypto::DCRTPoly>& digest,
                const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context,
                const lbcrypto::PrivateKey<lbcrypto::DCRTPoly>& HEsk);

void checkResult(const std::vector<int>& decodedIdx,
                    const std::vector<std::vector<uint32_t>>& decodedPayload,
                    const std::vector<int>& pertinentIdx,
                    const std::vector<std::vector<uint32_t>>& payloads);
