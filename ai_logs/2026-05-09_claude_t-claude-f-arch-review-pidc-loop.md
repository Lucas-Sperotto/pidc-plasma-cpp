# Log — T-CLAUDE-F-ARCH-REVIEW-PIDC-LOOP

**Agente:** Claude
**Data:** 2026-05-09
**Tarefa:** Revisão arquitetural do PIDCLoop, solver cacheado e riscos periódicos

---

## Arquivos lidos

- `include/pidc/pidc/PIDCLoop.hpp`
- `include/pidc/pidc/DiffuseCell.hpp`
- `include/pidc/pidc/PIDCFieldInterpolator.hpp`
- `include/pidc/pidc/ChargeDeposition.hpp`
- `include/pidc/efg/EFGPoissonSolver.hpp` (versão pós-T-045A/D)
- `tests/test_pidc_loop.cpp`
- `docs/ai/DECISIONS.md` (DEC-0032, DEC-0033)
- `docs/ai/RISKS.md` (R-001 a R-021)

---

## Arquivos alterados

- `docs/ai/RISKS.md` — adicionados R-022 a R-025
- `docs/ai/DECISIONS.md` — DEC-0032/0033 aceitas; DEC-0034 proposta
- `docs/ai/AI_BOARD.md` — T-CLAUDE concluída; T-047 proposta para Codex
- `docs/ai/AI_STATUS.md` — resumo atualizado

---

## Achados principais

### Riscos registrados

**R-022 — Leapfrog 2D sem inicialização de meio-passo (BLOQUEADOR FÍSICO)**

`advance_particle_leapfrog_2d` em `PIDCLoop.hpp` tem a fórmula correta do
leapfrog, mas não há função de inicialização que desloque as velocidades para
t = -dt/2 antes do primeiro passo. Sem isso, o primeiro passo é Euler-Cromer.
Para o smoke test (2 passos, campo pequeno) invisível; para Langmuir 2D ou
reprodução da tese — conservação de energia degradada.

Contraste: `initialize_leapfrog_velocity_1d` existe e é obrigatória na Fase E.

**R-023 — Avanço de partículas sem aplicação de contorno**

`advance_particle_leapfrog_2d` avança posição sem wrap. Para Dirichlet, o
`test_pidc_loop` detecta a saída mas não corrige. Para periódico futuro, precisaria
de `PeriodicBoundary2D::wrap`.

**R-024 — Cargas em nós de fronteira não zeradas**

`deposit_charge_from_cells` deposita em todos os nós, incluindo fronteira. Para
BC homogêneo, penalty domina numericamente. Para unidades normalizadas de plasma
ou BC não-homogêneo, pode haver erro físico.

**R-025 — Assinatura de 7 parâmetros em `pidc_advance_one_step`**

Qualquer expansão (domínio para wrap, flags de diagnóstico) vai ampliar a
assinatura. Analogia: `LangmuirConfig1D` resolveu o mesmo problema na Fase E.

### Decisões

**DEC-0032 aceita:** `b_i = Q_i / ε₀` — matematicamente correto pela forma fraca
de Galerkin com partículas como delta-functions. Implementado em `pidc_rhs_from_charge`.

**DEC-0033 aceita:** Domínio de influência circular como padrão, definido por
`MLSConfig::support_radius`. Helpers `CircularInfluenceDomain` /
`RectangularInfluenceDomain` são para testes especializados.

**DEC-0034 proposta:** Criar `initialize_pidc_velocity_2d` — análogo direto do
`initialize_leapfrog_velocity_1d`. Critério de aceite: teste de reversibilidade
temporal (dois passos forward + dois backward → posição inicial). Tarefa: T-047.

---

## Testes executados

Nenhum — tarefa de revisão pura. 29/29 CTest inalterado.

---

## Hipóteses assumidas

- O comportamento atual (Euler-Cromer no primeiro passo) é um defeito, não uma
  escolha deliberada de integrador — baseado na nomenclatura "leapfrog" e na
  analogia com a Fase E.
- A dominância da penalidade sobre as cargas físicas (R-024) é válida para as
  unidades do smoke test, mas pode não ser para casos físicos reais.

---

## Pendências

- T-047: implementar `initialize_pidc_velocity_2d` + teste de reversibilidade
  (Codex)
- T-046: definir caso comum PIC-FD vs PIDC (Gemini + Claude)
- R-015/R-016 continuam bloqueando PIDC periódico

---

## Próximo passo sugerido

Codex: implementar T-047 (DEC-0034) — adicionar `initialize_pidc_velocity_2d`
e teste de reversibilidade. Isso remove o bloqueador físico mais urgente antes
de avançar para Marco 6 (comparação PIC vs PIDC).
