# Log — 2026-05-08 — Claude — Auditoria e fechamento das Fases 0/A/B/C

## Objetivo

Verificar o estado real das Fases 0, A, B e C; executar pendências que Claude
pode resolver diretamente; criar tarefas formais para outras IAs (Codex, Gemini,
Professor) para o que ainda está incompleto.

## Estado encontrado (antes desta sessão)

| Fase | Item pendente |
| --- | --- |
| 0 | "Núcleo matemático" — MLS feito, Poisson/PIDC ausentes |
| A | `scripts/build.sh` e `scripts/run_tests.sh` ausentes |
| B | `PeriodicBoundary2D` e `NeighborSearchGrid` ausentes |
| C | `docs/validation/VALIDATION_PLAN.md` ausente; DEC-0018 em "proposta" |

## Ações executadas

### 1. scripts/build.sh e scripts/run_tests.sh

Criados em `scripts/`. Ambos com `set -euo pipefail`. `build.sh` executa
`cmake -S . -B build && cmake --build build -j$(nproc)`. `run_tests.sh`
executa build incremental + `ctest --test-dir build --output-on-failure`.
TODO.md: marcados `[x]`.

### 2. DEC-0018 aceita

`docs/ai/DECISIONS.md`: status alterado de `proposta` para `aceita` com nota
de aceite. Desbloqueia T-Poisson para Codex.

### 3. DEC-0019 proposta

Adicionada entrada DEC-0019 em `docs/ai/DECISIONS.md` sobre semântica de
`Node::volume`. Propõe: campo informativo (`dx*dy`) ou remoção. Decisão
delegada para T-023 (Professor + Gemini).

### 4. docs/validation/VALIDATION_PLAN.md criado

Novo arquivo em `docs/validation/`. Define:
- Fase C: 9 propriedades (PU, LR, gradientes), tolerância $10^{-10}$,
  mínimo 4 pontos de consulta incluindo 3 assimétricos.
- Fase D: MMS com $u = \sin(\pi x)\sin(\pi y)$, erro $L^2 < 10^{-2}$.
- Fases E–H: referência a critérios futuros.

### 5. TODO.md atualizado

- `[x] scripts/build.sh`, `[x] scripts/run_tests.sh`.
- Condicionamento MLS: anotado como verificado indiretamente; T-024 proposta.

### 6. AI_BOARD.md atualizado

Adicionadas tarefas:

| ID | Tarefa | IA |
| --- | --- | --- |
| T-025 | NeighborSearchGrid | Codex |
| T-026 | PeriodicBoundary2D | Codex |
| T-028 | Revisar VALIDATION_PLAN.md | Gemini |
| T-Poisson | Assembler EFG (DEC-0018 aceita) | Codex |

## Arquivos alterados

| Arquivo | Operação |
| --- | --- |
| `scripts/build.sh` | criado |
| `scripts/run_tests.sh` | criado |
| `docs/ai/DECISIONS.md` | DEC-0018 aceita; DEC-0019 proposta |
| `docs/validation/VALIDATION_PLAN.md` | criado |
| `TODO.md` | checkboxes Fase A; nota Fase C |
| `docs/ai/AI_BOARD.md` | T-025, T-026, T-028 adicionadas |
| `docs/ai/AI_STATUS.md` | resumo atualizado |

## Comandos executados

```bash
chmod +x scripts/build.sh scripts/run_tests.sh
cmake -S . -B build
cmake --build build -j
ctest --test-dir build --output-on-failure
bash scripts/build.sh
bash scripts/run_tests.sh
```

## Resultado dos testes

9/9 testes passando. Nenhum arquivo C++ foi alterado.

## Estado após esta sessão

| Fase | Estado |
| --- | --- |
| 0 | Bootstrap, protocolo, testes: completos. Núcleo matemático: MLS completo; Poisson/PIDC pendentes. |
| A | **Completa** — CMake, Eigen, smoke, scripts de build/test. |
| B | Parcialmente completa — Vec2/Particle/Species/Node/Domain2D/NodeCloud/RegularNodeCloud2D presentes. PeriodicBoundary2D e NeighborSearchGrid ausentes (T-025, T-026). |
| C | **Completa para MLS** — mls_evaluate, gradientes, 4 pontos assimétricos, VALIDATION_PLAN. Condicionamento explícito pendente (T-024). |

## Próximos passos sugeridos (ordem recomendada)

1. **T-023** (Professor + Gemini): decidir DEC-0019 (Node::volume).
2. **T-025** (Codex): NeighborSearchGrid.
3. **T-026** (Codex): PeriodicBoundary2D.
4. **T-024** (Codex): teste NaN/Inf em MLS.
5. **T-028** (Gemini): revisar VALIDATION_PLAN.md.
6. **T-Poisson** (Codex): assembler EFG — pode iniciar após T-023 e DEC-0018 aceita.
