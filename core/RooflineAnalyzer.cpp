#include "RooflineAnalyzer.h"
#include <algorithm>

RooflineAnalyzer::RooflineAnalyzer(
    double flops_per_token_,
    double bytes_per_token_,
    double peak_compute_gflops_,
    double peak_bandwidth_gbs_,
    double reuse_factor_,
    double bandwidth_efficiency_
)
    : flops_per_token(flops_per_token_),
      bytes_per_token(bytes_per_token_),
      peak_compute_gflops(peak_compute_gflops_),
      peak_bandwidth_gbs(peak_bandwidth_gbs_),
      reuse_factor(reuse_factor_),
      bandwidth_efficiency(bandwidth_efficiency_) {}

double RooflineAnalyzer::effective_bandwidth() const {
    return peak_bandwidth_gbs * bandwidth_efficiency;
}

double RooflineAnalyzer::arithmetic_intensity() const {
    double naive_ai = flops_per_token / bytes_per_token;
    return naive_ai * reuse_factor;
}

double RooflineAnalyzer::memory_roof() const {
    return arithmetic_intensity() * effective_bandwidth();
}

double RooflineAnalyzer::compute_roof() const {
    return peak_compute_gflops;
}

double RooflineAnalyzer::predicted_gflops() const {
    return std::min(memory_roof(), compute_roof());
}

double RooflineAnalyzer::predicted_tokens_per_sec() const {
    return predicted_gflops() / flops_per_token;
}

std::string RooflineAnalyzer::regime() const {
    if (memory_roof() < compute_roof())
        return "MEMORY_BOUND";
    else
        return "COMPUTE_BOUND";
}

void RooflineAnalyzer::set_reuse_factor(double rf) {
    reuse_factor = rf;
}