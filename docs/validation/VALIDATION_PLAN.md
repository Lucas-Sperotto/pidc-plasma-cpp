# VALIDATION_PLAN — Critérios de aceite quantitativos

Atualizado em: 2026-05-08

Este documento define as tolerâncias numéricas que devem ser respeitadas em
cada fase de validação. Um teste só é considerado aprovado se **todas** as
propriedades abaixo forem satisfeitas com as tolerâncias indicadas.

---

## Fase C — MLS: funções de forma e gradientes

### Configuração de referência

- Nuvem regular 5×5 em $[0,1]^2$, passo $h_g = 0.25$.
- Raio de suporte $h = 0.6$ (cobre ≈ 12 vizinhos no interior).
- Ponto de consulta interior arbitrário (não restrito ao centro).

### Propriedades obrigatórias

| Propriedade | Expressão | Tolerância |
| --- | --- | --- |
| Partição da unidade | $\|\sum_i \phi_i(\mathbf{x}) - 1\|$ | $< 10^{-10}$ |
| Reprodução linear x | $\|\sum_i \phi_i x_i - x\|$ | $< 10^{-10}$ |
| Reprodução linear y | $\|\sum_i \phi_i y_i - y\|$ | $< 10^{-10}$ |
| Gradiente PU — x | $\|\sum_i \partial_x\phi_i\|$ | $< 10^{-10}$ |
| Gradiente PU — y | $\|\sum_i \partial_y\phi_i\|$ | $< 10^{-10}$ |
| Gradiente LR — $\partial_x\phi \cdot x_i$ | $\|\sum_i \partial_x\phi_i\,x_i - 1\|$ | $< 10^{-10}$ |
| Gradiente LR — $\partial_x\phi \cdot y_i$ | $\|\sum_i \partial_x\phi_i\,y_i\|$ | $< 10^{-10}$ |
| Gradiente LR — $\partial_y\phi \cdot x_i$ | $\|\sum_i \partial_y\phi_i\,x_i\|$ | $< 10^{-10}$ |
| Gradiente LR — $\partial_y\phi \cdot y_i$ | $\|\sum_i \partial_y\phi_i\,y_i - 1\|$ | $< 10^{-10}$ |

A tolerância de $10^{-10}$ é um padrão de projeto para verificar propriedades que
devem ser satisfeitas até a precisão da máquina (double). A tese de Marques
(2008) não especifica tolerâncias para estes testes, portanto este é um critério
de robustez numérica definido para esta implementação.

As 9 propriedades devem ser verificadas em **pelo menos 4 pontos de consulta**,
incluindo pelo menos 3 pontos assimétricos (i.e., $x \neq y$ e $x,y \notin \{0, 0.25, 0.5, 0.75, 1.0\}$).

### Condicionamento

O número de condição de $A$ (razão entre maior e menor valor singular) deve
ser $< 10^8$ para que a solução do sistema linear seja numericamente estável.
Além dos testes de LR, `tests/test_mls_robustness.cpp` verifica explicitamente
esse limite nos pontos de consulta de referência (T-024).

### Robustez

- `mls_evaluate` deve lançar `std::invalid_argument` se o raio de suporte não
  for positivo e finito.
- `mls_evaluate` deve lançar `std::runtime_error` se $n < m = 3$.
- Nenhum `NaN` ou `Inf` deve aparecer nos vetores `phi` ou `grad_phi`
  para qualquer ponto de consulta com $n \geq 3$ vizinhos em posição geral.

### Implementação de referência

`tests/test_mls_shape_function.cpp` — cobre PU, LR e gradientes com
4 pontos de consulta: `{0.5,0.5}`, `{0.3,0.7}`, `{0.6,0.4}`, `{0.37,0.61}`.
Tolerância: $10^{-10}$.

`tests/test_mls_robustness.cpp` — cobre ausência de NaN/Inf, condicionamento
explícito de $A$ e falha controlada para raio de suporte inválido. 12/12 testes
passando em 2026-05-08.

---

## Fase D — Solver EFG de Poisson (MMS)

### Método das soluções manufaturadas (MMS)

Solução analítica:

$$u(x,y) = \sin(\pi x)\sin(\pi y), \qquad (x,y) \in [0,1]^2$$

Fonte correspondente ($-\Delta u = f$):

$$f(x,y) = 2\pi^2 \sin(\pi x)\sin(\pi y)$$

Condições de contorno: $u = 0$ em $\partial\Omega$ (Dirichlet homogêneo).

### Critério de aceite

| Métrica | Valor alvo |
| --- | --- |
| Erro $L^2$ do potencial (nuvem 5×5) | $< 10^{-2}$ |
| Erro $L^\infty$ do potencial (nuvem 5×5) | $< 10^{-2}$ |
| Erro $L^2$ do campo (nuvem 5×5) | $< 5\times10^{-2}$ |
| Erro $L^\infty$ do campo (nuvem 5×5) | $< 1.2\times10^{-1}$ |
| Erros em 9×9 | menores que os respectivos erros em 5×5 |
| Ausência de NaN/Inf | obrigatório |

O campo manufaturado é avaliado como $\mathbf{E} = -\nabla u$. A norma $L^\infty$
do campo usa a norma Euclidiana do erro vetorial em cada ponto de quadratura.

### Implementação de referência

`tests/test_gauss_cell2d.cpp` — verifica soma de pesos e integração de
polinômios simples por Gauss 2×2.

`tests/test_efg_poisson_mms.cpp` — usa `support_radius = 1.8*h_g`, monta e
resolve o sistema esparso inicial. Resultado em 2026-05-08 (T-035):

| Nuvem | Potencial L2 | Potencial L∞ | Campo L2 | Campo L∞ |
| --- | --- | --- | --- | --- |
| 5×5 | 0.00359683506821715 | 0.00698519578344722 | 0.0330520262581074 | 0.105393836778107 |
| 9×9 | 0.000827503569222836 | 0.00169751665145062 | 0.0136450944195131 | 0.0554928606259795 |

CTest: 14/14 testes passando.

---

## Fase E — PIC 1D de referência

### T-039B — Referências de deposição CIC 1D

Critérios de aceite em `tests/test_cic_deposition_reference_1d.cpp`:

| Propriedade | Tolerância |
| --- | --- |
| Uma partícula no centro de célula deposita 50/50 nos vizinhos | $< 10^{-12}$ |
| Uma partícula no centro do domínio sobre nó deposita 100% no nó | $< 10^{-12}$ |
| Conjunto determinístico com semente fixa é reprodutível | $< 10^{-12}$ |
| Deposição do conjunto fixo coincide com referência manual CIC | $< 10^{-12}$ |
| Conservação global no conjunto fixo | $< 10^{-12}$ |

Resultado em 2026-05-09 (T-039B): `cic_deposition_reference_1d` passou; CTest 23/23.

### T-040 — Campo manufaturado 1D em grade periódica

Campo analítico:

$$\phi(x) = A\sin(2\pi x/L)$$

$$E(x) = -A(2\pi/L)\cos(2\pi x/L)$$

$$\rho(x) = \epsilon_0 A(2\pi/L)^2\sin(2\pi x/L)$$

Critérios de aceite em `tests/test_manufactured_field_1d.cpp`:

| Propriedade | Tolerância |
| --- | --- |
| Valores analíticos em $0$, $L/4$, $L/2$, $3L/4$ | $< 10^{-12}$ |
| Periodicidade entre $x$ e $x \pm L$ | $< 10^{-12}$ |
| Tamanho dos vetores amostrados | igual a `grid.size()` |
| Soma nodal de $\rho_i$ em grade periódica uniforme | $< 10^{-12}$ |
| Ausência de Eigen no módulo PIC 1D | obrigatório |

Resultado em 2026-05-09 (T-040): `manufactured_field_1d` passou; CTest 18/18.

### T-041 — Poisson 1D periódico por FD/DFT manual

Equação resolvida:

$$\frac{\partial^2\phi}{\partial x^2} = -\frac{\rho}{\epsilon_0}$$

Condições:

- domínio periódico em `Grid1D`;
- gauge de potencial médio zero;
- vetores nodais `std::vector<double>`;
- sem Eigen, EFG, MLS ou PIDC.

Critérios de aceite em `tests/test_poisson_solver_1d.cpp`:

| Propriedade | Tolerância |
| --- | --- |
| Erro $L^\infty$ de $\phi$ contra `SineManufacturedField1D`, `nx=128` | $< 10^{-3}$ |
| Erro $L^\infty$ de $E$ contra `SineManufacturedField1D`, `nx=128` | $< 5\times10^{-3}$ |
| Média do potencial calculado | $< 10^{-12}$ |
| Densidade nula → potencial e campo nulos | $< 10^{-12}$ |
| Falha controlada para tamanho incorreto ou `epsilon0 <= 0` | obrigatório |

Resultado em 2026-05-09 (T-041): `poisson_solver_1d` passou; CTest 19/19.

### T-042 — Interpolação CIC campo → partícula

Regra de interpolação:

$$E_p = E_j(1-f) + E_{j+1}f$$

onde `j = grid.left_node_index(x_p)`, `j+1` é periódico e
`f = grid.fraction_in_cell(x_p)`.

Critérios de aceite em `tests/test_field_interpolation_1d.cpp`:

| Propriedade | Tolerância |
| --- | --- |
| Casos exatos em grid `[0,1)`, `nx=4`, campo `{0,4,8,12}` | $< 10^{-12}$ |
| Wrap periódico para posições negativas e maiores que `xmax` | $< 10^{-12}$ |
| Campo uniforme interpolado em posições arbitrárias | $< 10^{-12}$ |
| Sobrecarga para `std::vector<Particle>` preserva tamanho e ordem | obrigatório |
| Campo manufaturado senoidal amostrado com `nx=128` | erro absoluto $< 3\times10^{-3}$ |
| Falha controlada para tamanho incorreto ou valor não finito | obrigatório |

Resultado em 2026-05-09 (T-042): `field_interpolation_1d` passou; CTest 20/20.

### T-043 — Leap-frog 1D isolado

Convenção:

- antes de `initialize_leapfrog_velocity_1d`, `Particle::velocity.x` é `v(t=0)`;
- após a inicialização, `Particle::velocity.x` é `v(t=-dt/2)`;
- cada avanço aplica `v_half += (q/m)E dt` e depois
  `x = grid.wrap_position(x + v_half dt)`.

Critérios de aceite em `tests/test_leapfrog_1d.cpp`:

| Propriedade | Tolerância |
| --- | --- |
| Campo nulo → movimento uniforme | $< 10^{-12}$ |
| Campo constante → posição analítica em dois passos | $< 10^{-12}$ |
| Campo constante → velocidade de meio passo analítica | $< 10^{-12}$ |
| Reversibilidade drift-only com `dt` e `-dt` | $< 10^{-10}$ |
| Wrap periódico para cruzamento de `xmax` | $< 10^{-12}$ |
| Falha controlada para entrada não finita | obrigatório |

Resultado em 2026-05-09 (T-043): `leapfrog_1d` passou; CTest 21/21.

### T-043B — Contorno periódico PIC 1D em movimento

Critérios de aceite em `tests/test_pic_periodic_motion_1d.cpp`:

| Propriedade | Tolerância |
| --- | --- |
| Cruzamento pela direita em campo nulo | $< 10^{-12}$ |
| Cruzamento pela esquerda em campo nulo | $< 10^{-12}$ |
| Imagem exata de `xmax` retorna a `xmin` | $< 10^{-12}$ |
| Todas as partículas permanecem em `[xmin,xmax)` | obrigatório |
| Número de partículas preservado após múltiplos passos | obrigatório |
| Posições após múltiplos passos coincidem com `wrap(x0 + v*t)` | $< 10^{-12}$ |
| Velocidades em campo nulo permanecem constantes | $< 10^{-12}$ |

Resultado em 2026-05-09 (T-043B): `pic_periodic_motion_1d` passou; CTest 22/22.

---

## Fases posteriores (referência)

| Fase | Critério principal |
| --- | --- |
| E (PIC) | conservação de carga $< 10^{-10}$ relativo por passo |
| F (PIDC) | Langmuir 1D: frequência de plasma dentro de 1% do valor teórico |
| G/H | reprodução qualitativa das figuras da tese (seções 4 e 5) |
