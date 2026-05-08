# 2026-05-08 — Codex — bootstrap mínimo

## Objetivo

Implementar apenas o bootstrap mínimo do projeto: CMake C++17, diretórios-base, estruturas `Vec2`, `Particle`, `Node` e `Domain2D`, executável smoke e teste simples de contorno periódico.

## Arquivos alterados

- `CMakeLists.txt`
- `include/pidc/Vec2.hpp`
- `include/pidc/Particle.hpp`
- `include/pidc/Node.hpp`
- `include/pidc/Domain2D.hpp`
- `src/Domain2D.cpp`
- `apps/smoke.cpp`
- `tests/test_periodic_boundary.cpp`
- `TODO.md`
- `docs/ai/AI_BOARD.md`
- `docs/ai/AI_STATUS.md`
- `ai_logs/2026-05-08_codex_bootstrap_minimo.md`

## Comandos executados

- `pwd && rg --files -g 'AGENTS.md' -g 'CODEX.md' -g 'TODO.md' -g 'ROADMAP.md' -g 'docs/ai/CURRENT_CONTEXT.md' -g 'docs/ai/AI_BOARD.md' -g 'docs/ai/DECISIONS.md' -g 'docs/ai/AI_STATUS.md' -g 'README.md' -g 'docs/00_visao_geral.md' -g 'docs/01_mapa_tese_para_codigo.md'`
- `git status --short`
- Leituras com `sed -n` dos arquivos centrais exigidos por `AGENTS.md` e pelo usuário.
- `rg -n "Catch|GoogleTest|gtest|doctest|CTest|test framework|framework de testes|add_test|enable_testing" .`
- `find include src apps tests scripts -maxdepth 3 -type f -print | sort`
- `cmake -S . -B build`
- `cmake --build build -j`
- `ctest --test-dir build --output-on-failure`
- `type -a ctest`
- `/usr/bin/ctest --test-dir build --output-on-failure`
- `./build/pidc_smoke`
- `./build/pidc_test_periodic_boundary`
- `mkdir -p ai_logs`
- `git diff --stat`
- `git diff --check`
- `git status --short`

## Testes executados

- `cmake -S . -B build`: passou.
- `cmake --build build -j`: passou.
- `./build/pidc_smoke`: passou.
- `./build/pidc_test_periodic_boundary`: passou.
- `/usr/bin/ctest --test-dir build --output-on-failure`: passou, 1/1 teste.

Observação: `ctest --test-dir build --output-on-failure` usando o PATH atual falhou porque `/home/sperotto/.local/bin/ctest` é um wrapper Python sem o módulo `cmake`. O binário do sistema `/usr/bin/ctest` funcionou corretamente.

## Resultados

- Criada biblioteca `pidc_core`.
- Criado executável `pidc_smoke`.
- Criado teste executável `pidc_test_periodic_boundary` e registrado no CTest.
- Teste cobre retorno periódico para direita/esquerda, topo/base e ponto exatamente no limite superior.

## Hipóteses assumidas

- Como o framework de testes ainda não foi definido, foi criado um executável de teste simples integrado ao CTest, sem dependência externa.
- `Domain2D` usa domínio periódico semiaberto `[lower, upper)`.

## Pendências

- Definir framework de testes (`T-005`).
- Adicionar Eigen e opção futura para `nlohmann_json`.
- Criar scripts `scripts/build.sh` e `scripts/run_tests.sh`.
- Implementar `Species`, `PeriodicBoundary2D`, nuvem regular de nós e busca de vizinhança.
- Não foram implementados MLS, Poisson ou PIDC completo.

## Próximo passo sugerido

Definir o framework de testes ou criar os scripts mínimos de build/teste antes de avançar para busca de vizinhança e MLS.
