#include "sophomr.h"
#include "param.h"
#include <string>
#include <iostream>
#include <map>
#include <functional>

std::map<std::string, std::function<void()>> paramFuncs = {
    // OMR configurations
    {"OMR_65536_50_250", param_OMR_65536_50_250},
    {"OMR_524288_50_250", param_OMR_524288_50_250},
    {"OMR_65536_50_64", param_OMR_65536_50_64},
    {"OMR_65536_50_128", param_OMR_65536_50_128},
    {"OMR_65536_50_256", param_OMR_65536_50_256},
    {"OMR_65536_50_512", param_OMR_65536_50_512},
    {"OMR_65536_16_250", param_OMR_65536_16_250},
    {"OMR_65536_32_250", param_OMR_65536_32_250},
    {"OMR_65536_64_250", param_OMR_65536_64_250},
    {"OMR_65536_128_250", param_OMR_65536_128_250},
    // OMD configurations
    {"OMD_65536_50", param_OMD_65536_50},
    {"OMD_524288_50", param_OMD_524288_50},
    {"OMD_65536_16", param_OMD_65536_16},
    {"OMD_65536_32", param_OMD_65536_32},
    {"OMD_65536_64", param_OMD_65536_64},
    {"OMD_65536_128", param_OMD_65536_128},
};

int main(int argc, char* argv[])
{
    bool paramSet = false;

    if (argc >= 4) {
        std::string mode = argv[1];
        std::string N = argv[2];
        std::string k = argv[3];

        std::string key = mode + "_" + N + "_" + k;
        if (mode == "OMR" && argc >= 5) {
            key += "_" + std::string(argv[4]);
        }

        auto it = paramFuncs.find(key);
        if (it != paramFuncs.end()) {
            it->second();
            paramSet = true;
        }
    }

    if (!paramSet) {
        std::cout << "Usage: ./test OMR <N> <k> <t>" << std::endl;
        std::cout << "       ./test OMD <N> <k>" << std::endl;
        std::cout << "Using the default parameters set in global.h" << std::endl;
    }

    sophomr();
}
