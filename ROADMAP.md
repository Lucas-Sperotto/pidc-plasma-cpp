# ROADMAP — Desenvolvimento incremental do PIDC

## Marco 1 — Repositório respirando

Resultado esperado: projeto compila, roda um executável mínimo e possui testes vazios/estruturais.

Entrega:

- `CMakeLists.txt`
- `scripts/build.sh`
- `apps/smoke.cpp`
- `tests/`
- documentação inicial

---

## Marco 2 — MLS/EFG verificado

Resultado esperado: funções de forma MLS/EFG passam em testes matemáticos básicos.

Entrega:

- `MLSShapeFunction`
- função peso compacta
- busca de vizinhança
- testes de partição da unidade
- testes de reprodução linear

---

## Marco 3 — Poisson por EFG ✓ CONCLUÍDO (2026-05-09)

Resultado esperado: solver resolve problema manufaturado com erro medido.

Entrega:

- [x] montagem de `K` (esparso, `SimplicialLDLT`, DEC-0025)
- [x] montagem de `b`
- [x] imposição de Dirichlet (penalidade, DEC-0024)
- [x] métricas MMS: L2/L∞ do potencial e do campo com convergência verificada
- [ ] exportação CSV — diferido (não bloqueia Marco 4)
- [ ] script de plotagem — diferido (não bloqueia Marco 4)
- [ ] relatório de validação formal — diferido (métricas registradas em AI_STATUS.md)

---

## Marco 4 — PIC baseline ✓ CONCLUÍDO (2026-05-09)

Resultado esperado: PIC regular simples funciona e serve como referência.

Entrega:

- [x] deposição em grade (`deposit_charge_cic_1d`, T-039)
- [x] campo manufaturado 1D (`SineManufacturedField1D`, T-040)
- [x] solver de Poisson FD/DFT (`solve_poisson_periodic_1d`, T-041)
- [x] interpolação de campo CIC (`interpolate_field_cic_1d`, T-042)
- [x] leap-frog (`leapfrog_advance_1d`, T-043)
- [x] oscilação de Langmuir (`run_langmuir_1d`, T-044; ω_obs=1.005 ∈ [0.8,1.2]·ω_pe)
- [x] app diagnóstico com convergência O(h²) e saída CSV (T-REVIEW-E)

---

## Marco 5 — PIDC pequeno

Resultado esperado: PIDC roda com poucas partículas, conserva carga e exporta dados.

Entrega:

- célula difusa
- deposição PIDC
- interpolação PIDC
- ciclo temporal
- relatório de conservação

---

## Marco 6 — Comparação PIC versus PIDC

Resultado esperado: resultados comparáveis em caso controlado.

Entrega:

- mesmo caso rodando em PIC e PIDC
- métricas de erro
- energia, carga e frequência dominante
- gráficos comparativos

---

## Marco 7 — Reprodução da tese

Resultado esperado: reproduzir qualitativamente a simulação descrita na tese, com saídas reprodutíveis.

Entrega:

- caso pequeno
- caso médio
- caso grande
- figuras de potencial
- figuras vetoriais do campo
- relatório de limitações
