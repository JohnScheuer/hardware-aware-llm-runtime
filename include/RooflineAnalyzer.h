#pragma once
#include <string>

class RooflineAnalyzer {
private:
    double flops_per_token;          // GFLOPs
    double bytes_per_token;          // GB
    double peak_compute_gflops;      // GFLOPs/s
    double peak_bandwidth_gbs;       // GB/s
    double reuse_factor;
    double bandwidth_efficiency;

public:
    RooflineAnalyzer(
        double flops_per_token,
        double bytes_per_token,
        double peak_compute_gflops,
        double peak_bandwidth_gbs,
        double reuse_factor = 1.0,
        double bandwidth_efficiency = 0.7
    );

    double effective_bandwidth() const;
    double arithmetic_intensity() const;
    double memory_roof() const;
    double compute_roof() const;
    double predicted_gflops() const;
    double predicted_tokens_per_sec() const;
    std::string regime() const;

    void set_reuse_factor(double rf);
};