#!/usr/bin/env python3
"""
plot_phase_e.py — Visualização dos diagnósticos da Fase E PIC 1D.

Requer: numpy, matplotlib
Uso:    python3 scripts/plot_phase_e.py
Entrada: data/output/poisson1d_convergence.csv
         data/output/langmuir1d_summary.csv
Saída:   docs/figures/poisson1d_convergence.png
"""

import csv
import math
import os
import sys

def read_csv(path):
    with open(path, newline="") as f:
        reader = csv.DictReader(f)
        return list(reader)

def print_summary(rows, title):
    print(f"\n{'='*60}")
    print(f"  {title}")
    print('='*60)
    if rows:
        header = list(rows[0].keys())
        widths = [max(len(h), max(len(r[h]) for r in rows)) + 2 for h in header]
        fmt = "  ".join(f"{{:<{w}}}" for w in widths)
        print(fmt.format(*header))
        print("-" * (sum(widths) + 2 * (len(widths) - 1)))
        for row in rows:
            print(fmt.format(*[row[h] for h in header]))

def main():
    csv_conv     = "data/output/poisson1d_convergence.csv"
    csv_langmuir = "data/output/langmuir1d_summary.csv"

    missing = [p for p in (csv_conv, csv_langmuir) if not os.path.exists(p)]
    if missing:
        print(f"Arquivo(s) não encontrado(s): {missing}")
        print("Execute primeiro: ./build/pidc_phase_e_diagnostics")
        sys.exit(1)

    conv_rows     = read_csv(csv_conv)
    langmuir_rows = read_csv(csv_langmuir)

    print_summary(conv_rows,     "Poisson 1D MMS — Convergência")
    print_summary(langmuir_rows, "Langmuir 1D — Resultado")

    try:
        import matplotlib
        matplotlib.use("Agg")
        import matplotlib.pyplot as plt
        import numpy as np
    except ImportError:
        print("\nmastplotlib/numpy não disponíveis — pulando geração de figuras.")
        return

    os.makedirs("docs/figures", exist_ok=True)

    # --- Convergência log-log ---
    nx_vals   = [int(r["nx"])         for r in conv_rows]
    linf_phi  = [float(r["linf_phi"]) for r in conv_rows]
    linf_E    = [float(r["linf_E"])   for r in conv_rows]

    h_vals = [1.0 / nx for nx in nx_vals]
    ref_scale_phi = linf_phi[0] / h_vals[0]**2
    ref_scale_E   = linf_E[0]   / h_vals[0]**2
    ref_line      = [ref_scale_phi * h**2 for h in h_vals]

    fig, ax = plt.subplots(figsize=(6, 4))
    ax.loglog(h_vals, linf_phi, "o-", label=r"$L^\infty(\phi)$")
    ax.loglog(h_vals, linf_E,   "s-", label=r"$L^\infty(E)$")
    ax.loglog(h_vals, ref_line, "k--", alpha=0.5, label=r"$O(h^2)$")
    ax.set_xlabel(r"$h = L/n_x$")
    ax.set_ylabel(r"$L^\infty$ error")
    ax.set_title("Poisson 1D periódico — Convergência MMS")
    ax.legend()
    ax.grid(True, which="both", alpha=0.3)
    fig.tight_layout()

    out_path = "docs/figures/poisson1d_convergence.png"
    fig.savefig(out_path, dpi=150)
    print(f"\nFigura salva: {out_path}")
    plt.close(fig)

if __name__ == "__main__":
    main()
