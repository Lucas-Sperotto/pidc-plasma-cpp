# Plano de Comparação — PIC-FD vs. PIDC

**ID da Tarefa:** T-046
**Status:** Proposta (Gemini) -> Para revisão (Claude)
**Data:** 2026-05-09

---

## 1. Objetivo

Definir um caso de teste comum, quantitativo e fisicamente relevante para comparar o desempenho e a precisão do código de referência PIC com grade (Particle-In-Cell with Finite-Differences) e o código PIDC (Particle-In-Diffuse-Cell).

## 2. Princípio Orientador

A comparação deve ser "maçãs com maçãs". Conforme a descrição da tarefa T-046, é proibido misturar dimensionalidade e condições de contorno.

- O PIC de referência atual é 1D periódico.
- O PIDC atual é 2D com contorno de Dirichlet.

Uma comparação direta é impossível. É necessário definir um novo cenário comum.

## 3. Decisão Proposta (ver DEC-0035)

A comparação será realizada no seguinte cenário:

- **Dimensionalidade:** 2D
- **Condições de Contorno:** Periódicas em X e Y.
- **Caso Físico:** Instabilidade de duas correntes (Two-Stream Instability).

### Justificativa

- **2D:** É a dimensionalidade nativa do solver EFG/PIDC implementado e permite a observação de fenômenos mais ricos que o 1D.
- **Periódico:** É a condição de contorno mais comum para problemas de plasma homogêneo, como a instabilidade de duas correntes, e é um requisito para a reprodução da tese (Marco 7).
- **Instabilidade de Duas Correntes:** É um problema canônico em física de plasmas, ideal para validar um código eletrostático. Ele testa todo o ciclo (deposição, solver, interpolação, avanço) e possui uma taxa de crescimento teórica bem definida com a qual os resultados numéricos podem ser comparados.

## 4. Especificação do Caso de Teste (Proposta Inicial)

- **Domínio:** Quadrado, $L \times L$, com $L = 2\pi$.
- **Partículas:** Duas espécies de elétrons, mesma carga e massa.
  - Corrente 1: $N/2$ partículas com velocidade inicial $v_0 \hat{x}$.
  - Corrente 2: $N/2$ partículas com velocidade inicial $-v_0 \hat{x}$.
  - Uma pequena perturbação na posição é adicionada para iniciar a instabilidade.
- **Parâmetros Numéricos:**
  - PIC-FD: Grade de $64 \times 64$.
  - PIDC: Nuvem de nós regular $64 \times 64$.
  - Número total de partículas $N$: a ser definido (e.g., $4 \times 64^2 = 16384$).
  - Passo de tempo `dt`: a ser definido para resolver a frequência de plasma.

## 5. Métricas de Comparação

1.  **Taxa de Crescimento:** A energia do campo eletrostático deve crescer exponencialmente no início. A taxa de crescimento medida (via FFT da série temporal da energia) será comparada com a teoria.
2.  **Conservação de Energia:** A energia total (cinética + potencial) deve ser conservada com alta precisão. A deriva de energia será medida para ambos os códigos.
3.  **Espaço de Fase:** Gráficos de $(x, v_x)$ em diferentes instantes de tempo para visualizar a formação de vórtices.
4.  **Custo Computacional:** Tempo de CPU por passo de tempo por partícula.

## 6. Roadmap de Implementação (Pré-requisitos para o Marco 6)

A execução desta comparação requer um trabalho de desenvolvimento significativo:

1.  **Caminho do PIDC Periódico:**
    -   Implementar busca de vizinhança periódica em `NeighborSearchGrid` (resolve R-016).
    -   Implementar avaliação de MLS periódica em `MLSShapeFunction` (resolve R-015).
    -   Implementar um solver de Poisson EFG 2D periódico (provavelmente baseado em FFT ou um sistema linear com restrições).

2.  **Caminho do PIC-FD 2D Periódico (novo módulo):**
    -   Criar `Grid2D`.
    -   Implementar deposição de carga 2D (e.g., CIC).
    -   Implementar solver de Poisson 2D periódico (FFT é o padrão).
    -   Implementar interpolação de campo 2D.

## 7. Próximo Passo (Claude)

Revisar este plano, analisar os riscos associados ao desenvolvimento dos pré-requisitos e refinar as especificações do caso de teste e das métricas.

---

## 8. Revisão de Claude (T-046, 2026-05-09)

### 8.1 O que está correto na proposta do Gemini

- **2D como dimensionalidade comum:** correto. Comparar PIC 1D vs PIDC 2D seria
  incomparável. Definir tudo em 2D é a única opção coerente.
- **Princípio "maçãs com maçãs":** correto. Não misturar dimensionalidade
  ou condições de contorno.
- **Instabilidade de duas correntes como caso de longo prazo:** correto. É o
  caso canônico para validação de códigos eletrostáticos 2D e é o mais relevante
  para a tese de Marques. Deve ser o objetivo do **Marco 7**, não do Marco 6.

### 8.2 O que precisa ser revisado

**Problema central: DEC-0035 supõe pré-requisitos não implementados.**

Para executar a instabilidade de duas correntes 2D periódica, são necessários:

1. **R-015 resolvido:** `mls_evaluate` periódico (imagem mínima na busca de vizinhos)
2. **R-016 resolvido:** `NeighborSearchGrid` periódico
3. **Solver de Poisson EFG 2D periódico:** completamente novo (DEC-0007 identificou
   complexidade; nunca implementado)
4. **PIC-FD 2D periódico:** grade 2D, deposição CIC 2D, solver FFT 2D, interpolação 2D
5. **R-022 resolvido:** `initialize_pidc_velocity_2d` (T-047, DEC-0034)

Nenhum desses blocos existe. Isso representa semanas de desenvolvimento antes de
qualquer comparação. Isso viola o princípio do projeto (AGENTS.md §1):
*"A prioridade não é 'fazer rodar rápido'. A prioridade é: entender, validar,
reproduzir, otimizar."*

O Marco 6 exige "caso controlado". A instabilidade de duas correntes não é
controlada — envolve modos oblíquos 2D, taxa de crescimento dependente de k,
saturação não-linear. Qualquer discrepância entre PIC e PIDC será difícil de
diagnosticar porque o sistema já é complexo.

### 8.3 Proposta revisada: Marco 6 em duas fases

#### Marco 6 — Fase A: comparação com campo manufaturado (sem periodicidade)

**Caso:** campo manufaturado estático E(x,y) = E₀(x,y) conhecido, mesmo campo
aplicado a N partículas em PIC-2D e em PIDC.

**Por que funciona:** Remove o Poisson da comparação. Ambos os métodos recebem
o mesmo campo; a diferença entre as trajetórias mede apenas o erro do leapfrog
2D e do contorno. Esse é o caso mais controlado possível.

**Implementação necessária:**
- T-047 (leapfrog init) — pré-condição
- `PIC2DManufactured` — apenas leapfrog 2D sobre campo manufaturado (sem deposição,
  sem Poisson); análogo ao `SineManufacturedField1D` da Fase E

**Métricas:** trajetória analítica vs. numérica; erro L∞ de posição após N passos.

#### Marco 6 — Fase B: Langmuir 2D Dirichlet (PIC-FD 2D vs PIDC)

**Caso:** perturbação senoidal no domínio [0,1]² com BC Dirichlet u=0, N partículas,
campo self-consistent. Frequência dominante medida por FFT.

**Implementação necessária (nova para PIC-FD 2D Dirichlet):**
- `Grid2D` (nx × ny, espaçamento uniforme)
- `deposit_charge_cic_2d` (bilinear)
- `PoissonSolver2D_FD` (laplaciano 5-pontos, Eigen SimplicialLDLT, BCs Dirichlet)
- `interpolate_field_cic_2d`

**Esta rota evita R-015/R-016 inteiramente.** Usa apenas o que já existe no PIDC
(solver EFG com Dirichlet), sem exigir periodicidade.

**Métricas:** energia total, carga conservada, frequência dominante de E(t).

#### Marco 7: instabilidade de duas correntes 2D periódica (proposta Gemini)

A proposta original da Gemini (DEC-0035) é correta como objetivo de Marco 7.
Requer resolver R-015, R-016, novo solver periódico, e PIC-FD 2D periódico.

### 8.4 Ação concreta proposta

1. **Modificar DEC-0035:** aceitar o princípio (2D, mesma dimensão), redirecionar
   o caso específico (instabilidade de duas correntes) para Marco 7.
2. **Adicionar DEC-0036:** Fase A de Marco 6 — comparação com campo manufaturado.
3. **T-047 (Codex):** implementar `initialize_pidc_velocity_2d` (pré-condição).
4. **T-048 (Codex):** implementar `PIC2DManufactured` + comparação Fase A.
5. **T-049 (Codex):** módulo PIC-FD 2D Dirichlet + comparação Fase B.

### 8.5 Resumo da revisão

| Aspecto | Avaliação |
|---|---|
| 2D como dimensionalidade comum | ✓ Aceito |
| "Maçãs com maçãs" como princípio | ✓ Aceito |
| Instabilidade de duas correntes | ✗ Muito ambicioso para Marco 6 → Marco 7 |
| Periodicidade em Marco 6 | ✗ Requer R-015/R-016 não implementados |
| Pré-condição T-047 explicitada | ✓ Necessária antes de qualquer fase |
| Sequência incremental (Fase A → B → 7) | ✓ Proposta como revisão |