#include "param.h"
#include "global.h"

void param_OMR_65536_50_250()
{
    num_transaction = 65536;
    num_pertinent = 50;

    ptxt_modulus = 786433;
    degree = 65536;

    NumLargeDigits = 2;
    ScalingModSize = 60;
    MultiplicativeDepth = 28;

    NumLargeDigits_comp = 1;
    MultiplicativeDepth_comp = 1;
    dim_trace = 4;

    PSparam.n = 1024;
    PSparam.q = ptxt_modulus;
    PSparam.ell = 2;
    PSparam.h = 80;
    PSparam.sigma = 0.5;

    coeff_rangeCheck =
    {{0,216615,237416,741379,99137,105134,235513,643136},
    {498247,88199,302058,515923,339325,161996,88430,643211},
    {342302,233360,731664,655413,219378,239548,347203,445370},
    {361791,80486,126394,391207,551639,528197,478787,88772},
    {710790,716155,456692,301062,159136,300549,8286,765893},
    {1,0,0,0,0,0,0,0}};
}

void param_OMD_65536_50()
{
    payload_size = 0;

    num_transaction = 65536;
    num_pertinent = 50;

    ptxt_modulus = 786433;
    degree = 65536;

    NumLargeDigits = 2;
    ScalingModSize = 60;
    MultiplicativeDepth = 28;

    NumLargeDigits_comp = 2;
    MultiplicativeDepth_comp = 1;
    dim_trace = 8;

    PSparam.n = 1024;
    PSparam.q = ptxt_modulus;
    PSparam.ell = 2;
    PSparam.h = 80;
    PSparam.sigma = 0.5;

    coeff_rangeCheck =
    {{0,216615,237416,741379,99137,105134,235513,643136},
    {498247,88199,302058,515923,339325,161996,88430,643211},
    {342302,233360,731664,655413,219378,239548,347203,445370},
    {361791,80486,126394,391207,551639,528197,478787,88772},
    {710790,716155,456692,301062,159136,300549,8286,765893},
    {1,0,0,0,0,0,0,0}};
}

void param_OMR_524288_50_250()
{
    num_transaction = 524288;
    num_pertinent = 50;

    ptxt_modulus = 786433;
    degree = 65536;

    NumLargeDigits = 2;
    ScalingModSize = 60;
    MultiplicativeDepth = 28;

    NumLargeDigits_comp = 1;
    MultiplicativeDepth_comp = 1;
    dim_trace = 4;

    PSparam.n = 1024;
    PSparam.q = ptxt_modulus;
    PSparam.ell = 2;
    PSparam.h = 80;
    PSparam.sigma = 0.5;

    coeff_rangeCheck =
    {{0,216615,237416,741379,99137,105134,235513,643136},
    {498247,88199,302058,515923,339325,161996,88430,643211},
    {342302,233360,731664,655413,219378,239548,347203,445370},
    {361791,80486,126394,391207,551639,528197,478787,88772},
    {710790,716155,456692,301062,159136,300549,8286,765893},
    {1,0,0,0,0,0,0,0}};
}

void param_OMD_524288_50()
{
    payload_size = 0;

    num_transaction = 524288;
    num_pertinent = 50;

    ptxt_modulus = 786433;
    degree = 65536;

    NumLargeDigits = 2;
    ScalingModSize = 60;
    MultiplicativeDepth = 28;

    NumLargeDigits_comp = 2;
    MultiplicativeDepth_comp = 1;
    dim_trace = 8;

    PSparam.n = 1024;
    PSparam.q = ptxt_modulus;
    PSparam.ell = 2;
    PSparam.h = 80;
    PSparam.sigma = 0.5;

    coeff_rangeCheck =
    {{0,216615,237416,741379,99137,105134,235513,643136},
    {498247,88199,302058,515923,339325,161996,88430,643211},
    {342302,233360,731664,655413,219378,239548,347203,445370},
    {361791,80486,126394,391207,551639,528197,478787,88772},
    {710790,716155,456692,301062,159136,300549,8286,765893},
    {1,0,0,0,0,0,0,0}};
}

// OMR: Varying t (payload size)
void param_OMR_65536_50_64()
{
    num_transaction = 65536;
    num_pertinent = 50;
    payload_size = 1240;

    ptxt_modulus = 786433;
    degree = 65536;

    NumLargeDigits = 2;
    ScalingModSize = 60;
    MultiplicativeDepth = 28;

    NumLargeDigits_comp = 2;
    MultiplicativeDepth_comp = 1;
    dim_trace = 8;

    PSparam.n = 1024;
    PSparam.q = ptxt_modulus;
    PSparam.ell = 2;
    PSparam.h = 80;
    PSparam.sigma = 0.5;

    coeff_rangeCheck =
    {{0,216615,237416,741379,99137,105134,235513,643136},
    {498247,88199,302058,515923,339325,161996,88430,643211},
    {342302,233360,731664,655413,219378,239548,347203,445370},
    {361791,80486,126394,391207,551639,528197,478787,88772},
    {710790,716155,456692,301062,159136,300549,8286,765893},
    {1,0,0,0,0,0,0,0}};
}

void param_OMR_65536_50_128()
{
    num_transaction = 65536;
    num_pertinent = 50;
    payload_size = 2500;

    ptxt_modulus = 786433;
    degree = 65536;

    NumLargeDigits = 2;
    ScalingModSize = 60;
    MultiplicativeDepth = 28;

    NumLargeDigits_comp = 2;
    MultiplicativeDepth_comp = 1;
    dim_trace = 8;

    PSparam.n = 1024;
    PSparam.q = ptxt_modulus;
    PSparam.ell = 2;
    PSparam.h = 80;
    PSparam.sigma = 0.5;

    coeff_rangeCheck =
    {{0,216615,237416,741379,99137,105134,235513,643136},
    {498247,88199,302058,515923,339325,161996,88430,643211},
    {342302,233360,731664,655413,219378,239548,347203,445370},
    {361791,80486,126394,391207,551639,528197,478787,88772},
    {710790,716155,456692,301062,159136,300549,8286,765893},
    {1,0,0,0,0,0,0,0}};
}

void param_OMR_65536_50_256()
{
    num_transaction = 65536;
    num_pertinent = 50;
    payload_size = 4930;

    ptxt_modulus = 786433;
    degree = 65536;

    NumLargeDigits = 2;
    ScalingModSize = 60;
    MultiplicativeDepth = 28;

    NumLargeDigits_comp = 1;
    MultiplicativeDepth_comp = 1;
    dim_trace = 4;

    PSparam.n = 1024;
    PSparam.q = ptxt_modulus;
    PSparam.ell = 2;
    PSparam.h = 80;
    PSparam.sigma = 0.5;

    coeff_rangeCheck =
    {{0,216615,237416,741379,99137,105134,235513,643136},
    {498247,88199,302058,515923,339325,161996,88430,643211},
    {342302,233360,731664,655413,219378,239548,347203,445370},
    {361791,80486,126394,391207,551639,528197,478787,88772},
    {710790,716155,456692,301062,159136,300549,8286,765893},
    {1,0,0,0,0,0,0,0}};
}

void param_OMR_65536_50_512()
{
    num_transaction = 65536;
    num_pertinent = 50;
    payload_size = 10010;

    ptxt_modulus = 786433;
    degree = 65536;

    NumLargeDigits = 2;
    ScalingModSize = 60;
    MultiplicativeDepth = 28;

    NumLargeDigits_comp = 1;
    MultiplicativeDepth_comp = 1;
    dim_trace = 2;

    PSparam.n = 1024;
    PSparam.q = ptxt_modulus;
    PSparam.ell = 2;
    PSparam.h = 80;
    PSparam.sigma = 0.5;

    coeff_rangeCheck =
    {{0,216615,237416,741379,99137,105134,235513,643136},
    {498247,88199,302058,515923,339325,161996,88430,643211},
    {342302,233360,731664,655413,219378,239548,347203,445370},
    {361791,80486,126394,391207,551639,528197,478787,88772},
    {710790,716155,456692,301062,159136,300549,8286,765893},
    {1,0,0,0,0,0,0,0}};
}

// OMR: Varying k (num_pertinent)
void param_OMR_65536_16_250()
{
    num_transaction = 65536;
    num_pertinent = 16;

    ptxt_modulus = 786433;
    degree = 65536;

    NumLargeDigits = 2;
    ScalingModSize = 60;
    MultiplicativeDepth = 28;

    NumLargeDigits_comp = 2;
    MultiplicativeDepth_comp = 1;
    dim_trace = 8;

    PSparam.n = 1024;
    PSparam.q = ptxt_modulus;
    PSparam.ell = 2;
    PSparam.h = 80;
    PSparam.sigma = 0.5;

    coeff_rangeCheck =
    {{0,216615,237416,741379,99137,105134,235513,643136},
    {498247,88199,302058,515923,339325,161996,88430,643211},
    {342302,233360,731664,655413,219378,239548,347203,445370},
    {361791,80486,126394,391207,551639,528197,478787,88772},
    {710790,716155,456692,301062,159136,300549,8286,765893},
    {1,0,0,0,0,0,0,0}};
}

void param_OMR_65536_32_250()
{
    num_transaction = 65536;
    num_pertinent = 32;

    ptxt_modulus = 786433;
    degree = 65536;

    NumLargeDigits = 2;
    ScalingModSize = 60;
    MultiplicativeDepth = 28;

    NumLargeDigits_comp = 2;
    MultiplicativeDepth_comp = 1;
    dim_trace = 8;

    PSparam.n = 1024;
    PSparam.q = ptxt_modulus;
    PSparam.ell = 2;
    PSparam.h = 80;
    PSparam.sigma = 0.5;

    coeff_rangeCheck =
    {{0,216615,237416,741379,99137,105134,235513,643136},
    {498247,88199,302058,515923,339325,161996,88430,643211},
    {342302,233360,731664,655413,219378,239548,347203,445370},
    {361791,80486,126394,391207,551639,528197,478787,88772},
    {710790,716155,456692,301062,159136,300549,8286,765893},
    {1,0,0,0,0,0,0,0}};
}

void param_OMR_65536_64_250()
{
    num_transaction = 65536;
    num_pertinent = 64;

    ptxt_modulus = 786433;
    degree = 65536;

    NumLargeDigits = 2;
    ScalingModSize = 60;
    MultiplicativeDepth = 28;

    NumLargeDigits_comp = 1;
    MultiplicativeDepth_comp = 1;
    dim_trace = 4;

    PSparam.n = 1024;
    PSparam.q = ptxt_modulus;
    PSparam.ell = 2;
    PSparam.h = 80;
    PSparam.sigma = 0.5;

    coeff_rangeCheck =
    {{0,216615,237416,741379,99137,105134,235513,643136},
    {498247,88199,302058,515923,339325,161996,88430,643211},
    {342302,233360,731664,655413,219378,239548,347203,445370},
    {361791,80486,126394,391207,551639,528197,478787,88772},
    {710790,716155,456692,301062,159136,300549,8286,765893},
    {1,0,0,0,0,0,0,0}};
}

void param_OMR_65536_128_250()
{
    num_transaction = 65536;
    num_pertinent = 128;

    ptxt_modulus = 786433;
    degree = 65536;

    NumLargeDigits = 2;
    ScalingModSize = 60;
    MultiplicativeDepth = 28;

    NumLargeDigits_comp = 1;
    MultiplicativeDepth_comp = 1;
    dim_trace = 2;

    PSparam.n = 1024;
    PSparam.q = ptxt_modulus;
    PSparam.ell = 2;
    PSparam.h = 80;
    PSparam.sigma = 0.5;

    coeff_rangeCheck =
    {{0,216615,237416,741379,99137,105134,235513,643136},
    {498247,88199,302058,515923,339325,161996,88430,643211},
    {342302,233360,731664,655413,219378,239548,347203,445370},
    {361791,80486,126394,391207,551639,528197,478787,88772},
    {710790,716155,456692,301062,159136,300549,8286,765893},
    {1,0,0,0,0,0,0,0}};
}

// OMD: Varying k (num_pertinent)
void param_OMD_65536_16()
{
    payload_size = 0;

    num_transaction = 65536;
    num_pertinent = 16;

    ptxt_modulus = 786433;
    degree = 65536;

    NumLargeDigits = 2;
    ScalingModSize = 60;
    MultiplicativeDepth = 28;

    NumLargeDigits_comp = 2;
    MultiplicativeDepth_comp = 1;
    dim_trace = 8;

    PSparam.n = 1024;
    PSparam.q = ptxt_modulus;
    PSparam.ell = 2;
    PSparam.h = 80;
    PSparam.sigma = 0.5;

    coeff_rangeCheck =
    {{0,216615,237416,741379,99137,105134,235513,643136},
    {498247,88199,302058,515923,339325,161996,88430,643211},
    {342302,233360,731664,655413,219378,239548,347203,445370},
    {361791,80486,126394,391207,551639,528197,478787,88772},
    {710790,716155,456692,301062,159136,300549,8286,765893},
    {1,0,0,0,0,0,0,0}};
}

void param_OMD_65536_32()
{
    payload_size = 0;

    num_transaction = 65536;
    num_pertinent = 32;

    ptxt_modulus = 786433;
    degree = 65536;

    NumLargeDigits = 2;
    ScalingModSize = 60;
    MultiplicativeDepth = 28;

    NumLargeDigits_comp = 2;
    MultiplicativeDepth_comp = 1;
    dim_trace = 8;

    PSparam.n = 1024;
    PSparam.q = ptxt_modulus;
    PSparam.ell = 2;
    PSparam.h = 80;
    PSparam.sigma = 0.5;

    coeff_rangeCheck =
    {{0,216615,237416,741379,99137,105134,235513,643136},
    {498247,88199,302058,515923,339325,161996,88430,643211},
    {342302,233360,731664,655413,219378,239548,347203,445370},
    {361791,80486,126394,391207,551639,528197,478787,88772},
    {710790,716155,456692,301062,159136,300549,8286,765893},
    {1,0,0,0,0,0,0,0}};
}

void param_OMD_65536_64()
{
    payload_size = 0;

    num_transaction = 65536;
    num_pertinent = 64;

    ptxt_modulus = 786433;
    degree = 65536;

    NumLargeDigits = 2;
    ScalingModSize = 60;
    MultiplicativeDepth = 28;

    NumLargeDigits_comp = 2;
    MultiplicativeDepth_comp = 1;
    dim_trace = 8;

    PSparam.n = 1024;
    PSparam.q = ptxt_modulus;
    PSparam.ell = 2;
    PSparam.h = 80;
    PSparam.sigma = 0.5;

    coeff_rangeCheck =
    {{0,216615,237416,741379,99137,105134,235513,643136},
    {498247,88199,302058,515923,339325,161996,88430,643211},
    {342302,233360,731664,655413,219378,239548,347203,445370},
    {361791,80486,126394,391207,551639,528197,478787,88772},
    {710790,716155,456692,301062,159136,300549,8286,765893},
    {1,0,0,0,0,0,0,0}};
}

void param_OMD_65536_128()
{
    payload_size = 0;

    num_transaction = 65536;
    num_pertinent = 128;

    ptxt_modulus = 786433;
    degree = 65536;

    NumLargeDigits = 2;
    ScalingModSize = 60;
    MultiplicativeDepth = 28;

    NumLargeDigits_comp = 2;
    MultiplicativeDepth_comp = 1;
    dim_trace = 8;

    PSparam.n = 1024;
    PSparam.q = ptxt_modulus;
    PSparam.ell = 2;
    PSparam.h = 80;
    PSparam.sigma = 0.5;

    coeff_rangeCheck =
    {{0,216615,237416,741379,99137,105134,235513,643136},
    {498247,88199,302058,515923,339325,161996,88430,643211},
    {342302,233360,731664,655413,219378,239548,347203,445370},
    {361791,80486,126394,391207,551639,528197,478787,88772},
    {710790,716155,456692,301062,159136,300549,8286,765893},
    {1,0,0,0,0,0,0,0}};
}
