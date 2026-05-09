# Log — 2026-05-09 — Gemini — T-046 (Parte 1)

## Objetivo

Definir um caso de teste comum para a comparação entre o PIC-FD de referência e o PIDC, resolvendo a incompatibilidade de dimensionalidade (1D vs 2D) e condições de contorno (periódico vs Dirichlet) das implementações atuais.

## Contexto lido

- [x] AGENTS.md
- [x] TODO.md
- [x] ROADMAP.md
- [x] docs/ai/CURRENT_CONTEXT.md
- [x] docs/ai/AI_BOARD.md
- [x] docs/ai/AI_STATUS.md
- [x] docs/ai/DECISIONS.md
- [x] docs/ai/RISKS.md

## Arquivos alterados

- `docs/ai/AI_BOARD.md`: Movi a tarefa T-046 para "concluída" (parte Gemini) e criei a tarefa de revisão para Claude.
- `docs/ai/AI_STATUS.md`: Adicionei o resumo da proposta.
- `docs/ai/DECISIONS.md`: Adicionei a proposta `DEC-0035`.
- `TODO.md`: Atualizei o status da tarefa de comparação.

## Arquivos criados

- `docs/validation/comparison_plan_pic_pidc.md`: Documento que detalha o plano de comparação.
- `ai_logs/2026-05-09_gemini_t046_define_comparison_case.md`: Este log.

## Análise do Problema

A comparação direta é impossível. A solução mais robusta e alinhada com os objetivos de longo prazo (Marco 7) é definir um caso comum em 2D com condições de contorno periódicas.

## Decisões propostas

- **DEC-0035:** Formaliza a escolha da **instabilidade de duas correntes 2D periódica** como o caso de teste para a comparação.

## Plano Proposto

O plano, detalhado em `docs/validation/comparison_plan_pic_pidc.md`, estabelece que a comparação requer o desenvolvimento de dois grandes blocos:
1.  **PIDC 2D Periódico:** Exige a implementação de MLS e busca de vizinhos periódicos (resolvendo R-015, R-016) e um solver de Poisson EFG 2D periódico.
2.  **PIC-FD 2D Periódico:** Exige a criação de um novo código de referência 2D (grade, deposição, solver FFT, interpolação).

## Problemas encontrados

Nenhum problema de implementação, mas a tarefa de definição revelou que o "Marco 6 - Comparação PIC versus PIDC" tem pré-requisitos de desenvolvimento significativos que não estavam explicitamente sequenciados.

## Próximo passo sugerido

Passar a tarefa para **Claude** para que ele revise o plano de comparação (`docs/validation/comparison_plan_pic_pidc.md`), analise os riscos e refine as métricas e os parâmetros do caso de teste.