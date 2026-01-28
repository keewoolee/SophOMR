#pragma once

#include "openfhe.h"

struct param {
    int n; int q; int ell; int h; double sigma;
    param(int n, int q, int ell, int h, double sigma)
    : n(n), q(q), ell(ell), h(h), sigma(sigma)
    {}
};

typedef NativeVector PSsk;

struct Signal {
    NativeVector a;
    NativeVector b;
};

typedef Signal PSpk;

PSsk PSskGen(const param& param);
std::vector<std::vector<uint64_t>> expand(NativeVector a, int n, int q);
NativeVector ringMult(NativeVector a, NativeVector b, int n, int q, int ell);
PSpk PSpkGen(const param& param, const PSsk& sk);
void PSsignal(Signal& sig, const PSpk& pk, const param& param);
