# pidc-plasma-cpp

Implementação C/C++ reprodutível do método **Particle-In-Diffuse-Cell (PIDC)** para simulação eletrostática de plasmas, com base na tese de Gleber Nelson Marques e na publicação do CMNE/CILAMCE 2007.

O objetivo inicial não é apenas reproduzir figuras qualitativas, mas construir uma base computacional verificável, com testes unitários, soluções manufaturadas, casos físicos de referência e comparação com métodos PIC clássicos.

## Ideia central

O PIDC combina:

- modelo **Particle-In-Cell (PIC)** eletrostático;
- método **Element-Free Galerkin (EFG/MEFG)**;
- aproximação **Moving Least Squares (MLS)**;
- conceito de **célula difusa**, definida pelas funções de forma e pelos domínios de influência nodais;
- particionamento de carga usando funções de forma que satisfazem partição de unidade.

Em vez de a partícula pertencer a uma célula geométrica rígida, ela pertence a uma célula funcional/difusa: o conjunto de nós cujos domínios de influência contêm a posição da partícula.

## Primeira meta científica

Reconstruir o método em camadas:

1. Validar MLS/EFG isoladamente.
2. Validar o solver de Poisson.
3. Validar conservação de carga no particionamento PIDC.
4. Implementar o ciclo PIDC completo.
5. Comparar com um PIC regular de referência.
6. Reproduzir, de forma controlada, os resultados qualitativos da tese.

## Estrutura planejada

```text
pidc-plasma-cpp/
├── AGENTS.md
├── GEMINI.md
├── CLAUDE.md
├── CODEX.md
├── TODO.md
├── ROADMAP.md
├── docs/
├── include/
├── src/
├── apps/
├── tests/
├── configs/
├── scripts/
├── data/
└── ai_logs/
```

## Como as IAs devem trabalhar

Este repositório foi pensado para cooperação entre **Gemini**, **Codex** e **Claude**.

Antes de qualquer alteração, cada IA deve ler:

1. `AGENTS.md`
2. `TODO.md`
3. `ROADMAP.md`
4. `docs/ai/CURRENT_CONTEXT.md`
5. `docs/ai/AI_BOARD.md`
6. `docs/ai/DECISIONS.md`

Ao terminar uma tarefa, a IA deve atualizar:

- `docs/ai/AI_STATUS.md`
- `docs/ai/AI_BOARD.md`
- um arquivo próprio em `ai_logs/`

## Princípio do projeto

Não pular para a simulação grande de 360.000 partículas sem antes validar os blocos matemáticos e computacionais menores.

A reconstrução deve honrar o trabalho original, mas corrigir sua fragilidade principal: a falta de uma validação quantitativa robusta do plasma PIDC.
