#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <limits>
#include <fstream>
#include <cstring>

#include "RooflineAnalyzer.h"

// ============================================================
// Data Structures
// ============================================================

struct DataPoint {
    int batch;
    double tok;
};

struct FitResult {
    double alpha;
    double beta;
    double B0;
};

// ============================================================
// Real RTX 2070 data
// ============================================================

std::vector<DataPoint> real_data = {
    {8, 117.0},
    {10, 129.9},
    {12, 122.0},
    {16, 108.0}
};

// ============================================================
// Reuse model
// ============================================================

double reuse_model(int batch,
                   double alpha,
                   double beta,
                   double B0)
{
    if (batch <= 1) return 1.0;

    double reuse =
        1.0 +
        alpha * std::log((double)batch) -
        beta * (batch - B0) * (batch - B0);

    return std::max(1.0, reuse);
}

// ============================================================
// Analytical optimum
// ============================================================

double optimal_batch(double alpha,
                     double beta,
                     double B0)
{
    double term = B0 * B0 + (2.0 * alpha / beta);
    return (B0 + std::sqrt(term)) / 2.0;
}

// ============================================================
// Throughput prediction
// ============================================================

double predict_tok(int batch,
                   double alpha,
                   double beta,
                   double B0,
                   double flops,
                   double bytes,
                   double peak_compute,
                   double bandwidth,
                   double efficiency)
{
    double reuse = reuse_model(batch, alpha, beta, B0);

    RooflineAnalyzer analyzer(
        flops,
        bytes,
        peak_compute,
        bandwidth,
        reuse,
        efficiency
    );

    return analyzer.predicted_tokens_per_sec();
}

// ============================================================
// Grid search fitting
// ============================================================

FitResult fit_parameters(double flops,
                         double bytes,
                         double peak_compute,
                         double bandwidth,
                         double efficiency)
{
    double best_alpha = 0.0;
    double best_beta  = 0.0;
    double best_B0    = 0.0;
    double best_error = std::numeric_limits<double>::max();

    for (double alpha = 0.1; alpha <= 0.8; alpha += 0.02) {
        for (double beta = 0.0001; beta <= 0.01; beta += 0.0003) {
            for (double B0 = 6.0; B0 <= 14.0; B0 += 0.5) {

                double error = 0.0;

                for (auto& d : real_data) {

                    double pred = predict_tok(
                        d.batch,
                        alpha,
                        beta,
                        B0,
                        flops,
                        bytes,
                        peak_compute,
                        bandwidth,
                        efficiency
                    );

                    double diff = pred - d.tok;
                    error += diff * diff;
                }

                if (error < best_error) {
                    best_error = error;
                    best_alpha = alpha;
                    best_beta  = beta;
                    best_B0    = B0;
                }
            }
        }
    }

    std::cout << "\n=========== FIT RESULT ===========\n";
    std::cout << "Alpha  : " << best_alpha << "\n";
    std::cout << "Beta   : " << best_beta  << "\n";
    std::cout << "B0     : " << best_B0    << "\n";
    std::cout << "RMS    : "
              << std::sqrt(best_error / real_data.size())
              << "\n";

    double B_opt =
        optimal_batch(best_alpha,
                      best_beta,
                      best_B0);

    std::cout << "Optimal Batch (analytical) ≈ "
              << B_opt << "\n";

    return {best_alpha, best_beta, best_B0};
}

// ============================================================
// CSV Export
// ============================================================

void export_curve(FitResult fit,
                  double flops,
                  double bytes,
                  double peak_compute,
                  double bandwidth,
                  double efficiency,
                  int max_batch)
{
    std::ofstream file("batch_curve.csv");

    file << "batch,model_tok,real_tok\n";

    for (int b = 1; b <= max_batch; ++b) {

        double model = predict_tok(
            b,
            fit.alpha,
            fit.beta,
            fit.B0,
            flops,
            bytes,
            peak_compute,
            bandwidth,
            efficiency
        );

        double real = -1.0;
        for (auto& d : real_data)
            if (d.batch == b)
                real = d.tok;

        file << b << "," << model << ",";
        if (real > 0) file << real;
        file << "\n";
    }

    file.close();

    std::cout << "Exported batch_curve.csv\n";
}

// ============================================================
// CLI
// ============================================================

int main(int argc, char* argv[])
{
    std::string mode = "fit";

    for (int i = 1; i < argc; ++i)
        if (std::strcmp(argv[i], "--mode") == 0 && i + 1 < argc)
            mode = argv[++i];

    // TinyLLaMA
    double flops = 0.87661;
    double bytes = 3.50644;

    // RTX 2070
    double peak_compute = 7500.0;
    double bandwidth = 448.0;
    double efficiency = 0.7;

    if (mode == "fit") {

        FitResult fit = fit_parameters(
            flops,
            bytes,
            peak_compute,
            bandwidth,
            efficiency
        );

        export_curve(fit,
                     flops,
                     bytes,
                     peak_compute,
                     bandwidth,
                     efficiency,
                     20);
    }

    else {
        std::cout << "Unknown mode\n";
    }

    return 0;
}