# Log de Tarefa: T-040C — Sincronização de numeração da Fase E

**Agente:** Claude
**Data:** 2026-05-09

## 1. Tarefa

Corrigir a divergência documental de numeração na Fase E. A sequência canônica
definida pelo professor é:

| ID     | Módulo                      |
|--------|-----------------------------|
| T-038A | Grid1D periódico            |
| T-039  | Deposição CIC 1D            |
| T-040  | Campo elétrico manufaturado |
| T-041  | Poisson 1D por FD/DFT       |
| T-042  | Interpolação campo→partícula|
| T-043  | Leap-frog isolado           |
| T-044  | Oscilação de Langmuir 1D    |

## 2. Diagnóstico antes das correções

| Arquivo | Estado |
|---------|--------|
| `PHASE_E_PIC1D_TECH_PLAN.md` | Correto (criado por Codex com numeração canônica) |
| `AI_BOARD.md` | Correto (já apontava T-041=Poisson, T-044=Langmuir) |
| `CURRENT_CONTEXT.md` | Correto (linha 99 com sequência canônica) |
| `AI_STATUS.md` | Correto (Gemini já atualizou com T-041=Poisson) |
| `TODO.md` | Sem referências numéricas conflitantes |
| `ROADMAP.md` | Sem referências numéricas à Fase E |
| `PHASE_E_PIC1D_PLAN.md` | **Divergente** — T-040=Poisson, T-041=Interp, T-042=LeapFrog, T-043=Langmuir |
| `RISKS.md` (R-021) | **Divergente** — sequência sem T-040 (campo) e sem T-044 (Langmuir) |

## 3. Arquivos alterados

- `docs/ai/PHASE_E_PIC1D_PLAN.md`: reescrito com numeração canônica. T-040 (campo
  manufaturado) adicionado como concluído; T-041=Poisson, T-042=Interp, T-043=LeapFrog,
  T-044=Langmuir. Nota de referência ao TECH_PLAN adicionada.
- `docs/ai/RISKS.md` (R-021): sequência atualizada de "T-039→...→T-043(Langmuir)"
  para sequência completa de 6 etapas terminando em T-044.
- `ai_logs/2026-05-09_claude_t040c_sync_phase_e_numbering.md`: este arquivo.

## 4. Arquivos **não alterados** (corretos ou fora de escopo)

- Headers C++ em `include/pidc/pic/` — nenhum alterado.
- Testes C++ em `tests/` — nenhum alterado.
- `CMakeLists.txt` — nenhum alterado.
- `PHASE_E_PIC1D_TECH_PLAN.md` — já estava correto.
- `AI_BOARD.md`, `AI_STATUS.md`, `CURRENT_CONTEXT.md` — já estavam corretos.
- `TODO.md`, `ROADMAP.md` — sem referências numéricas conflitantes.

## 5. Critério de aceite verificado

`docs/ai/PHASE_E_PIC1D_PLAN.md` aponta T-040 como campo manufaturado e T-041 como
Poisson 1D. `RISKS.md` R-021 inclui T-044 (Langmuir) na sequência de mitigação.

## 6. Próximo passo sugerido

**T-041 (Codex):** implementar `solve_poisson_periodic_1d` em
`include/pidc/pic/PoissonSolver1D.hpp`. Especificação técnica completa em
`docs/ai/PHASE_E_PIC1D_TECH_PLAN.md` seção T-041. CTest esperado: 19/19.
