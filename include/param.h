#pragma once

// Default configurations
void param_OMR_65536_50_250();
void param_OMD_65536_50();

void param_OMR_524288_50_250();
void param_OMD_524288_50();

// OMR: Varying t (payload size)
void param_OMR_65536_50_64();
void param_OMR_65536_50_128();
void param_OMR_65536_50_256();
void param_OMR_65536_50_512();

// OMR: Varying k (num_pertinent)
void param_OMR_65536_16_250();
void param_OMR_65536_32_250();
void param_OMR_65536_64_250();
void param_OMR_65536_128_250();

// OMD: Varying k (num_pertinent)
void param_OMD_65536_16();
void param_OMD_65536_32();
void param_OMD_65536_64();
void param_OMD_65536_128();
