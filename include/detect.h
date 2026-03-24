#pragma once

#include "openfhe.h"
#include "setup.h"

std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>> detect(
                const Signals& signals,
                const lbcrypto::Ciphertext<lbcrypto::DCRTPoly>& PSsk_enc);
