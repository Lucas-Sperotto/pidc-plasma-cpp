# Log — T-MARCO3-DEFERRED

**Agente:** Claude
**Data:** 2026-05-09
**Tarefa:** Completar os três itens diferidos do Marco 3

---

## Arquivos alterados

- `CMakeLists.txt` — adicionado bloco `pidc_marco3_mms_diagnostics`
- `ROADMAP.md` — marcados três itens `[ ]` → `[x]`
- `docs/ai/AI_STATUS.md` — resumo atualizado

## Arquivos criados

- `apps/marco3_mms_diagnostics.cpp` — app diagnóstico EFG MMS convergência
- `scripts/plot_efg_mms.py` — script de plotagem (convergência log-log + barplot de ordens)
- `docs/validation/marco3_poisson_efg_validation.md` — relatório formal de validação

## Arquivos gerados (em tempo de execução)

- `data/output/efg_mms_convergence.csv` — tabela de convergência para n∈{5,7,9,13,17}
- `docs/figures/efg_mms_convergence.png` — convergência log-log L²(φ) e L²(E)
- `docs/figures/efg_mms_orders.png` — barplot de ordens por par de refinamento

---

## Comandos executados

```bash
cmake -S . -B build
cmake --build build --target pidc_marco3_mms_diagnostics
./build/pidc_marco3_mms_diagnostics
python3 scripts/plot_efg_mms.py
```

---

## Resultados

| n  | h       | L²(φ)      | L²(E)      | ord(φ) | ord(E) |
|----|---------|------------|------------|--------|--------|
|  5 | 0.2500  | 3.597e-03  | 3.305e-02  | —      | —      |
|  7 | 0.1667  | 1.515e-03  | 1.925e-02  | 2.13   | 1.33   |
|  9 | 0.1250  | 8.275e-04  | 1.365e-02  | 2.10   | 1.20   |
| 13 | 0.0833  | 3.556e-04  | 8.795e-03  | 2.08   | 1.08   |
| 17 | 0.0625  | 1.963e-04  | 6.563e-03  | 2.06   | 1.02   |

Todos os critérios de aceite passaram ✓.

---

## Testes executados

- 24/24 CTest passando (verificado ao final)
- App retorna 0 (todos os critérios internos ✓)

---

## Hipóteses assumidas

- Nenhuma: todos os valores são medidos numericamente.

---

## Pendências

- Nenhuma. Marco 3 fechado.

---

## Próximo passo sugerido

Marco 5 (PIDC pequeno): implementar `PIDCFieldInterpolator` e `PIDCLoop`
conforme T-045B e T-045C.
