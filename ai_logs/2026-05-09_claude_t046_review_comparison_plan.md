# Log — 2026-05-09 — Claude — T-046 (Revisão)

## Objetivo

Revisar o plano de comparação PIC-FD vs. PIDC proposto pelo Gemini em T-046
(`docs/validation/comparison_plan_pic_pidc.md`), analisar riscos dos pré-requisitos
e refinar a sequência de desenvolvimento para o Marco 6.

## Contexto lido

- [x] AGENTS.md (via CLAUDE.md)
- [x] docs/ai/AI_BOARD.md
- [x] docs/ai/AI_STATUS.md
- [x] docs/ai/DECISIONS.md (DEC-0035)
- [x] docs/validation/comparison_plan_pic_pidc.md
- [x] ai_logs/2026-05-09_gemini_t046_define_comparison_case.md

## Arquivos alterados

- `docs/validation/comparison_plan_pic_pidc.md`: Adicionada Seção 8 com a revisão
  completa de Claude — avaliação por aspecto, proposta de fases A+B para Marco 6,
  tarefas T-047/T-048/T-049 e tabela-resumo.
- `docs/ai/DECISIONS.md`: DEC-0035 atualizada de `proposta` para `parcialmente aceita
  (emendada)` com seção de emenda detalhando o redirecionamento para Marco 7 e a
  abordagem revisada para Marco 6.
- `docs/ai/AI_BOARD.md`: T-046 revisão Claude marcada como concluída; T-047/T-048/T-049
  adicionadas na seção de propostas.
- `docs/ai/AI_STATUS.md`: Resumo atualizado com resultado da revisão.

## Arquivos criados

- `ai_logs/2026-05-09_claude_t046_review_comparison_plan.md`: Este log.

## Análise realizada

### Problema identificado na proposta Gemini

A instabilidade de duas correntes 2D periódica (DEC-0035) requer cinco blocos
não-implementados:

1. R-015: `mls_evaluate` periódico (imagem mínima na busca de vizinhos)
2. R-016: `NeighborSearchGrid` periódico
3. Solver de Poisson EFG 2D periódico (nunca implementado)
4. PIC-FD 2D periódico completo (grade 2D, deposição CIC 2D, FFT 2D)
5. R-022 resolvido via T-047 (DEC-0034)

Isso representa semanas de desenvolvimento antes de qualquer comparação e viola
o princípio de "caso controlado" do Marco 6.

### Revisão proposta

Marco 6 em duas fases incrementais que **não requerem periodicidade**:

**Fase A — campo manufaturado:**
- Campo estático E(x,y) conhecido aplicado a PIC-2D e PIDC
- Remove Poisson da comparação — mede só o erro do leapfrog 2D
- Pré-condição: T-047 (DEC-0034 — `initialize_pidc_velocity_2d`)
- Tarefa nova: T-048

**Fase B — Langmuir 2D Dirichlet:**
- Perturbação senoidal em `[0,1]²` com BC u=0, campo self-consistent
- Novo módulo PIC-FD 2D Dirichlet: `Grid2D`, `deposit_charge_cic_2d`,
  `PoissonSolver2D_FD` (5-pontos, SimplicialLDLT), `interpolate_field_cic_2d`
- Usa solver EFG Dirichlet existente — sem R-015/R-016
- Tarefa nova: T-049

**Marco 7:** instabilidade de duas correntes (proposta original Gemini).

## Hipóteses assumidas

- Os pré-requisitos R-015 e R-016 continuam sem implementação.
- A abordagem de `PoissonSolver2D_FD` com 5 pontos e SimplicialLDLT é padrão
  e factível rapidamente (análoga ao solver 1D já validado).
- T-047 (DEC-0034) é pré-condição para qualquer fase de comparação com PIDC.

## Testes executados

Nenhum código alterado nesta revisão. 29/29 CTest inalterado.

## Pendências

- T-047: implementar `initialize_pidc_velocity_2d` (Codex)
- T-048: implementar `PIC2DManufactured` + comparação Fase A (Codex)
- T-049: módulo PIC-FD 2D Dirichlet + comparação Fase B (Codex)

## Próximo passo sugerido

Codex executa T-047 (pré-condição crítica, R-022, bloqueador físico identificado
na revisão da arquitetura PIDC Loop).
