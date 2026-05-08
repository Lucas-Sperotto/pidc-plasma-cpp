# 2026-05-08 — Codex — T-009 NodeCloud

## Objetivo

Implementar a `T-009`/`DEC-0012`: criar `NodeCloud` como proprietário canônico da lista de nós, sem implementar busca de vizinhança, MLS, Poisson ou PIDC completo.

## Arquivos alterados

- `CMakeLists.txt`
- `include/pidc/geometry/NodeCloud.hpp`
- `tests/test_node_cloud.cpp`
- `docs/ai/AI_BOARD.md`
- `docs/ai/AI_STATUS.md`
- `docs/ai/DECISIONS.md`
- `ai_logs/2026-05-08_codex_t009_node_cloud.md`

## Comandos executados

- `sed -n '1,260p' AGENTS.md`
- `sed -n '1,300p' TODO.md`
- `sed -n '1,240p' ROADMAP.md`
- `sed -n '1,220p' docs/ai/CURRENT_CONTEXT.md`
- `sed -n '1,260p' docs/ai/AI_BOARD.md`
- `sed -n '1,260p' docs/ai/AI_STATUS.md`
- `sed -n '1,360p' docs/ai/DECISIONS.md`
- `git status --short`
- `sed -n '1,220p' README.md`
- `sed -n '1,220p' docs/00_visao_geral.md`
- `sed -n '1,260p' docs/01_mapa_tese_para_codigo.md`
- `find include src apps tests ai_logs -maxdepth 5 -type f | sort`
- `sed -n '1,240p' CMakeLists.txt`
- `sed -n '1,220p' include/pidc/Node.hpp`
- `sed -n '1,220p' include/pidc/ShapeFunctionData.hpp`
- `sed -n '1,220p' tests/test_shape_function_data.cpp`
- `cmake -S . -B build`
- `cmake --build build -j`
- `./build/pidc_smoke`
- `./build/pidc_test_node_cloud`
- `/usr/bin/ctest --test-dir build --output-on-failure`
- `git diff --check`
- `git diff --stat`
- `git status --short`

## Testes executados

- `cmake -S . -B build`: passou.
- `cmake --build build -j`: passou.
- `./build/pidc_smoke`: passou.
- `./build/pidc_test_node_cloud`: passou.
- `/usr/bin/ctest --test-dir build --output-on-failure`: passou, 3/3 testes.
- `git diff --check`: passou.

## Resultados

- Criado `pidc::NodeCloud` em `include/pidc/geometry/NodeCloud.hpp`.
- `NodeCloud` possui `std::vector<Node>`, expõe `size()`, `operator[]` const e `nodes()` const.
- Criado teste `node_cloud` cobrindo nuvem vazia, preservação de ordem/ids, posições e volumes.
- `DEC-0012` foi marcada como aceita e implementada.

## Hipóteses assumidas

- `NodeCloud` é header-only nesta fase porque não há lógica além da posse e acesso à lista de nós.
- `operator[]` não faz checagem de faixa, seguindo o contrato usual de `std::vector::operator[]`; testes usam índices válidos.
- Não há item direto no `TODO.md` para `NodeCloud`, então o arquivo não foi alterado.

## Pendências

- Implementar `Species` (`T-008`) ou decidir o framework de testes (`T-005`).
- Implementar `RegularNodeCloud2D` e `NeighborSearchGrid` em tarefas futuras.
- Validar MLS antes de qualquer avanço para Poisson, deposição PIDC, interpolação ou caso grande da tese.

## Próximo passo sugerido

Implementar `T-008` (`Species`) para retirar carga/massa de cada partícula, ou `T-005` para consolidar o framework de testes antes dos testes matemáticos de MLS.
