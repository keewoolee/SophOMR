#!/usr/bin/env python3
"""
Benchmarking script for SophOMR/SophOMD experiments.
Runs experiments multiple times and outputs formatted results for paper tables/figures.
"""

import subprocess
import re
import numpy as np
from dataclasses import dataclass

NUM_RUNS = 5

# Regex patterns to extract timing from program output
PATTERNS = {
    "Affine": r"Affine Transform time:\s*([\d.]+)sec",
    "RangeCheck": r"RangeCheck time:\s*([\d.]+)sec",
    "Compress": r"Compression time:\s*([\d.]+)sec",
    "Decode": r"Client-side Decoding Time:\s*([\d.]+)ms",
}


@dataclass
class Stats:
    mean: float
    std: float

    def __str__(self):
        return f"{self.mean:.1f} ± {self.std:.1f}"


def compute_stats(values: list[float]) -> Stats:
    arr = np.array(values)
    return Stats(
        mean=arr.mean(),
        std=arr.std(ddof=1) if len(arr) > 1 else 0.0
    )


def run_benchmark(scheme: str, N: int, k: int, t: int = None) -> dict[str, Stats]:
    """Run benchmark NUM_RUNS times and return statistics for each metric."""

    # Build command - OMD doesn't need t parameter
    if scheme == "OMD":
        cmd = ["./build/test", scheme, str(N), str(k)]
    else:
        cmd = ["./build/test", scheme, str(N), str(k), str(t)]

    results = {name: [] for name in PATTERNS}

    for run in range(NUM_RUNS):
        proc = subprocess.run(cmd, capture_output=True, text=True, check=True)
        output = proc.stdout

        for name, pattern in PATTERNS.items():
            match = re.search(pattern, output)
            if not match:
                raise RuntimeError(f"Run {run+1}: Failed to parse '{name}' from output")
            results[name].append(float(match.group(1)))

    # Compute stats for each metric
    stats = {name: compute_stats(values) for name, values in results.items()}

    # Add derived metric: Digest = Affine + RangeCheck + Compress
    digest_values = [a + r + c for a, r, c in zip(results["Affine"], results["RangeCheck"], results["Compress"])]
    stats["Digest"] = compute_stats(digest_values)

    return stats


# =============================================================================
# Experiment Definitions
# =============================================================================

# Each experiment: (scheme, N, k, t) - t is None for OMD
EXPERIMENTS = {
    # Table 4 / Figure 2: Default configurations
    "OMR_65536_50_250": ("OMR", 65536, 50, 250),
    "OMD_65536_50": ("OMD", 65536, 50, None),

    # Table 8 / Figure 4: Large N
    "OMR_524288_50_250": ("OMR", 524288, 50, 250),
    "OMD_524288_50": ("OMD", 524288, 50, None),

    # Figure 3-a: OMR varying k
    "OMR_65536_16_250": ("OMR", 65536, 16, 250),
    "OMR_65536_32_250": ("OMR", 65536, 32, 250),
    "OMR_65536_64_250": ("OMR", 65536, 64, 250),
    "OMR_65536_128_250": ("OMR", 65536, 128, 250),

    # Figure 3-b: OMD varying k
    "OMD_65536_16": ("OMD", 65536, 16, None),
    "OMD_65536_32": ("OMD", 65536, 32, None),
    "OMD_65536_64": ("OMD", 65536, 64, None),
    "OMD_65536_128": ("OMD", 65536, 128, None),

    # Figure 3-c: OMR varying t
    "OMR_65536_50_64": ("OMR", 65536, 50, 64),
    "OMR_65536_50_128": ("OMR", 65536, 50, 128),
    "OMR_65536_50_256": ("OMR", 65536, 50, 256),
    "OMR_65536_50_512": ("OMR", 65536, 50, 512),
}

# Group experiments by figure/table
FIGURES = {
    "Table 4 / Figure 2": ["OMR_65536_50_250", "OMD_65536_50"],
    "Table 8 / Figure 4": ["OMR_524288_50_250", "OMD_524288_50"],
    "Figure 3-a (OMR, vary k)": ["OMR_65536_16_250", "OMR_65536_32_250", "OMR_65536_64_250", "OMR_65536_128_250"],
    "Figure 3-b (OMD, vary k)": ["OMD_65536_16", "OMD_65536_32", "OMD_65536_64", "OMD_65536_128"],
    "Figure 3-c (OMR, vary t)": ["OMR_65536_50_64", "OMR_65536_50_128", "OMR_65536_50_256", "OMR_65536_50_512"],
}


def main():
    # Run all experiments and cache results
    print("Running experiments...")
    results = {}
    for name, (scheme, N, k, t) in EXPERIMENTS.items():
        print(f"  {name}...")
        results[name] = run_benchmark(scheme, N, k, t)

    print("\n" + "=" * 60)

    # Output results by figure/table
    for fig_name, exp_names in FIGURES.items():
        print(f"\n[{fig_name}]")
        print("-" * 40)

        # Digest & Decode times
        print(f"\n{'Config':<18}{'Digest (s)':>18}{'std':>6}{'Decode (ms)':>18}{'std':>6}")
        for name in exp_names:
            stats = results[name]
            d, c = stats["Digest"], stats["Decode"]
            print(f"{name:<18}{d.mean:18.1f}{d.std:6.1f}{c.mean:18.1f}{c.std:6.1f}")

        # Breakdown format (for figures)
        print(f"\n{'Config':<18}{'Compress (s)':>18}{'RangeCheck (s)':>24}{'Affine (s)':>24}")
        for name in exp_names:
            s = results[name]
            print(f"{name:<18}{s['Compress'].mean:18.1f}{s['RangeCheck'].mean:24.1f}{s['Affine'].mean:24.1f}")


if __name__ == "__main__":
    main()
