# Log de Tarefa: T-037 — Definir teste de deposição conservativa de carga

**Agente:** Gemini
**Data:** 2026-05-09

## 1. Tarefa

Realizar a minha parte da tarefa T-037: definir o teste de validação para a deposição de carga conservativa do PIDC e propor uma arquitetura inicial.

## 2. Arquivos alterados

- `docs/ai/DECISIONS.md`: Adicionada a `DEC-0026` propondo a interface para `deposit_charge`.
- `docs/ai/AI_BOARD.md`: Atualizada a tarefa T-037 com o progresso e passada para Claude.
- `docs/ai/AI_STATUS.md`: Adicionado o resumo da atividade.
- `ai_logs/2026-05-09_gemini_define-charge-conservation-test-T037.md`: Este arquivo.

## 3. Análise e Decisão

1.  **Fundamento**: A conservação de carga `Σ Q_i = Σ q_p` é uma consequência direta da propriedade de partição da unidade do MLS (`Σ φᵢ = 1`), que já foi validada.
2.  **Validação**: O teste deve ser numérico. Um conjunto determinístico de partículas com cargas conhecidas é criado. A função de deposição é chamada. A soma das cargas nodais resultantes deve ser igual à soma das cargas das partículas originais, a menos de erros de ponto flutuante.
3.  **Arquitetura**: Proposta a `DEC-0026` para criar uma função livre `deposit_charge` em `include/pidc/pidc/ChargeDeposition.hpp`. Esta função encapsula o loop sobre as partículas e a chamada a `mls_evaluate`.

**Conclusão:** O plano de validação e a interface inicial para a deposição de carga estão definidos. O projeto está pronto para a implementação desta funcionalidade.

## 4. Próximo passo sugerido (Instruções para Claude)

O agente **Claude** deve continuar a tarefa `T-037`:
1.  Revise a `DEC-0026`. Se concordar, mude seu status para `aceita`.
2.  Implemente a função `deposit_charge` conforme a decisão. Crie o novo arquivo `include/pidc/pidc/ChargeDeposition.hpp`.
3.  Implemente o teste `test_charge_conservation.cpp` em `tests/`.
    - Use uma nuvem 5x5, `MLSConfig` com `support_radius = 1.8 * (1.0/4.0)`.
    - Crie 2 espécies (e.g., `q=1`, `q=-2`).
    - Crie 3-4 partículas em posições não-simétricas (e.g., `{0.1, 0.2}`, `{0.5, 0.5}`, `{0.8, 0.9}`).
    - Verifique se `Σ Q_i == Σ q_p` com `approx_equal` e tolerância apertada (`1e-14`).
4.  Adicione o novo teste ao `CMakeLists.txt` e verifique se todos os 15 testes passam.
5.  Atualize o `TODO.md` e o `AI_STATUS.md` para refletir a conclusão da tarefa.
6.  Mova a T-037 para "Concluídas" no `AI_BOARD.md`.
7.  Crie seu próprio log de conclusão.