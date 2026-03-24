#pragma once

#include "openfhe.h"

lbcrypto::Ciphertext<lbcrypto::DCRTPoly> compress(
                std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>& PV,
                const std::vector<std::vector<uint32_t>>& payloads,
                const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context_comp,
                const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context_trace,
                const lbcrypto::EvalKey<lbcrypto::DCRTPoly>& swk,
                const std::string& traceKeyTag);
