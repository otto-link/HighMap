# FFT vs. GPU Convolution and Morphological Expansion

This document records the experimental evaluation of 2D Fast Fourier Transform (FFT) algorithms as potential alternatives to spatial GPU implementations for convolution and morphological operations.

Related issue: [#528](https://github.com/ottolink-dev/HighMap/issues/528).

## 1. Context and Motivation

Spatial filtering on large kernels can become computationally demanding with direct convolution, which scales with the kernel area $\mathcal{O}(K^2)$. In contrast, frequency-domain convolution via FFT operates in $\mathcal{O}(N \log N)$ independent of kernel radius:

$$f * g = \mathcal{F}^{-1}\{\mathcal{F}\{f\} \cdot \mathcal{F}\{g\}\}$$

Similarly, morphological expansion (dilation / maximum filter) can theoretically be approximated in the Fourier domain via a soft-max formulation:

$$\max_i x_i \approx \frac{1}{\beta} \ln \sum_i e^{\beta x_i}$$

which can be evaluated using FFT convolutions of exponentiated fields.

Two experiments were conducted to evaluate whether FFT-based methods provide performance or scaling advantages over existing GPU implementations:
1. FFT 2D convolution (`convolve2d_fft`) vs. GPU separable convolution (`smooth_cpulse`).
2. FFT soft-max morphological expansion (`expand_soft_fft`) vs. GPU morphological expansion (`expand`).

## 2. Experiment 1: 2D Convolution

Comparison of `convolve2d_fft` against `smooth_cpulse` across various mesh resolutions and filter radii. Execution times are in milliseconds (ms).

| Mesh size | Radius | Kernel size | FFT convolution (ms) | GPU separable (ms) | FFT / GPU ratio |
|---|---|---|---|---|---|
| 128×128 | 8 | 17×17 | 4.99 | 0.43 | 11.53× |
| 128×128 | 16 | 33×33 | 12.73 | 0.62 | 20.39× |
| 128×128 | 32 | 65×65 | 12.81 | 0.59 | 21.54× |
| 128×128 | 64 | 129×129 | 17.76 | 0.93 | 19.17× |
| 128×128 | 128 | 257×257 | 13.26 | 0.89 | 14.91× |
| 256×256 | 8 | 17×17 | 6.53 | 0.96 | 6.82× |
| 256×256 | 16 | 33×33 | 6.71 | 1.05 | 6.40× |
| 256×256 | 32 | 65×65 | 6.48 | 1.19 | 5.46× |
| 256×256 | 64 | 129×129 | 10.54 | 1.50 | 7.04× |
| 256×256 | 128 | 257×257 | 17.54 | 2.06 | 8.51× |
| 512×512 | 8 | 17×17 | 10.59 | 2.68 | 3.95× |
| 512×512 | 16 | 33×33 | 9.21 | 2.94 | 3.13× |
| 512×512 | 32 | 65×65 | 10.86 | 3.55 | 3.06× |
| 512×512 | 64 | 129×129 | 15.48 | 4.74 | 3.27× |
| 512×512 | 128 | 257×257 | 33.06 | 7.68 | 4.31× |

### Findings

The FFT implementation is consistently slower across all tested configurations, with a performance penalty ranging from $3\times$ to $22\times$.

While the performance gap narrows as the mesh size increases, the GPU separable convolution remains significantly faster even at $512\times512$. For separable filters (such as standard smooth pulses and Gaussian-like kernels), the $\mathcal{O}(2K)$ complexity of separable passes combined with GPU memory bandwidth efficiency outperforms the forward and inverse FFT transform pipeline.

Conclusion: FFT-based convolution does not provide a performance advantage over the existing GPU separable convolution.

## 3. Experiment 2: Morphological Expansion

Comparison of GPU morphological expansion (`expand`) against FFT soft-max expansion (`expand_soft_fft`). Execution times are in milliseconds (ms).

| Mesh size | Radius | Kernel size | GPU expand (ms) | Soft-max FFT (ms) | GPU / FFT ratio |
|---|---|---|---|---|---|
| 128×128 | 8 | 17×17 | 0.47 | 5.25 | 0.09× |
| 128×128 | 16 | 33×33 | 1.06 | 5.15 | 0.21× |
| 128×128 | 32 | 65×65 | 2.82 | 6.68 | 0.42× |
| 128×128 | 64 | 129×129 | 10.05 | 8.17 | 1.23× |
| 128×128 | 128 | 257×257 | 36.68 | 11.97 | 3.06× |
| 256×256 | 8 | 17×17 | 1.27 | 6.43 | 0.20× |
| 256×256 | 16 | 33×33 | 3.14 | 7.61 | 0.41× |
| 256×256 | 32 | 65×65 | 9.97 | 18.51 | 0.54× |
| 256×256 | 64 | 129×129 | 37.03 | 9.02 | 4.11× |
| 256×256 | 128 | 257×257 | 144.48 | 15.02 | 9.62× |
| 512×512 | 8 | 17×17 | 3.27 | 14.95 | 0.22× |
| 512×512 | 16 | 33×33 | 7.02 | 14.72 | 0.48× |
| 512×512 | 32 | 65×65 | 21.71 | 20.79 | 1.04× |
| 512×512 | 64 | 129×129 | 86.06 | 17.83 | 4.83× |
| 512×512 | 128 | 257×257 | 337.00 | 31.38 | 10.74× |

### Findings

The FFT soft-max implementation achieves a substantial computational speedup for large radii, reaching up to $10.74\times$ faster than direct GPU expansion at radius 128 on $512\times512$. Direct spatial expansion scales non-linearly with radius because morphological operations are generally non-separable in 2D.

However, the soft-max approximation introduces major quality deficiencies:
- The smooth exponential soft-max does not reproduce the sharp plateau and boundary characteristics of an exact morphological dilation.
- Tuning the sharpness parameter $\beta$ leads to numerical overflow or underflow issues in floating-point ranges without fully matching the discrete maximum.
- For small and moderate radii, the constant overhead of FFT transforms outweighs any scaling benefit.

Conclusion: FFT soft-max expansion is computationally faster for very large radii, but the loss of output accuracy and fidelity makes it unsuitable as a replacement for morphological operations.

## 4. Overall Decision

Neither FFT approach is adopted into the production pipeline:

- **Convolution**: Retain the GPU separable implementation (`smooth_cpulse`).
- **Morphological Expansion**: Retain the GPU spatial implementation (`expand`).

Re-evaluating FFT-based approximations would only be justified if an alternative formulation eliminates soft-max approximation error without sacrificing execution speed.
