#pragma once

#include "openfhe.h"
#include "signal.h"

void updateGlobal();
void initBFVparam(lbcrypto::CCParams<lbcrypto::CryptoContextBFVRNS>& BFVparam);
void enable(lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context);

lbcrypto::Ciphertext<lbcrypto::DCRTPoly> encryptPSsk(const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context,
                                                        const lbcrypto::PrivateKey<lbcrypto::DCRTPoly>& HEsk,
                                                        const PSsk& PSsk);

void liftsk(lbcrypto::KeyPair<lbcrypto::DCRTPoly>& keyPair_comp, const lbcrypto::KeyPair<lbcrypto::DCRTPoly>& keyPair_trace);

void updateTraceInfo(const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context_comp,
                        const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context_trace,
                        const lbcrypto::KeyPair<lbcrypto::DCRTPoly>& keyPair_comp,
                        const lbcrypto::KeyPair<lbcrypto::DCRTPoly>& keyPair_trace);

void printParam(const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context,
                const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context_trace);

void saveKeys(const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context,
                const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context_comp,
                const lbcrypto::Ciphertext<lbcrypto::DCRTPoly>& PSsk_enc,
                const lbcrypto::EvalKey<lbcrypto::DCRTPoly>& swk);

void simulatePayloads(std::vector<std::vector<uint32_t>>& payloads);
void sampleIdx(std::vector<int>& pertinentIdx);

void simulateSignals(std::vector<std::vector<uint64_t>>& signals_a,
                        std::vector<std::vector<uint64_t>>& signals_b,
                        const std::vector<int>& pertinentIdx,
                        const PSpk& PSpk);
