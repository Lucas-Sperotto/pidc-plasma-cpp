# Log de Tarefa: T-020 — Auditar matematicamente MLSShapeFunction e gradientes

**Agente:** Gemini
**Data:** 2026-05-08

## 1. Tarefa

Realizar a auditoria matemática da implementação de `mls_evaluate` em `MLSShapeFunction.hpp`, com foco na correção das fórmulas das funções de forma e seus gradientes, conforme a teoria EFG/MLS.

## 2. Arquivos alterados

- `docs/ai/RISKS.md`: Adicionado o risco `R-013` para documentar o bug encontrado e a fragilidade do teste.
- `docs/ai/AI_BOARD.md`: Movida a tarefa `T-020` para "Concluídas". Criada a tarefa `T-021` para corrigir o bug.
- `docs/ai/AI_STATUS.md`: Atualizado o status do projeto para refletir a descoberta do bug crítico.

## 3. Comandos executados

Nenhum. A tarefa foi uma auditoria de código e matemática, sem execução de comandos.

## 4. Resultados

**Bug Crítico Encontrado:** A implementação do gradiente da função de forma em `mls_evaluate` (realizada na tarefa T-017) está matematicamente incorreta.

- **Erro:** A fórmula implementada para `∇φᵢ` contém um termo extra, `(∇p)ᵀ c wᵢ`.
- **Fórmula Correta:** `∇φᵢ(x) = (∇c(x))ᵀ p(xᵢ) wᵢ(x) + c(x)ᵀ p(xᵢ) ∇wᵢ(x)`.
- **Causa da Falha na Detecção:** O teste de validação (`test_mls_shape_function`) utiliza uma nuvem de nós e um ponto de avaliação perfeitamente simétricos. Nesta configuração especial, o termo de erro se anula, fazendo com que o teste passe indevidamente.

Esta falha compromete a precisão de qualquer cálculo que utilize o gradiente, incluindo a montagem da matriz de rigidez `K` no solver de Poisson e a interpolação do campo elétrico nas partículas.

## 5. Hipóteses assumidas

- A teoria de referência é a formulação padrão de EFG, conforme Belytschko et al. (1994).
- A implementação de `c(x)` e `∇c(x)` está correta, mas sua utilização no cálculo de `∇φᵢ` está errada.

## 6. Pendências

O bug precisa ser corrigido antes de qualquer avanço na Fase D (Solver EFG de Poisson).

## 7. Próximo passo sugerido

O agente **Claude**, que implementou a T-017, deve executar a tarefa `T-021`:
1. Corrigir a fórmula do gradiente em `MLSShapeFunction.hpp`.
2. Adicionar um novo caso de teste em `test_mls_shape_function.cpp` com um ponto de avaliação assimétrico (e.g., `{0.6, 0.4}` na nuvem 5x5) para garantir que o bug seja corrigido e não retorne.