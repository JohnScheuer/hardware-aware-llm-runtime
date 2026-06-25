Hardware‑Aware LLM Runtime Performance Model

A hardware-calibrated analytical performance model for LLM inference, based on Roofline theory, batch-dependent reuse modeling, and empirical fitting against real GPU measurements.

This project demonstrates:

- ✅ Formal Roofline modeling
- ✅ Hardware-aware throughput prediction
- ✅ Batch-dependent memory reuse modeling
- ✅ Empirical calibration against real CUDA measurements
- ✅ Analytical derivation of optimal batch size
- ✅ Automated parameter fitting
- ✅ CSV export for visualization and validation

> This project models and predicts the optimal batch size for LLM inference on RTX 2070 using analytical performance modeling.

---

# 1. Motivation

LLM inference performance is typically memory-bound on consumer GPUs.

Optimizations such as:

- Kernel fusion
- Tensor Core utilization
- Improved L2 reuse
- Batch tuning

affect **arithmetic intensity** and therefore shift performance along the Roofline.

Instead of blind benchmarking, this project builds a formal predictive model that:

- Explains observed performance
- Learns hardware-dependent sweet spots
- Predicts optimal batch analytically
- Validates against real GPU measurements

---

# 2. Hardware & Model

## GPU
RTX 2070 (Turing, SM75)

- Peak Compute (FP16 Tensor Core): ~7.5 TFLOPs
- DRAM Bandwidth: 448 GB/s
- Assumed bandwidth efficiency: 70%

## Model
TinyLLaMA 1.1B

- FLOPs/token ≈ 0.8766 GFLOPs
- Bytes/token (naïve) ≈ 3.506 GB

---

# 3. Roofline Model

Performance is bounded by: Performance = min(ComputeRoof, MemoryRoof)
Where: MemoryRoof = ArithmeticIntensity × EffectiveBandwidth
Arithmetic intensity depends on batch-dependent reuse: AI(batch) = reuse(batch) × (FLOPs/token ÷ Bytes/token)

---

# 4. Reuse Model

To model cache reuse and saturation effects: reuse(B) = 1 + α log(B) − β (B − B0)^2


Where:

- α → locality growth factor
- β → saturation penalty
- B0 → architectural sweet spot

This captures:

- ✅ Increasing L2 reuse with batch
- ✅ Register pressure growth
- ✅ Memory contention
- ✅ Occupancy degradation
- ✅ Sweet spot behavior

---

# 5. Analytical Optimal Batch

We derive the optimum analytically:

Take derivative: d/dB reuse(B) = α/B − 2β(B − B0)
Set to zero: α/B = 2β(B − B0)
Solving yields: B_opt = (B0 + sqrt(B0² + 2α/β)) / 2


This predicts the optimal batch **without sweep or brute force**.

---

# 6. Empirical Calibration

Measured CUDA runtime (MLP fusion enabled):

| Batch | Measured tok/s |
|--------|----------------|
| 8      | 117            |
| 10     | 129.9 ✅       |
| 12     | 122            |
| 16     | 108            |

Fitted parameters:
Alpha ≈ 0.18
Beta ≈ 0.0085
B0 ≈ 10
RMS error ≈ 3.19 tok/s

Analytical optimal batch: B_opt ≈ 10.97

Observed peak: Batch 10


The model predicts the sweet spot within ~1 batch.

---

# 7. Results

The generated `batch_curve.csv` shows:

- Throughput rising with batch
- Peak near batch 10–11
- Decline after saturation
- Convergence to memory-bound baseline at extremes

This matches real GPU behavior.

---

# 8. What This Demonstrates

This project shows the ability to:

- Model hardware-aware performance analytically
- Reason about memory vs compute regimes
- Fit physical models to empirical data
- Derive optimal configurations mathematically
- Validate predictions quantitatively

This is aligned with responsibilities of:

- AI Infrastructure Engineers
- ML Systems Engineers
- Performance Engineers
- LLM Runtime Developers

---

# 9. How to Run

Compile:

```bash
rm -rf build
mkdir build
cd build
cmake ..
make
Run fitting + export: ./runtime --mode fit
Outputs: batch_curve.csv

10. Future Extensions
Compute-bound crossover modeling
Multi-GPU comparison
Automatic reuse calibration from runtime logs
Roofline visualization
JSON export for dashboards



Author João Felipe De Souza
Hardware-aware LLM runtime performance modeling project.
Focused on quantitative systems reasoning and empirical validation.

---

# License

This project is licensed under the MIT License — see the `LICENSE` file for details.