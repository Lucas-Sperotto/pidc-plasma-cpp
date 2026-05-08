# CURRENT_CONTEXT — Contexto mínimo para retomada

Atualizado em: 2026-05-08

Estamos planejando o repositório `pidc-plasma-cpp`, uma reconstrução moderna em C/C++ do método Particle-In-Diffuse-Cell (PIDC).

## Objetivo

Implementar e validar o PIDC, começando por blocos pequenos:

1. MLS/EFG;
2. Poisson;
3. PIC baseline;
4. deposição PIDC;
5. interpolação PIDC;
6. leap-frog;
7. reprodução qualitativa da tese.

## Fonte científica

A tese propõe uma formulação meshfree para simulação de plasmas baseada em PIC e MEFG. A partir do conceito de elemento difuso, a tese formula a célula difusa e o método Particle-In-Diffuse-Cell.

## Direção estratégica

Não começar pelo caso de 360.000 partículas.

Começar por:

- estruturas básicas;
- testes de partição da unidade;
- reprodução linear;
- conservação de carga;
- Poisson MMS;
- caso pequeno.

## Estado atual

O repositório ainda está em fase de planejamento. Os arquivos deste pacote são um ponto de partida para criar a estrutura e orientar Gemini, Codex e Claude.

## Próximo passo recomendado

Entregar ao Codex a tarefa de bootstrap mínimo do repositório, conforme `docs/08_prompt_inicial_para_cada_ia.md`.
