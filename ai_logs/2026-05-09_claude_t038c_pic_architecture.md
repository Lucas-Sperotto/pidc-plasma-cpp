# Log de Tarefa: T-038C — Revisão arquitetural PIC 1D

**Agente:** Claude
**Data:** 2026-05-09

## 1. Tarefa

Revisar a arquitetura da Fase E (PIC 1D) com base no trabalho de T-038A (Codex)
e T-038B (Gemini). Estabelecer fronteiras claras entre os módulos antes de
implementar deposição, Poisson 1D, interpolação e leap-frog.

## 2. Arquivos alterados

- `docs/ai/DECISIONS.md`: DEC-0027 aceita; DEC-0028 proposta e aceita.
- `docs/ai/RISKS.md`: R-018, R-019, R-020, R-021 registrados.
- `docs/ai/AI_BOARD.md`: T-038C concluída; T-039 e T-040 na fila.
- `docs/ai/AI_STATUS.md`: entrada T-038C no histórico; "último resumo" atualizado.
- `docs/ai/CURRENT_CONTEXT.md`: estado PIC 1D expandido; sequência de tarefas definida.
- `ai_logs/2026-05-09_claude_t038c_pic_architecture.md`: este arquivo.

## 3. Comandos executados

Nenhum. Tarefa de revisão arquitetural — sem código alterado.

16/16 testes continuam passando (verificado por inspeção do estado atual).

## 4. Revisão arquitetural — 5 questões

### 4.1 Onde deve ficar `Grid1D`?

`include/pidc/pic/Grid1D.hpp` com namespace `pidc::pic` — **correto.**
O módulo PIC é separado de PIDC, EFG e MLS. O diretório é o lugar natural.

### 4.2 PIC 1D deve usar `Particle`/`Species` existentes?

**Sim.** Criar `pic::Particle1D` seria duplicação prematura de conceito num
módulo de referência. `Particle.position.x` e `velocity.x` são suficientes.
O componente `.y` não é usado no PIC 1D — R-020 documenta esse risco.

### 4.3 PIC 1D deve ser independente de EFG/MLS?

**Sim, sem exceções.** Deposição CIC usa interpolação bilinear direta — não usa
`mls_evaluate`. `PoissonSolver1D` usa FFT/FD — não usa `EFGPoissonSolver`.
Nenhum header de `mls/` ou `efg/` deve aparecer em `include/pidc/pic/`.
R-018 documenta o risco de acoplamento. DEC-0028 formaliza que vetores nodais
PIC 1D são `std::vector<double>`, sem Eigen.

### 4.4 Solver Poisson FD 1D deve ser separado do `EFGPoissonSolver`?

**Sim.** São algoritmos e dimensionalidades completamente diferentes:
- `EFGPoissonSolver`: 2D, esparso, Galerkin, `SimplicialLDLT`, `Eigen::VectorXd`
- `PoissonSolver1D` (futuro): 1D, periódico, FFT ou Thomas, `std::vector<double>`

Criar `include/pidc/pic/PoissonSolver1D.hpp` separado (T-040).

### 4.5 Comparação PIC vs PIDC deve ser adiada?

**Sim.** A comparação exige ambos os caminhos validados individualmente.
Tentativa prematura mascara qual lado está errado. Adiada para depois de:
T-043 (Langmuir PIC 1D) + Fase F (PIDC completo).

## 5. Decisões registradas

| DEC | Conteúdo |
| --- | --- |
| DEC-0027 | Convenção grade periódica 1D: `nx` nós = `nx` células, `dx = L/nx` — aceita |
| DEC-0028 | Vetores nodais PIC 1D como `std::vector<double>`, sem Eigen — aceita |

## 6. Riscos registrados

| Risco | Descrição resumida |
| --- | --- |
| R-018 | Acoplamento prematuro pic/ com pidc/ ou efg/ |
| R-019 | Ambiguidade nx nós vs nx células (FEM vs PIC) |
| R-020 | Particle 2D usada no PIC 1D: componente y ignorado |
| R-021 | Tentativa de Langmuir antes de deposição + campo + leap-frog validados |

## 7. Sequência de implementação definida

```text
T-039  deposit_charge_cic_1d   → std::vector<double>, teste conservação CIC
T-040  PoissonSolver1D          → FFT ou FD periódico, teste MMS 1D
T-041  field_interpolate_1d     → E = -du/dx, teste campo manufaturado
T-042  LeapFrog1D               → avanço temporal, teste reversibilidade
T-043  Langmuir 1D              → frequência plasma, critério de aceite da Fase E
```

Cada tarefa deve ter seu próprio teste antes de avançar (R-021).

## 8. Próximo passo sugerido

**T-039** (Codex): implementar `deposit_charge_cic_1d` em
`include/pidc/pic/ChargeDeposition1D.hpp`:

```cpp
// Retorno: std::vector<double> de tamanho grid.size()
// Usa Particle.position.x; reutiliza Species.charge
// Não inclui Eigen, mls/ nem efg/
std::vector<double> deposit_charge_cic_1d(
    const Grid1D& grid,
    const std::vector<Particle>& particles,
    const std::vector<Species>& species_list);
```

Testes obrigatórios (conforme T-038B / Gemini):
1. Partícula no centro da célula → 50/50 nos dois nós vizinhos.
2. Partícula sobre um nó → 100% naquele nó.
3. Partícula perto de `xmax` → nós `nx-1` e `0` (periodicidade).
4. Conservação global: `sum(Q_i) == sum(q_p)`.
