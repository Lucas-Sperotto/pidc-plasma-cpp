# Log — 2026-05-08 — Claude — T-034 + T-033: penalidade Dirichlet e plano esparso

## Objetivo

T-034: Revisar DEC-0024 (proposta por Gemini, T-031) e refatorar `EFGPoissonSolver`
para usar o método de penalidade para impor BCs de Dirichlet.

T-033: Planejar a migração do backend de K de denso para esparso, documentando
DEC-0025 sem implementar o solver esparso agora.

## Análise do problema na implementação anterior

`impose_dirichlet` usava eliminação sequencial de linha/coluna:

```text
Para nó i: rhs(row) -= K(row, i) * g_i  para todo row != i
           K.row(i).setZero(); K.col(i).setZero(); K(i,i) = 1; rhs(i) = g_i
```

**Bug real para Dirichlet não-homogêneo com múltiplos nós de contorno:**

Ao processar nó `j` APÓS nó `i` (que zerou a coluna `i`):
```
rhs(i) -= K(i, j) * g_j   → K(i, j) já foi zerado → contribuição perdida
```

Para `g = 0` (homogêneo), todas as atualizações são `0 * 0 = 0`: o bug não
se manifesta. O teste MMS usava somente Dirichlet homogêneo, portanto passava.

## T-034 — Implementação

### Método de penalidade (DEC-0024 aceita)

Substituição de `impose_dirichlet` por `impose_dirichlet_penalty`:

```cpp
const double max_diag = stiffness_.diagonal().cwiseAbs().maxCoeff();
const double penalty  = 1.0e12 * std::max(max_diag, 1.0);

for (cada nó de contorno i):
    stiffness_(index, index) += penalty;
    rhs_(index)              += penalty * value;
```

Propriedades:
- Cada nó é processado independentemente: sem efeitos colaterais entre nós.
- Apenas a diagonal é tocada: simetria preservada exatamente.
- Erro no contorno: `u_i - g_i = O(rhs_orig / P) ≈ O(1e-12)`.

### Ajuste no teste

`test_efg_poisson_mms.cpp`: tolerância de valor no contorno `1e-12` → `1e-10`.
O teste de simetria (`< 1e-10`) e os testes de L2 não foram alterados.

## T-033 — Planejamento (DEC-0025)

DEC-0025 registrada em `docs/ai/DECISIONS.md`. Estratégia:

```text
MatrixXd stiffness_        → SparseMatrix<double> stiffness_
FullPivLU<MatrixXd>        → SimplicialLDLT<SparseMatrix<double>>
stiffness_(row, col) +=    → Triplet list + setFromTriplets()
```

Pré-condições: DEC-0024 aceita (feita) + todos os MMS passando com denso.
Interface pública inalterada. Nenhum teste muda. Responsável: Codex (T-035).

## Arquivos alterados

| Arquivo | Operação |
| --- | --- |
| `include/pidc/efg/EFGPoissonSolver.hpp` | `impose_dirichlet` → `impose_dirichlet_penalty` |
| `tests/test_efg_poisson_mms.cpp` | tolerância contorno `1e-12` → `1e-10` |
| `docs/ai/DECISIONS.md` | DEC-0024 aceita; DEC-0025 proposta |
| `docs/ai/AI_BOARD.md` | T-034, T-033 concluídas; T-035 proposta |
| `docs/ai/AI_STATUS.md` | resumo atualizado |

## Resultado dos testes

```
14/14 testes passando
L2 5×5 = 0.00359684 (< 1e-2) ✓
L2 9×9 = 0.000827504 (< L2 5×5) ✓
Simetria max error ≈ 0 (< 1e-10) ✓
```

## Próximo passo sugerido

**T-035 (Codex)** — implementar migração densa → esparsa conforme DEC-0025,
validando que L2 5×5 e 9×9 são numericamente idênticos ao backend denso.

**T-032 (Codex)** — adicionar métricas L∞ e erro de campo ao teste MMS.
