# Log de Tarefa: T-GEMINI-F-READINESS — Auditoria pré-Fase F

**Agente:** Gemini
**Data:** 2026-05-09

## 1. Tarefa

Auditar os riscos e decisões pendentes para a Fase F (PIDC completo), responder às perguntas da especificação da tarefa e propor um plano de ação concreto.

## 2. Arquivos alterados

- `docs/ai/DECISIONS.md`: Adicionada a `DEC-0031` para resolver R-017.
- `docs/ai/AI_BOARD.md`: Marcada T-GEMINI-F-READINESS como concluída e desmembrada a T-045 em subtarefas (T-045A, T-045B, T-045C).
- `docs/ai/AI_STATUS.md`: Adicionado o resumo da auditoria e do plano.
- `ai_logs/2026-05-09_gemini_t-gemini-f-readiness.md`: Este arquivo.

## 3. Resultados da Auditoria

1.  **R-017 (Cache LDLT):** Confirmado que `EFGPoissonSolver` não armazena a fatoração, sendo ineficiente para o ciclo PIDC. **Ação:** Proposta a `DEC-0031` para refatorar a interface, separando `assemble_and_factorize()` de `solve()`.

2.  **R-015/R-016 (Periodicidade):** A falta de periodicidade em `mls_evaluate` e `NeighborSearchGrid` é um bloqueador para simulações periódicas, mas não para um teste de integração inicial. **Recomendação:** Iniciar a Fase F com um domínio Dirichlet não-periódico para validar o ciclo PIDC de forma isolada.

3.  **DEC-0022 (Periodicidade MLS):** A implementação desta decisão não é um pré-requisito para o *início* da Fase F, com base na recomendação acima.

4.  **Módulos Faltantes:** Para o ciclo PIDC mínimo, faltam:
    - Uma função `interpolate_field_pidc` para calcular o campo nas posições das partículas.
    - Uma estratégia para reutilizar `ShapeFunctionData` entre a deposição e a interpolação.
    - A estrutura do laço temporal principal (`PIDCLoop` ou um app).

## 4. Plano de Ação Proposto

A tarefa T-045 foi desmembrada na seguinte sequência:

- **T-045A (Claude):** Implementar `DEC-0031`. Refatorar `EFGPoissonSolver` para cachear a fatoração e resolver o risco de desempenho R-017.
- **T-045B (Codex):** Implementar a função `interpolate_field_pidc` e um teste de unidade para ela, usando um campo manufaturado.
- **T-045C (Claude):** Implementar o aplicativo `apps/pidc_smoke_2d.cpp`, que orquestra o primeiro ciclo PIDC completo em um domínio Dirichlet, utilizando os componentes das tarefas anteriores e reutilizando `ShapeFunctionData`.

## 5. Conclusão

A auditoria está completa. Com este plano, a Fase F pode começar de forma incremental e validada, tratando os riscos conhecidos na ordem correta.