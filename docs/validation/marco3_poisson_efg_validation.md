# Relatório de Validação — Marco 3: Poisson por EFG

**Projeto:** PIDC Plasma C++
**Data:** 2026-05-09
**Versão:** 1.0
**Método:** Method of Manufactured Solutions (MMS)
**Status:** ✓ VALIDADO

---

## 1. Objetivo

Verificar que o solver EFG Poisson 2D (`EFGPoissonSolver`) resolve corretamente
a equação de Poisson em um domínio quadrado com condições de contorno de Dirichlet,
produzindo erros que decrescem monotonicamente com o refinamento da nuvem de nós e
que exibem ordem de convergência consistente com a teoria de elementos livres de
galerkin (EFG) com base polinomial linear.

"Validado" significa: (a) erro L² do potencial decrescente e de ordem ~2 em grade
suficientemente fina; (b) erro L² do campo decrescente e de ordem ~1; (c) nenhum
NaN ou Inf; (d) simetria da matriz de rigidez preservada após imposição de Dirichlet.

---

## 2. Metodologia

### 2.1 Método das Soluções Manufaturadas (MMS)

A solução exata é escolhida antes de resolver o problema numérico. O termo fonte
é derivado analiticamente para satisfazer a equação de Poisson com aquela solução.
O erro numérico é então medido contra a solução exata conhecida.

**Solução manufaturada:**

$$u(x, y) = \sin(\pi x)\sin(\pi y), \quad (x, y) \in [0, 1]^2$$

**Justificativa da escolha:**
- Satisfaz automaticamente a condição de Dirichlet homogênea em todas as quatro
  paredes (sin(0) = sin(π) = 0).
- Gradiente não-trivial em ambas as direções (simetria x-y → diagnóstico limpo).
- Suave e analítica → convergência assintótica esperada para qualquer método consistente.

**Termo fonte** (derivado de $-\Delta u = f$):

$$f(x, y) = 2\pi^2 \sin(\pi x)\sin(\pi y)$$

**Campo elétrico exato** ($\mathbf{E} = -\nabla u$):

$$E_x = -\pi\cos(\pi x)\sin(\pi y), \quad E_y = -\pi\sin(\pi x)\cos(\pi y)$$

**Condição de contorno:** $u = 0$ em $\partial\Omega$ (Dirichlet homogêneo).

### 2.2 Métricas de erro

Os erros são avaliados nos pontos de quadratura de Gauss 2×2 de cada célula de
integração (o mesmo conjunto usado na montagem de $K$ e $b$):

$$\|e_\phi\|_{L^2} = \sqrt{\sum_q w_q \left(u_h(\mathbf{x}_q) - u(\mathbf{x}_q)\right)^2}$$

$$\|e_\phi\|_{L^\infty} = \max_q \left|u_h(\mathbf{x}_q) - u(\mathbf{x}_q)\right|$$

$$\|e_E\|_{L^2} = \sqrt{\sum_q w_q \left\|\mathbf{E}_h(\mathbf{x}_q) - \mathbf{E}(\mathbf{x}_q)\right\|^2}$$

$$\|e_E\|_{L^\infty} = \max_q \left\|\mathbf{E}_h(\mathbf{x}_q) - \mathbf{E}(\mathbf{x}_q)\right\|$$

onde $u_h = \sum_i \phi_i u_i$ e $\mathbf{E}_h = -\sum_i \nabla\phi_i u_i$ usando
as funções de forma MLS e seus gradientes.

A ordem de convergência entre dois refinamentos consecutivos é:

$$p = \frac{\ln(e_{\text{prev}} / e_{\text{curr}})}{\ln(h_{\text{prev}} / h_{\text{curr}})}$$

---

## 3. Discretização

| Parâmetro | Valor |
|---|---|
| Domínio | $[0,1]^2$ |
| Nuvem de nós | regular $n \times n$, $n \in \{5, 7, 9, 13, 17\}$ |
| Espaçamento | $h = 1/(n-1)$ |
| Raio de suporte MLS | $1.8\,h$ |
| Função peso MLS | spline quártica (DEC-0013) |
| Base polinomial | linear 2D, $m = 3$ (DEC-0014) |
| Células de integração | $(n-1)^2$ células retangulares |
| Quadratura | Gauss $2 \times 2$ por célula (DEC-0018) |
| Dirichlet | método da penalidade, $\alpha = 10^{12} \cdot \max(K_{ii})$ (DEC-0024) |
| Backend linear | `Eigen::SimplicialLDLT` (DEC-0025) |

---

## 4. Resultados

### 4.1 Tabela de convergência

| n  | h       | L²(φ)      | L∞(φ)      | L²(E)      | L∞(E)      | ord L²(φ) | ord L²(E) |
|----|---------|------------|------------|------------|------------|-----------|-----------|
|  5 | 0.2500  | 3.597e-03  | 6.985e-03  | 3.305e-02  | 1.054e-01  | —         | —         |
|  7 | 0.1667  | 1.515e-03  | 3.036e-03  | 1.925e-02  | 7.330e-02  | 2.13      | 1.33      |
|  9 | 0.1250  | 8.275e-04  | 1.698e-03  | 1.365e-02  | 5.549e-02  | 2.10      | 1.20      |
| 13 | 0.0833  | 3.556e-04  | 7.521e-04  | 8.795e-03  | 3.713e-02  | 2.08      | 1.08      |
| 17 | 0.0625  | 1.963e-04  | 4.228e-04  | 6.563e-03  | 2.786e-02  | 2.06      | 1.02      |

Dados gerados por `apps/marco3_mms_diagnostics.cpp` em 2026-05-09.

### 4.2 Figuras

- `docs/figures/efg_mms_convergence.png` — convergência log-log L²(φ) e L²(E) com
  retas de referência O(h²) e O(h)
- `docs/figures/efg_mms_orders.png` — barplot de ordens de convergência por par de
  refinamento

---

## 5. Critérios de aceite

| Critério | Valor alvo | Resultado | Status |
|---|---|---|---|
| L²(φ) em n=5 | < 1.0e-2 | 3.60e-03 | ✓ |
| L∞(φ) em n=5 | < 1.0e-2 | 6.99e-03 | ✓ |
| L²(E) em n=5 | < 5.0e-2 | 3.31e-02 | ✓ |
| L∞(E) em n=5 | < 1.2e-1 | 1.05e-01 | ✓ |
| Erros L²(φ) monotone decrescentes (5→17) | sim | sim | ✓ |
| Erros L²(E) monotone decrescentes (5→17) | sim | sim | ✓ |
| Ordem L²(φ) no par 13→17 | > 1.7 | 2.06 | ✓ |
| Simetria de K após Dirichlet | < 1e-10 | < 1e-10 | ✓ |
| Nós de contorno \|u_h\| | < 1e-10 | < 1e-10 | ✓ |
| Solução finita (sem NaN/Inf) | sim | sim | ✓ |

---

## 6. Discussão

### 6.1 Convergência do potencial

O potencial exibe convergência O(h²) consistente para todos os pares de
refinamento (ordem medida entre 2.06 e 2.13). Isso é compatível com o esperado
teórico para o método de Galerkin com base MLS linear: o erro de aproximação MLS
é O(h²) para funções suficientemente suaves, e o estimador de erro de Galerkin
herda essa taxa quando a quadratura é suficientemente acurada (Gauss 2×2 é exato
para polinômios de grau ≤ 3).

### 6.2 Convergência do campo — regime pré-assintótico

O campo é calculado como $\mathbf{E}_h = -\sum_i \nabla\phi_i u_i$. A derivada da
função de forma MLS tem ordem de convergência O(h), inferior à do potencial.

**Em grades grossas (n=5→9):** a ordem medida do campo é 1.20–1.33, acima do
esperado assintótico. Isso é um artefato de regime pré-assintótico: com n=5, o raio
de suporte (1.8h = 0.45) cobre 45% da largura do domínio, e a vizinhança de cada
ponto inclui a maioria dos nós. O sistema é sobre-determinado nesse regime e a
função de forma MLS produz interpolação mais agressiva do que a que seria obtida
em regime de suporte local.

**Em grades finas (n=13→17):** a ordem cai para ~1.02, muito próxima ao esperado
teórico O(h) para gradientes MLS. A precisão absoluta é L²(E) = 6.6e-03 em n=17,
suficiente para o propósito físico do solver (campo elétrico para aceleração de
partículas no PIDC).

Este comportamento é consistente com a literatura de EFG para grades meshless
grossas e não representa um defeito de implementação.

### 6.3 Verificação da simetria e Dirichlet

O método da penalidade (DEC-0024) preserva a simetria da matriz de rigidez com
precisão de máquina (erro máximo de simetria < 1e-10), o que é necessário para o
`SimplicialLDLT` convergir. Todos os nós de contorno satisfazem |u_h| < 1e-10
após a solução, confirmando a correta imposição da condição de Dirichlet.

---

## 7. Rastreabilidade

| Artefato | Caminho |
|---|---|
| Solver | `include/pidc/efg/EFGPoissonSolver.hpp` |
| Funções de forma MLS | `include/pidc/mls/MLSShapeFunction.hpp` |
| Células de Gauss | `include/pidc/efg/GaussCell2D.hpp` |
| Teste de unidade | `tests/test_efg_poisson_mms.cpp` |
| App diagnóstico | `apps/marco3_mms_diagnostics.cpp` |
| Dados exportados | `data/output/efg_mms_convergence.csv` |
| Figura convergência | `docs/figures/efg_mms_convergence.png` |
| Figura ordens | `docs/figures/efg_mms_orders.png` |
| Decisão Dirichlet | DEC-0024 (penalidade) |
| Decisão backend | DEC-0025 (SimplicialLDLT) |
| Decisão quadratura | DEC-0018 (Gauss 2×2) |
| Decisão peso MLS | DEC-0013 (spline quártica) |
| Decisão base MLS | DEC-0014 (linear 2D) |

---

## 8. Referências metodológicas

- Belytschko, T., Lu, Y.Y., Gu, L. (1994). Element-free Galerkin methods.
  *International Journal for Numerical Methods in Engineering*, 37(2), 229–256.
- Marques, G.N. (2008). *O Método Particle-In-Diffuse-Cell: uma abordagem Meshfree
  para simulação de plasmas*. Tese, INPE.
- Oberkampf, W.L., Trucano, T.G. (2002). Verification and validation in computational
  fluid dynamics. *Progress in Aerospace Sciences*, 38(3), 209–272.
