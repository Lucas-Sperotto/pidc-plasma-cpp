# Log de Tarefa: T-036 — Revisão arquitetural e fechamento Marco 3

**Agente:** Claude
**Data:** 2026-05-09

## 1. Tarefa

Finalizar T-036: realizar revisão final de arquitetura do `EFGPoissonSolver` (T-032/T-035),
confirmar critérios da Fase D, fechar formalmente o Marco 3 do ROADMAP.md e registrar
riscos para fases futuras.

## 2. Arquivos alterados

- `docs/ai/RISKS.md`: R-017 registrado; R-006 atualizado.
- `ROADMAP.md`: Marco 3 marcado como concluído (CSV/plotagem diferidos).
- `docs/ai/AI_BOARD.md`: T-036 movida para concluídas; T-037 em andamento; T-038 proposta.
- `docs/ai/AI_STATUS.md`: entrada T-036 Claude adicionada.
- `docs/ai/CURRENT_CONTEXT.md`: próximos passos atualizados.
- `ai_logs/2026-05-09_claude_architecture-review-T036.md`: este arquivo.

## 3. Comandos executados

```bash
cmake --build build -j
ctest --test-dir build --output-on-failure
```

Resultado: **14/14 testes passando** (0.03 s).

## 4. Revisão arquitetural — EFGPoissonSolver

### 4.1 Consistência com decisões

- DEC-0024 (penalidade Dirichlet): implementada corretamente em `impose_dirichlet_penalty`.
  Apenas a diagonal de K é modificada; simetria preservada (verificada no teste com tol 1e-10).
- DEC-0025 (backend esparso): `Eigen::SparseMatrix<double>` + `SimplicialLDLT`. Correto.
  A separação de `assemble()` / `solve()` existente desde T-034 é compatível.

### 4.2 Problemas identificados

#### (a) `solve()` recria LDLT a cada chamada — R-017

`solve()` instancia e chama `.compute(stiffness_)` localmente. Para Phase D (MMS estático),
custo único: aceitável. Para Phase F (ciclo PIDC com K constante), re-fatorar a cada passo
de tempo desperdiça O(n^1.5) sem ganho. **Bloqueante para Phase F.**

Mitigação proposta: adicionar `factorize()` ou cache lazy antes de Phase F.

#### (b) `stiffness_matrix()` retorna cópia densa O(n²)

Método público que materializa a matriz esparsa como `Eigen::MatrixXd`. Uso legítimo:
testes de simetria. Risco: se chamado no ciclo temporal do PIDC, será gargalo.
Não requer mudança imediata — só documenta risco (incluído em R-017).

#### (c) Dependência de estado implícita em `impose_dirichlet_penalty`

A função lê `stiffness_.coeff(i,i)` para calcular `max_diag`, depois acrescenta
triplets em `stiffness_entries` (passado por referência) e chama `rebuild_stiffness`
novamente. O primeiro `rebuild_stiffness` em `assemble()` é necessário para popular
`stiffness_` antes da penalidade. O acoplamento é funcional mas não-óbvio — documentado
no comentário DEC-0024 existente. Não requer mudança para Phase D.

### 4.3 Critérios da Fase D (TODO.md)

| Critério | Estado |
| --- | --- |
| Células de integração definidas | concluído (GaussCell2D) |
| Quadratura Gauss 2D | concluído (gauss2x2_cells) |
| Montagem de K | concluído (esparso, DEC-0025) |
| Montagem de b | concluído |
| Dirichlet por penalidade | concluído (DEC-0024) |
| Backend denso inicial | concluído (T-Poisson) |
| Migração para esparso | concluído (T-035) |
| Erro L2 potencial | concluído: 0.00359684 → 0.000827504 |
| Erro L∞ potencial | concluído: 0.0069852 → 0.00169752 |
| Erro L2 campo | concluído: 0.033052 → 0.0136451 |
| Erro L∞ campo | concluído: 0.105394 → 0.0554929 |
| Convergência ao refinar | concluído (9×9 < 5×5 em todas as métricas) |

Todos os critérios da Fase D estão atendidos.

### 4.4 Entregas do Marco 3 (ROADMAP.md)

| Entrega | Estado |
| --- | --- |
| Montagem de K | concluído |
| Montagem de b | concluído |
| Imposição de Dirichlet | concluído |
| Métricas MMS L2/L∞ potencial e campo | concluído |
| Exportação CSV | diferido — não bloqueia Marco 4 |
| Script de plotagem | diferido — não bloqueia Marco 4 |
| Relatório de validação formal | diferido — métricas registradas em AI_STATUS.md |

**Conclusão: Marco 3 concluído para o núcleo matemático/computacional.**

## 5. Hipóteses assumidas

- A ausência de CSV/plotagem não é bloqueante para avançar para Fase E (T-037/T-038).
- Exportação CSV pode ser adicionada como tarefa separada sem impacto no solver.

## 6. Pendências registradas

- R-017: cache LDLT — bloqueante para Phase F.
- R-015, R-016, DEC-0022: periodicidade MLS/busca — bloqueante para Phase F.

## 7. Próximo passo sugerido

**T-037** (Gemini + Claude): definir contrato e teste mínimo de deposição conservativa
de carga antes de implementar qualquer módulo PIDC. O teste deve verificar:
- `sum_i phi_i(xp) * q_p == q_p` (conservação por partícula);
- `sum_p sum_i phi_i(xp) * q_p == sum_p q_p` (conservação global).

Isso é pré-requisito para Fase E (PIC baseline, T-038) e Fase F (PIDC, T-037).
