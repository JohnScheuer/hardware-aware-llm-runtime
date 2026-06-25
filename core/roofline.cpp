#include "roofline.h"
#include <algorithm>

RooflineResult analyze_transformer(
    int dim,
    int hidden_dim,
    int n_layers,
    double dram_bandwidth_gbps,
    double compute_peak_gflops)
{
    RooflineResult r{};

    // =============================
    // FLOP ESTIMATION (decode regime)
    // =============================
    // Per layer:
    // QKV: 3 * dim * dim
    // WO: dim * dim
    // W1: dim * hidden_dim
    // W3: dim * hidden_dim
    // W2: hidden_dim * dim

    double flops_per_layer =
        4.0 * dim * dim +
        2.0 * dim * hidden_dim;

    r.flops_per_token =
        flops_per_layer * n_layers;

    // =============================
    // MEMORY TRAFFIC ESTIMATION
    // =============================
    // Approximate:
    // Read weights once per token
    // Ignore KV cache for simplicity

    double bytes_per_layer =
        sizeof(float) * (
            4.0 * dim * dim +
            2.0 * dim * hidden_dim
        );

    r.bytes_per_token =
        bytes_per_layer * n_layers;

    // =============================
    // Arithmetic Intensity
    // =============================
    r.arithmetic_intensity =
        r.flops_per_token /
        r.bytes_per_token;

    // =============================
    // Roofs
    // =============================
    r.memory_roof_gflops =
        r.arithmetic_intensity *
        dram_bandwidth_gbps;

    r.compute_roof_gflops =
        compute_peak_gflops;

    r.predicted_gflops =
        std::min(r.memory_roof_gflops,
                 r.compute_roof_gflops);

    // =============================
    // Regime classification
    // =============================
    if (r.memory_roof_gflops <
        r.compute_roof_gflops)
        r.regime = "MEMORY_BOUND";
    else
        r.regime = "COMPUTE_BOUND";

    return r;
}