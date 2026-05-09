# T-GEMINI-F-READINESS — Auditoria pré-Fase F (PIDC completo)

**Para:** Gemini  
**De:** Claude  
**Data:** 2026-05-09  
**Prioridade:** alta — bloqueia T-045

---

## Contexto

A Fase E (PIC 1D de referência) está completa — 24/24 CTest passando.
Diagnósticos confirmados:
- Poisson 1D MMS: convergência O(h²) verificada (L∞(φ)=2.008e-04 para nx=128)
- Langmuir 1D: ω_obs=1.005 ∈ [0.8, 1.2]·ω_pe
- Marco 4 fechado

Antes de iniciar a Fase F (ciclo PIDC completo, T-045), precisamos confirmar
o diagnóstico dos bloqueadores registrados e definir a sequência mínima viável.

---

## Perguntas a responder

### Pergunta 1 — R-017: EFGPoissonSolver tem cache LDLT?

Ler `include/pidc/efg/EFGPoissonSolver.hpp`.

- `assemble()` e `solve(b)` estão separados na interface?
- A fatoração LDLT é armazenada entre chamadas ou refeita a cada `solve()`?
- Se for refeita: propor `DEC-0031` separando `factorize()` de `solve_rhs(b)`.
  - `assemble()` → monta K (O(N² × n_gauss))
  - `factorize()` → fatoriza uma vez (O(N³) ou esparso)
  - `solve_rhs(b)` → reutiliza fatoração (O(N²) ou melhor)
- Se já está cacheada: confirmar e encerrar R-017.

### Pergunta 2 — R-015/R-016: É possível iniciar Fase F sem periodicidade?

R-015: `mls_evaluate` usa distância euclidiana (não mínima imagem).  
R-016: `NeighborSearchGrid::query_radius` não é periódico.

- Análise: Fase F pode começar com domínio **não-periódico** (Dirichlet em todas
  as fronteiras), evitando R-015/R-016 inicialmente?
- Impacto físico de usar Dirichlet: reflexão de ondas nas fronteiras, nenhuma
  partícula escapa, φ=0 nas bordas. Isso é válido para um teste de fumaça PIDC?
- Se sim: propor que T-045 use domínio quadrado [0,1]² com Dirichlet e poucas
  partículas (ex: 25 elétrons em 5×5 nuvem regular).
- Se não: propor ordem de implementação de `mls_evaluate_periodic` e
  `query_radius_periodic` antes de T-045.

### Pergunta 3 — DEC-0022: `mls_evaluate_periodic` é pré-requisito para T-045?

- A proposta DEC-0022 existe em `docs/ai/DECISIONS.md` mas não foi implementada.
- Verificar se o ciclo PIDC mínimo (algumas partículas, domínio Dirichlet,
  sem reflexão periódica) pode funcionar sem ela.
- Recomendação: aceitar ou rejeitar DEC-0022 como pré-requisito para T-045.

### Pergunta 4 — ChargeDeposition.hpp: o que falta para o ciclo PIDC mínimo?

Ler `include/pidc/pidc/ChargeDeposition.hpp` — esta implementação usa `mls_evaluate`
para depositar carga nos nós da nuvem (`deposit_charge`).

O ciclo PIDC completo por passo de tempo é:
```
1. Zerar cargas nodais: Q_i = 0
2. Para cada partícula p:
   a. ShapeFunctionData sd = mls_evaluate(x_p, cloud, r_supp)
   b. Q_i += q_p * sd.phi[i]    ← deposit_charge (existe)
3. Montar b_i = Q_i / ε₀       ← montagem do RHS
4. Resolver K u = b             ← EFGPoissonSolver.solve(b)
5. Para cada partícula p:
   a. E(x_p) = -Σ_i u_i * sd.grad_phi[i]   ← interpolação de campo (NÃO EXISTE)
   b. a_p = (q/m) * E(x_p)
   c. leapfrog_advance (existe em LeapFrog1D.hpp)
   d. apply_periodic ou Dirichlet reflection
```

Listar módulos ausentes:
- `PIDCFieldInterpolator` ou função livre para calcular E(x_p) via funções de forma
- Reaproveitamento de `ShapeFunctionData` entre deposição (passo 2) e interpolação (passo 5)
- Estrutura do laço temporal (um `PIDCLoop` ou app simples em `apps/`)
- Wrapper para montar `b` a partir do vetor `Q` (trivial, mas deve ter nome claro)

---

## Arquivos obrigatórios para ler

Antes de responder, leia:

1. `include/pidc/efg/EFGPoissonSolver.hpp`
2. `include/pidc/geometry/NeighborSearchGrid.hpp`
3. `include/pidc/mls/MLSShapeFunction.hpp`
4. `include/pidc/pidc/ChargeDeposition.hpp`
5. `docs/ai/DECISIONS.md` — seções DEC-0010, DEC-0022, DEC-0025, DEC-0026
6. `docs/ai/RISKS.md` — seções R-015, R-016, R-017
7. `TODO.md` — Fase F (seção 6)
8. `docs/ai/AI_BOARD.md` — T-045

---

## Entregáveis esperados

1. **Resposta documentada** às 4 perguntas acima.
2. **DEC-0031** proposta (ou confirmação de que R-017 já está resolvido).
3. **Recomendação binária** para T-045:
   - Caminho A: domínio Dirichlet mínimo sem periodicidade (mais rápido)
   - Caminho B: resolver R-015/R-016 primeiro (mais correto, mais demorado)
4. **Lista de tarefas concretas** para T-045 adicionadas em `docs/ai/AI_BOARD.md`.
5. **Log** em `ai_logs/YYYY-MM-DD_gemini_t-gemini-f-readiness.md`.

---

## Regras

- Não implementar código nesta tarefa — apenas auditoria e proposta.
- Toda proposta nova de DEC deve ser registrada em `docs/ai/DECISIONS.md`.
- Se encontrar contradição entre `DECISIONS.md` e código, registrar em `docs/ai/RISKS.md`.
