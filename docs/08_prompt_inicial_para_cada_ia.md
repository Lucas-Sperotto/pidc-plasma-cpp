# 08 — Prompts iniciais para Gemini, Codex e Claude

## Prompt para o Codex

```text
Você está trabalhando no repositório pidc-plasma-cpp.

Leia obrigatoriamente:
- AGENTS.md
- CODEX.md
- TODO.md
- ROADMAP.md
- docs/ai/CURRENT_CONTEXT.md
- docs/ai/AI_BOARD.md
- docs/ai/DECISIONS.md

Sua tarefa inicial é implementar apenas o bootstrap mínimo do projeto:
1. CMakeLists.txt com C++17.
2. Diretórios include/pidc, src, apps e tests.
3. Estruturas Vec2, Particle, Node e Domain2D.
4. Um executável apps/smoke.cpp.
5. Um teste simples de contorno periódico, se o framework de testes já estiver definido; caso contrário, crie um executável de teste simples.

Não implemente MLS, Poisson nem PIDC completo ainda.

Ao final:
- rode os comandos de build;
- registre os comandos executados;
- atualize docs/ai/AI_STATUS.md;
- crie um log em ai_logs/.
```

## Prompt para o Gemini

```text
Você está trabalhando no repositório pidc-plasma-cpp.

Leia obrigatoriamente:
- AGENTS.md
- GEMINI.md
- README.md
- TODO.md
- docs/00_visao_geral.md
- docs/01_mapa_tese_para_codigo.md
- docs/03_modelo_matematico_minimo.md
- docs/04_validacoes_referencia.md

Sua tarefa inicial é fazer uma auditoria matemática/documental:
1. Verifique se o mapa tese → código está coerente.
2. Verifique se as equações mínimas estão com sinais e hipóteses explícitos.
3. Liste ambiguidades que precisam ser decididas antes de implementar Poisson e PIDC.
4. Crie ou atualize docs/ai/OPEN_QUESTIONS.md.
5. Proponha decisões em docs/ai/DECISIONS.md, sem apagar decisões existentes.

Não implemente código nesta tarefa.
```

## Prompt para o Claude

```text
Você está trabalhando no repositório pidc-plasma-cpp.

Leia obrigatoriamente:
- AGENTS.md
- CLAUDE.md
- TODO.md
- ROADMAP.md
- docs/02_arquitetura_repositorio.md
- docs/05_fluxo_de_trabalho_ias.md
- docs/07_riscos_tecnicos.md

Sua tarefa inicial é revisar criticamente a arquitetura:
1. Identifique riscos de acoplamento entre módulos.
2. Sugira interfaces mínimas para MLSShapeFunction, EFGPoissonSolver e PIDCLoop.
3. Aponte quais decisões devem ser congeladas antes de escrever muito código.
4. Registre riscos em docs/ai/RISKS.md.
5. Proponha decisões em docs/ai/DECISIONS.md.

Não reescreva o planejamento inteiro. Faça comentários objetivos e acionáveis.
```

## Prompt para retomada geral

```text
Estamos reconstruindo em C/C++ o método Particle-In-Diffuse-Cell (PIDC), baseado na tese de Gleber Nelson Marques.

Antes de responder, leia:
- AGENTS.md
- TODO.md
- ROADMAP.md
- docs/ai/CURRENT_CONTEXT.md
- docs/ai/AI_BOARD.md
- docs/ai/AI_STATUS.md
- docs/ai/DECISIONS.md

Trabalhe em uma tarefa pequena, com teste e registro. Não avance para o caso grande da tese antes das validações de MLS, Poisson, deposição de carga e interpolação de campo.
```
