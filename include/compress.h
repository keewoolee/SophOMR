#pragma once

#include "openfhe.h"

void computeVandermonde(std::vector<std::vector<uint64_t>>& vandermonde);

void ringSwitch(lbcrypto::Ciphertext<lbcrypto::DCRTPoly>& digest,
                const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context_comp,
                const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context_trace,
                const lbcrypto::PublicKey<lbcrypto::DCRTPoly>& publicKey_trace);

void compress(lbcrypto::Ciphertext<lbcrypto::DCRTPoly>& digest,
                std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>& PV,
                const std::vector<std::vector<uint32_t>>& payloads,
                const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context_comp,
                const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context_trace,
                const lbcrypto::PublicKey<lbcrypto::DCRTPoly>& publicKey_comp,
                const lbcrypto::PublicKey<lbcrypto::DCRTPoly>& publicKey_trace,
                const lbcrypto::PrivateKey<lbcrypto::DCRTPoly>& privateKey_comp);
