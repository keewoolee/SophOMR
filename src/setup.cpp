#include "setup.h"
#include "global.h"
#include "signal.h"
#include "encoding/encodingparams.h"

#include "ciphertext-ser.h"
#include "key/key-ser.h"
#include "scheme/bfvrns/bfvrns-ser.h"

void updateGlobal()
{
    payload_len = ceil((payload_size)/log2(ptxt_modulus));
    numctxt = ceil(num_transaction/float(degree));
    degree_half = degree / 2;

    b_tilde1 = ceil(sqrt(PSparam.n * numctxt * PSparam.ell));
    g_tilde1 = ceil(PSparam.n/float(b_tilde1));

    numrow = num_pertinent * (payload_len + 1);
    for(numrow_po2 = 1; numrow_po2 < numrow; numrow_po2*=2) {}

    b_tilde2 = ceil(sqrt(numrow_po2/float(numctxt)));
    g_tilde2 = ceil(numrow_po2/float(b_tilde2));

    degree_trace = degree / dim_trace;
    degree_trace_half = degree_trace / 2;
}

void initBFVparam(lbcrypto::CCParams<lbcrypto::CryptoContextBFVRNS>& BFVparam)
{
    BFVparam.SetRingDim(degree);
    BFVparam.SetPlaintextModulus(ptxt_modulus);
    BFVparam.SetKeySwitchTechnique(lbcrypto::HYBRID);
    BFVparam.SetNumLargeDigits(NumLargeDigits);
    BFVparam.SetScalingModSize(ScalingModSize);
    BFVparam.SetMultiplicativeDepth(MultiplicativeDepth);
}

void initBFVparam_comp(lbcrypto::CCParams<lbcrypto::CryptoContextBFVRNS>& BFVparam)
{
    BFVparam.SetRingDim(degree);
    BFVparam.SetPlaintextModulus(ptxt_modulus);
    BFVparam.SetKeySwitchTechnique(lbcrypto::HYBRID);
    BFVparam.SetNumLargeDigits(NumLargeDigits_comp);
    BFVparam.SetScalingModSize(ScalingModSize);
    BFVparam.SetMultiplicativeDepth(MultiplicativeDepth_comp);
}

void initBFVparam_trace(lbcrypto::CCParams<lbcrypto::CryptoContextBFVRNS>& BFVparam)
{
    BFVparam.SetRingDim(degree_trace);
    BFVparam.SetPlaintextModulus(ptxt_modulus);
    BFVparam.SetKeySwitchTechnique(lbcrypto::HYBRID);
    BFVparam.SetNumLargeDigits(NumLargeDigits_comp);
    BFVparam.SetScalingModSize(ScalingModSize);
    BFVparam.SetMultiplicativeDepth(MultiplicativeDepth_comp);
}

void enable(lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context)
{
    context->Enable(lbcrypto::PKE);
    context->Enable(lbcrypto::KEYSWITCH);
    context->Enable(lbcrypto::LEVELEDSHE);
}

lbcrypto::Ciphertext<lbcrypto::DCRTPoly> encryptPSsk(const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context,
                                                        const lbcrypto::PrivateKey<lbcrypto::DCRTPoly>& HEsk,
                                                        const PSsk& PSsk)
{
    std::vector<int64_t> PSsk_vec(degree);
    for (int i = 0; i < degree; i++) {
        PSsk_vec[i] = PSsk[i % PSparam.n].ConvertToInt();
        if (int(PSsk_vec[i]) == PSparam.q - 1) { PSsk_vec[i] = ptxt_modulus - 1; }
    }

    auto PSsk_ptxt = context->MakePackedPlaintext(PSsk_vec);
    auto PSsk_enc = context->Encrypt(HEsk, PSsk_ptxt);

    return PSsk_enc;
}

void liftsk(lbcrypto::KeyPair<lbcrypto::DCRTPoly>& keyPair_comp, const lbcrypto::KeyPair<lbcrypto::DCRTPoly>& keyPair_trace)
{
    auto sk_comp = keyPair_comp.secretKey->GetPrivateElement();
    auto sk_trace = keyPair_trace.secretKey->GetPrivateElement();
    sk_comp.SwitchFormat();
    sk_trace.SwitchFormat();
    for (size_t i = 0; i < sk_trace.GetNumOfElements(); i++) {
        auto temp_comp = sk_comp.GetElementAtIndex(i);
        auto temp_trace = sk_trace.GetElementAtIndex(i);
        for (size_t j = 0; j < sk_comp.GetElementAtIndex(i).GetLength(); j++) {
            temp_comp[j] = 0;
        }
        for (size_t j = 0; j < sk_trace.GetElementAtIndex(i).GetLength(); j++) {
            auto coeff_trace = temp_trace[j].ConvertToInt();
            if (coeff_trace == sk_trace.GetElementAtIndex(i).GetModulus()-1) {
                temp_comp[j * dim_trace] = sk_comp.GetElementAtIndex(i).GetModulus()-1;
            } else {
                temp_comp[j * dim_trace] = coeff_trace;
            }
        }
        sk_comp.SetElementAtIndex(i, temp_comp);
    }
    sk_comp.SwitchFormat();
    keyPair_comp.secretKey->SetPrivateElement(sk_comp);
}

void injectCompatibleRoot()
{
    using namespace lbcrypto;

    uint32_t mMain = 2 * degree;
    uint32_t mTrace = 2 * degree_trace;
    NativeInteger p(ptxt_modulus);

    NativeInteger zetaMain = RootOfUnity<NativeInteger>(mMain, p);
    NativeInteger zetaTrace = zetaMain.ModExp(dim_trace, p);

    PackedEncoding::Destroy();

    auto mainParams = std::make_shared<EncodingParamsImpl>(ptxt_modulus);
    mainParams->SetPlaintextRootOfUnity(zetaMain);
    PackedEncoding::SetParams(mMain, mainParams);

    auto traceParams = std::make_shared<EncodingParamsImpl>(ptxt_modulus);
    traceParams->SetPlaintextRootOfUnity(zetaTrace);
    PackedEncoding::SetParams(mTrace, traceParams);
}

lbcrypto::CryptoContext<lbcrypto::DCRTPoly> GenCryptoContextWithModuliFrom(
    const lbcrypto::CCParams<lbcrypto::CryptoContextBFVRNS>& params,
    const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& sourceContext)
{
    using namespace lbcrypto;

    auto cc = GenCryptoContext(params);

    auto sourceElemParams = sourceContext->GetCryptoParameters()->GetElementParams();
    auto targetElemParams = cc->GetCryptoParameters()->GetElementParams();

    size_t numTowers = std::min(targetElemParams->GetParams().size(),
                                sourceElemParams->GetParams().size());

    std::vector<NativeInteger> moduli(numTowers);
    std::vector<NativeInteger> roots(numTowers);
    for (size_t i = 0; i < numTowers; i++) {
        moduli[i] = sourceElemParams->GetParams()[i]->GetModulus();
        auto sourceRoot = sourceElemParams->GetParams()[i]->GetRootOfUnity();
        roots[i] = sourceRoot.ModExp(NativeInteger(dim_trace), moduli[i]);
    }

    auto elementParams = std::make_shared<ILDCRTParams<BigInteger>>(
        2 * degree_trace, moduli, roots);

    auto cryptoParams = std::dynamic_pointer_cast<CryptoParametersBFVRNS>(
        std::const_pointer_cast<CryptoParametersBase<DCRTPoly>>(cc->GetCryptoParameters()));
    cryptoParams->SetElementParams(elementParams);
    cryptoParams->PrecomputeCRTTables(
        cryptoParams->GetKeySwitchTechnique(),
        cryptoParams->GetScalingTechnique(),
        cryptoParams->GetEncryptionTechnique(),
        cryptoParams->GetMultiplicationTechnique(),
        cryptoParams->GetNumPartQ(),
        cryptoParams->GetAuxBits(),
        cryptoParams->GetExtraBits());

    return cc;
}

void printParam(const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context,
                const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context_trace)
{
    using namespace std;

    cout << "HEparam (Top Level): \t(n, logPQ, logQ, p) \t= (" << degree << ", "
    << dynamic_pointer_cast<lbcrypto::CryptoParametersBFVRNS>(context->GetCryptoParameters())->GetParamsQP()->GetModulus().GetMSB()
    << ", " << ScalingModSize * context->GetCryptoParameters()->GetElementParams()->GetParams().size()
    << ", " << ptxt_modulus << ")" << endl;

    cout << "HEparam (Ring-Switch): \t(n, logPQ, logQ, p) \t= (" << degree_trace << ", "
    << dynamic_pointer_cast<lbcrypto::CryptoParametersBFVRNS>(context_trace->GetCryptoParameters())->GetParamsQP()->GetModulus().GetMSB()
    << ", " << ScalingModSize * context_trace->GetCryptoParameters()->GetElementParams()->GetParams().size()
    << ", " << ptxt_modulus << ")" << endl;

    cout << "PSparam: \t(n, q, ell, h, sigma) \t= ("
    << PSparam.n << ", " << PSparam.q << ", " << PSparam.ell << ", " << PSparam.h << ", " << PSparam.sigma
    << ")" << endl << endl;
}

void saveKeys(const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context,
                const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context_comp,
                const lbcrypto::Ciphertext<lbcrypto::DCRTPoly>& PSsk_enc,
                const lbcrypto::EvalKey<lbcrypto::DCRTPoly>& swk)
{
    std::ofstream mkeyfile("data/key-mult.txt", std::ios::out | std::ios::binary);
    if (mkeyfile.is_open()) {
        context->SerializeEvalMultKey(mkeyfile, lbcrypto::SerType::BINARY);
        mkeyfile.close();
    }

    std::ofstream rkeyfile("data/key-rot.txt", std::ios::out | std::ios::binary);
    if (rkeyfile.is_open()) {
        context_comp->SerializeEvalAutomorphismKey(rkeyfile, lbcrypto::SerType::BINARY);
        rkeyfile.close();
    }

    lbcrypto::Serial::SerializeToFile("data/PSsk_enc.txt", PSsk_enc, lbcrypto::SerType::BINARY);

    lbcrypto::Serial::SerializeToFile("data/swk.txt", swk, lbcrypto::SerType::BINARY);
}

namespace {

void simulatePayloads(std::vector<std::vector<uint32_t>>& payloads)
{
    auto dug = lbcrypto::DiscreteUniformGeneratorImpl<NativeVector>();
    dug.SetModulus(ptxt_modulus);

    for(int i = 0; i < num_transaction; i++) {
        for(int j = 0; j < payload_len; j++) {
            payloads[i][j] = dug.GenerateInteger().ConvertToInt();
        }
    }
}

void sampleIdx(std::vector<int>& pertinentIdx)
{
    auto dug = lbcrypto::DiscreteUniformGeneratorImpl<NativeVector>();
    dug.SetModulus(num_transaction);

    for (int i = 0; i < num_pertinent; i++) {
        uint64_t temp = dug.GenerateInteger().ConvertToInt();
        while(find(pertinentIdx.begin(), pertinentIdx.end(), temp) != pertinentIdx.end()){
            temp = dug.GenerateInteger().ConvertToInt();
        }
        pertinentIdx.push_back(temp);
    }
    sort(pertinentIdx.begin(), pertinentIdx.end());
}

void simulateSignals(std::vector<std::vector<uint64_t>>& signals_a,
                        std::vector<std::vector<uint64_t>>& signals_b,
                        const std::vector<int>& pertinentIdx,
                        const PSpk& PSpk)
{
    lbcrypto::DiscreteUniformGeneratorImpl<NativeVector> dug;
    dug.SetModulus(PSparam.q);

    int counter = 0;
    for(int i = 0; i < num_transaction; i++){
        Signal sig;
        if(counter < num_pertinent && i == pertinentIdx[counter]) {
            PSsignal(sig, PSpk, PSparam);
            counter++;
        } else {
            sig.a = dug.GenerateVector(PSparam.n);
            sig.b = dug.GenerateVector(PSparam.ell);
        }
        for (int j = 0; j < PSparam.n; j++) {
            signals_a[i][j] = sig.a[j].ConvertToInt();
        }
        for (int j = 0; j < PSparam.ell; j++) {
            signals_b[i][j] = sig.b[j].ConvertToInt();
        }
    }
}

}  // namespace

TestData generateTestData(const PSpk& PSpk)
{
    TestData data;
    data.payloads.assign(num_transaction, std::vector<uint32_t>(payload_len));
    simulatePayloads(data.payloads);

    std::vector<int> pertinentIdx;
    sampleIdx(pertinentIdx);

    data.signals.a.assign(num_transaction, std::vector<uint64_t>(PSparam.n));
    data.signals.b.assign(num_transaction, std::vector<uint64_t>(PSparam.ell));
    simulateSignals(data.signals.a, data.signals.b, pertinentIdx, PSpk);

    data.groundTruth.pertinentIdx = pertinentIdx;
    data.groundTruth.pertinentPayloads.resize(pertinentIdx.size());
    for (size_t i = 0; i < pertinentIdx.size(); i++) {
        data.groundTruth.pertinentPayloads[i] = data.payloads[pertinentIdx[i]];
    }

    return data;
}
