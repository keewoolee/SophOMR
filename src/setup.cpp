#include "setup.h"
#include "global.h"
#include "signal.h"

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

void updateTraceInfo(const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context_comp,
                        const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context_trace,
                        const lbcrypto::KeyPair<lbcrypto::DCRTPoly>& keyPair_comp,
                        const lbcrypto::KeyPair<lbcrypto::DCRTPoly>& keyPair_trace)
{
    using namespace std;
    using namespace lbcrypto;

    vector<int64_t> vec(degree,0);
    for (int i = 0; i < degree_trace_half; i++) {
        vec[i] = i * dim_trace;
        vec[i + degree_half] = (i + degree_trace_half)* dim_trace;
    }
    Plaintext ptxt = context_comp->MakePackedPlaintext(vec);
    Ciphertext<DCRTPoly> ctxt_comp = context_comp->Encrypt(keyPair_comp.secretKey, ptxt);
    ctxt_comp = context_comp->Compress(ctxt_comp);

    vector<int64_t> vec_null(degree_trace);
    auto ptxt_null = context_trace->MakePackedPlaintext(vec_null);
    auto ctxt_trace = context_trace->Encrypt(keyPair_trace.secretKey, ptxt_null);
    ctxt_trace = context_trace->Compress(ctxt_trace);

    auto poly_comp = ctxt_comp->GetElements();
    auto poly_trace = ctxt_trace->GetElements();
    for (int i = 0; i < 2; i++) {
        poly_comp[i].SwitchFormat();
        poly_trace[i].SwitchFormat();
    }
    for (int i = 0; i < 2; i++) {
        NativePoly poly_comp_ = poly_comp[i].GetElementAtIndex(0);
        NativePoly poly_trace_ = poly_trace[i].GetElementAtIndex(0);
        for (size_t k = 0; k < poly_trace_.GetLength(); k++) {
            poly_trace_[k] = poly_comp_[dim_trace * k].Mod(poly_trace_.GetModulus());
        }
        poly_trace[i].SetElementAtIndex(0, poly_trace_);
    }
    for (int i = 0; i < 2; i++) {
        poly_trace[i].SwitchFormat();
    }
    ctxt_trace->SetElements(poly_trace);

    Plaintext ptxt_res;
    context_trace->Decrypt(keyPair_trace.secretKey, ctxt_trace, &ptxt_res);
    vector<int64_t> vec_res = ptxt_res->GetPackedValue();

    trace_swap = vec_res[0] / degree_trace_half;
    trace_shift = vec_res[0] % degree_trace_half;
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
