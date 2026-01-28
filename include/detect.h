#pragma once

#include "openfhe.h"

void affine(std::vector<std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>>& output,
            const std::vector<std::vector<uint64_t>>& signals_a,
            const std::vector<std::vector<uint64_t>>& signals_b,
            const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context,
            const lbcrypto::Ciphertext<lbcrypto::DCRTPoly>& PSsk_enc);

void powers(std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>& inplace, const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context);

void patersonStockmeyer(lbcrypto::Ciphertext<lbcrypto::DCRTPoly>& inplace,
                            const std::vector<std::vector<uint32_t>>& coeff,
                            const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context);

void FLT(lbcrypto::Ciphertext<lbcrypto::DCRTPoly>& inplace, const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context);

lbcrypto::Ciphertext<lbcrypto::DCRTPoly> product(std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>& input,
                                                    const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context);

void rangeCheck(std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>& output,
                    std::vector<std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>>& input,
                    const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context,
                    const lbcrypto::EvalKey<lbcrypto::DCRTPoly>& swk);

void detect(std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>& output,
                const std::vector<std::vector<uint64_t>>& signals_a,
                const std::vector<std::vector<uint64_t>>& signals_b,
                const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context,
                const lbcrypto::Ciphertext<lbcrypto::DCRTPoly>& PSsk_enc,
                const lbcrypto::EvalKey<lbcrypto::DCRTPoly>& swk);
