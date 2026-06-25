#pragma once

struct RooflineResult {
    double flops_per_token;
    double bytes_per_token;
    double arithmetic_intensity;
    double memory_roof_gflops;
    double compute_roof_gflops;
    double predicted_gflops;
    const char* regime;
};

RooflineResult analyze_transformer(
    int dim,
    int hidden_dim,
    int n_layers,
    double dram_bandwidth_gbps,
    double compute_peak_gflops);