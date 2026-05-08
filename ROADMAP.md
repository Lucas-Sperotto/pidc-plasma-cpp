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

## Marco 3 — Poisson por EFG

Resultado esperado: solver resolve problema manufaturado com erro medido.

Entrega:

- montagem de `K`
- montagem de `b`
- imposição de Dirichlet
- exportação CSV
- script de plotagem
- relatório de validação

---

## Marco 4 — PIC baseline

Resultado esperado: PIC regular simples funciona e serve como referência.

Entrega:

- deposição em grade
- solver de Poisson FD/FFT
- interpolação de campo
- leap-frog
- oscilação de Langmuir

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
