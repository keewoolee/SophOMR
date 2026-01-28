#pragma once

#include "signal.h"

extern int payload_size;

extern int num_transaction;
extern int num_pertinent;

extern int ptxt_modulus;
extern int degree;

extern int NumLargeDigits;
extern int ScalingModSize;
extern int MultiplicativeDepth;

extern int NumLargeDigits_comp;
extern int MultiplicativeDepth_comp;
extern int dim_trace;

extern int payload_len;
extern int numctxt;
extern int degree_half;
extern int b_tilde1, g_tilde1;
extern int numrow, numrow_po2;
extern int b_tilde2, g_tilde2;
extern int degree_trace;
extern int degree_trace_half;
extern int trace_swap;
extern int trace_shift;

extern param PSparam;

extern std::vector<std::vector<uint32_t>> coeff_rangeCheck;
