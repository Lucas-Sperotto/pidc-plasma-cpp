# 03 — Modelo matemático mínimo

## 1. Comprimento de Debye

O comprimento de Debye define a escala espacial coletiva relevante:

```math
\lambda_D =
\left(
\frac{\varepsilon_0 K T}{n_e e^2}
\right)^{1/2}
```

No PIDC, os raios de influência devem ser da ordem de `lambda_D`.

## 2. Frequência plasma-elétron

```math
\omega_{pe} =
\left(
\frac{n_e e^2}{\varepsilon_0 m_e}
\right)^{1/2}
```

O passo de tempo inicial recomendado é:

```math
\Delta t = 0.2 / \omega_{pe}
```

## 3. Poisson eletrostático

Usando potencial escalar:

```math
\mathbf{E} = -\nabla u
```

Conforme a decisão `DEC-0005`, o projeto adota o Sistema Internacional (SI) de unidades. A equação de Poisson para o potencial elétrico `u` em um meio com permissividade elétrica `ε` e densidade de carga volumétrica `ρ` é:

```math
\nabla \cdot (\varepsilon \nabla u) = -\rho
```

Para o vácuo ou plasma pouco denso, onde `ε ≈ ε₀` (permissividade do vácuo), a equação se simplifica para:

```math
\nabla^2 u = -\frac{\rho}{\varepsilon_0}
```

## 4. Aproximação EFG/MLS

```math
u^h(x) = \sum_i \phi_i(x) u_i
```

com:

```math
\sum_i \phi_i(x) = 1
```

## 5. Particionamento de carga PIDC

Para uma partícula `p` na posição `x_p`:

```math
q_p = \sum_i \phi_i(x_p)q_p
```

Carga acumulada no nó `i`:

```math
Q_i = \sum_p \phi_i(x_p) q_p
```

## 6. Interpolação do campo

```math
\mathbf{E}(x_p) =
-\sum_i u_i \nabla \phi_i(x_p)
```

O conjunto de nós usado aqui deve ser o mesmo conjunto da célula difusa usado na deposição de carga.

## 7. Leap-frog

```math
\mathbf{v}^{n+1/2} =
\mathbf{v}^{n-1/2} + \mathbf{a}^n \Delta t
```

```math
\mathbf{x}^{n+1} =
\mathbf{x}^{n} + \mathbf{v}^{n+1/2}\Delta t
```

com:

```math
\mathbf{a}_p = \frac{q_p}{m_p}\mathbf{E}(x_p)
```

## 8. Pontos de atenção

- **Unidades e Sinais**: A convenção de unidades (SI) e o sinal da equação de Poisson foram definidos em `DEC-0005`. A implementação deve ser consistente com `∇²u = -ρ/ε₀`.
- **Contorno Periódico**: A estratégia para o problema de Poisson com condições de contorno periódicas foi definida em `DEC-0007`. A implementação inicial focará em condições de Dirichlet, adiando a complexidade do caso periódico.
- **Conservação de Carga**: A formulação da deposição de carga (`Q_i = Σ_p φ_i(x_p) q_p`) e a propriedade de partição da unidade (`Σ_i φ_i(x) = 1`) garantem a conservação de carga global (`Σ_p q_p = Σ_i Q_i`). Testes devem validar essa propriedade numericamente.
- **Interpolação de Campo**: O campo elétrico sobre uma partícula `p` deve ser calculado usando os mesmos nós e funções de forma da sua "célula difusa" usada na deposição de carga, para garantir a conservação de momento (ausência de auto-força).
