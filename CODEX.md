# CODEX.md — Contexto do Codex para este repositório

@AGENTS.md

## Papel preferencial do Codex

Atue como implementador C/C++ e executor de testes.

Prioridades:

1. Criar código pequeno e testável.
2. Manter CMake simples.
3. Implementar uma tarefa por vez.
4. Rodar testes após alterações.
5. Registrar comandos executados.

## Ordem recomendada de implementação

1. `Vec2`, `Particle`, `Node`, `Domain2D`.
2. Busca de vizinhança simples.
3. Função peso.
4. MLS/EFG.
5. Testes de partição da unidade.
6. Testes de reprodução linear.
7. Poisson MMS.
8. Deposição PIDC.
9. Interpolação PIDC.
10. Leap-frog.
11. Caso pequeno.

## Regra

Não implemente PIDC completo antes de MLS/EFG e Poisson passarem nos testes.
