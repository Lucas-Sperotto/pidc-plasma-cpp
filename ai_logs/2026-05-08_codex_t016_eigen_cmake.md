# 2026-05-08 — Codex — T-016 Eigen no CMake

## Objetivo

Implementar a `T-016`: adicionar a dependência Eigen ao CMake como pré-requisito para MLS/EFG e Poisson, com teste simples de consumo da biblioteca. Não implementar MLS, Poisson, deposição PIDC, interpolação ou caso grande da tese.

## Arquivos alterados

- `CMakeLists.txt`
- `tests/test_eigen_dependency.cpp`
- `TODO.md`
- `docs/ai/AI_BOARD.md`
- `docs/ai/AI_STATUS.md`
- `docs/ai/DECISIONS.md`
- `ai_logs/2026-05-08_codex_t016_eigen_cmake.md`

## Comandos executados

- `sed -n '1,280p' AGENTS.md`
- `sed -n '1,320p' TODO.md`
- `sed -n '1,260p' ROADMAP.md`
- `sed -n '1,240p' docs/ai/CURRENT_CONTEXT.md`
- `sed -n '1,280p' docs/ai/AI_BOARD.md`
- `sed -n '1,300p' docs/ai/AI_STATUS.md`
- `sed -n '1,520p' docs/ai/DECISIONS.md`
- `git status --short && git log --oneline -12`
- `sed -n '1,240p' README.md`
- `sed -n '1,220p' docs/00_visao_geral.md`
- `sed -n '1,280p' docs/01_mapa_tese_para_codigo.md`
- `git diff --stat && git diff --name-only`
- `cmp -s 2026-05-08_gemini_audit-equations-T002.md docs/ai/2026-05-08_gemini_audit-equations-T002.md`
- `cmp -s Species.hpp include/pidc/Species.hpp`
- `cmp -s test_species.cpp tests/test_species.cpp`
- `git diff --check`
- `git add ... && git commit -m "docs(ai): sincroniza tarefas Gemini"`
- `test -d /usr/include/eigen3 && echo /usr/include/eigen3 || true`
- `find /usr -path '*Eigen3Config.cmake' -o -path '*eigen3-config.cmake'`
- `sed -n '1,260p' CMakeLists.txt`
- `find include tests apps src -maxdepth 5 -type f | sort`
- `cmake -S . -B build`
- `cmake --build build -j`
- `./build/pidc_smoke`
- `./build/pidc_test_eigen_dependency`
- `/usr/bin/ctest --test-dir build --output-on-failure`
- `git diff --check`

## Testes executados

- `cmake -S . -B build`: passou.
- `cmake --build build -j`: passou.
- `./build/pidc_smoke`: passou.
- `./build/pidc_test_eigen_dependency`: passou.
- `/usr/bin/ctest --test-dir build --output-on-failure`: passou, 7/7 testes.
- `git diff --check`: passou.

## Resultados

- `CMakeLists.txt` agora usa `find_package(Eigen3 3.3 REQUIRED NO_MODULE)`.
- `pidc_core` propaga `Eigen3::Eigen` como dependência pública.
- Criado o teste `eigen_dependency`, que inclui `<Eigen/Dense>`, resolve um sistema 2x2 e também inclui um header `pidc`.
- `TODO.md` marca a dependência Eigen como concluída.
- Criada a `DEC-0015`.

## Hipóteses assumidas

- O ambiente de desenvolvimento possui Eigen disponível via `/usr/share/eigen3/cmake/Eigen3Config.cmake`.
- A dependência deve ser pública em `pidc_core`, pois futuros headers MLS/EFG podem expor tipos ou includes que dependem de Eigen.
- Foi usado `/usr/bin/ctest` por causa do wrapper local `ctest` quebrado já observado em tarefas anteriores.

## Pendências

- Definir framework de testes (`T-005`) ou manter temporariamente testes executáveis simples.
- Implementar domínios de influência e busca de vizinhança antes do cálculo MLS completo.
- Validar MLS antes de Poisson, deposição PIDC, interpolação de campo e qualquer caso grande da tese.

## Próximo passo sugerido

Implementar uma tarefa pequena de MLS que use Eigen sem ainda calcular funções de forma completas, por exemplo uma matriz de momento local mínima com falha controlada para `n < m`.
