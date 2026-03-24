#pragma once

#include "openfhe.h"
#include "setup.h"

struct DecodeResult {
    std::vector<int> indices;
    std::vector<std::vector<uint32_t>> payloads;
};

DecodeResult decode(const lbcrypto::Ciphertext<lbcrypto::DCRTPoly>& digest,
                const lbcrypto::PrivateKey<lbcrypto::DCRTPoly>& HEsk);

void checkResult(const DecodeResult& decoded, const GroundTruth& groundTruth);
