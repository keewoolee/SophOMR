#pragma once

#include "openfhe.h"
#include "encoding/encodingparams.h"
#include "signal.h"

void updateGlobal();
void initBFVparam(lbcrypto::CCParams<lbcrypto::CryptoContextBFVRNS>& BFVparam);
void initBFVparam_comp(lbcrypto::CCParams<lbcrypto::CryptoContextBFVRNS>& BFVparam);
void initBFVparam_trace(lbcrypto::CCParams<lbcrypto::CryptoContextBFVRNS>& BFVparam);
void enable(lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context);

lbcrypto::Ciphertext<lbcrypto::DCRTPoly> encryptPSsk(const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context,
                                                        const lbcrypto::PrivateKey<lbcrypto::DCRTPoly>& HEsk,
                                                        const PSsk& PSsk);

void liftsk(lbcrypto::KeyPair<lbcrypto::DCRTPoly>& keyPair_comp, const lbcrypto::KeyPair<lbcrypto::DCRTPoly>& keyPair_trace);

void injectCompatibleRoot();
lbcrypto::CryptoContext<lbcrypto::DCRTPoly> GenCryptoContextWithModuliFrom(
    const lbcrypto::CCParams<lbcrypto::CryptoContextBFVRNS>& params,
    const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& sourceContext);

void printParam(const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context,
                const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context_trace);

void saveKeys(const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context,
                const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context_comp,
                const lbcrypto::Ciphertext<lbcrypto::DCRTPoly>& PSsk_enc,
                const lbcrypto::EvalKey<lbcrypto::DCRTPoly>& swk);

struct Signals {
    std::vector<std::vector<uint64_t>> a;
    std::vector<std::vector<uint64_t>> b;
};

struct GroundTruth {
    std::vector<int> pertinentIdx;
    std::vector<std::vector<uint32_t>> pertinentPayloads;
};

struct TestData {
    Signals signals;
    std::vector<std::vector<uint32_t>> payloads;
    GroundTruth groundTruth;
};

TestData generateTestData(const PSpk& PSpk);
