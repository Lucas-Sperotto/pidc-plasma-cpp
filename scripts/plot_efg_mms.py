#!/usr/bin/env python3
"""
plot_efg_mms.py — Visualização da convergência MMS EFG Poisson 2D.

Requer: numpy, matplotlib (opcional — fallback textual sem eles)
Uso:    python3 scripts/plot_efg_mms.py   (da raiz do repositório)
Entrada: data/output/efg_mms_convergence.csv
Saída:   docs/figures/efg_mms_convergence.png
         docs/figures/efg_mms_orders.png
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
    print(f"\n{'='*70}")
    print(f"  {title}")
    print('=' * 70)
    if rows:
        header = list(rows[0].keys())
        widths = [max(len(h), max(len(r[h]) for r in rows)) + 2 for h in header]
        fmt = "  ".join(f"{{:<{w}}}" for w in widths)
        print(fmt.format(*header))
        print("-" * (sum(widths) + 2 * (len(widths) - 1)))
        for row in rows:
            print(fmt.format(*[row[h] for h in header]))


def main():
    csv_path = "data/output/efg_mms_convergence.csv"

    if not os.path.exists(csv_path):
        print(f"Arquivo não encontrado: {csv_path}")
        print("Execute primeiro: ./build/pidc_marco3_mms_diagnostics")
        sys.exit(1)

    rows = read_csv(csv_path)
    print_summary(rows, "EFG Poisson MMS 2D — Convergência")

    try:
        import matplotlib
        matplotlib.use("Agg")
        import matplotlib.pyplot as plt
    except ImportError:
        print("\nmatplotlib não disponível — pulando geração de figuras.")
        return

    os.makedirs("docs/figures", exist_ok=True)

    # Extrair dados numéricos
    n_vals      = [int(r["n"])              for r in rows]
    h_vals      = [float(r["h"])            for r in rows]
    pot_l2      = [float(r["pot_l2"])       for r in rows]
    fld_l2      = [float(r["fld_l2"])       for r in rows]
    order_pot   = [float(r["order_pot_l2"]) for r in rows]
    order_fld   = [float(r["order_fld_l2"]) for r in rows]

    # --- Figura 1: convergência log-log ---
    ref_h2 = [pot_l2[0] * (h / h_vals[0]) ** 2 for h in h_vals]
    ref_h1 = [fld_l2[0] * (h / h_vals[0]) ** 1 for h in h_vals]

    fig, ax = plt.subplots(figsize=(6, 4))
    ax.loglog(h_vals, pot_l2, "o-", label=r"$L^2(\phi)$")
    ax.loglog(h_vals, fld_l2, "s-", label=r"$L^2(E)$")
    ax.loglog(h_vals, ref_h2, "k--", alpha=0.5, label=r"$O(h^2)$")
    ax.loglog(h_vals, ref_h1, "k:",  alpha=0.5, label=r"$O(h)$")
    ax.set_xlabel(r"$h = 1/(n-1)$")
    ax.set_ylabel(r"Erro $L^2$")
    ax.set_title("EFG Poisson 2D — Convergência MMS")
    ax.legend()
    ax.grid(True, which="both", alpha=0.3)
    fig.tight_layout()
    out1 = "docs/figures/efg_mms_convergence.png"
    fig.savefig(out1, dpi=150)
    plt.close(fig)
    print(f"\nFigura salva: {out1}")

    # --- Figura 2: ordens de convergência por par ---
    pair_labels = [f"{n_vals[i-1]}→{n_vals[i]}" for i in range(1, len(n_vals))]
    ord_pot_bars = [order_pot[i] for i in range(1, len(order_pot))]
    ord_fld_bars = [order_fld[i] for i in range(1, len(order_fld))]

    x = list(range(len(pair_labels)))
    width = 0.35

    fig, ax = plt.subplots(figsize=(7, 4))
    bars1 = ax.bar([xi - width / 2 for xi in x], ord_pot_bars, width, label=r"ord $L^2(\phi)$")
    bars2 = ax.bar([xi + width / 2 for xi in x], ord_fld_bars, width, label=r"ord $L^2(E)$")
    ax.axhline(y=2.0, color="k", linestyle="--", alpha=0.5, label="O(h²) teórico")
    ax.axhline(y=1.0, color="gray", linestyle=":",  alpha=0.5, label="O(h) teórico")
    ax.set_xticks(x)
    ax.set_xticklabels(pair_labels)
    ax.set_xlabel("Par de refinamento (n)")
    ax.set_ylabel("Ordem de convergência")
    ax.set_title("EFG Poisson 2D — Ordens de convergência MMS")
    ax.legend()
    ax.grid(axis="y", alpha=0.3)
    fig.tight_layout()
    out2 = "docs/figures/efg_mms_orders.png"
    fig.savefig(out2, dpi=150)
    plt.close(fig)
    print(f"Figura salva: {out2}")


if __name__ == "__main__":
    main()
