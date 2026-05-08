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

```math
\nabla \cdot (\varepsilon \nabla u) = -\rho
```

ou, no sistema de unidades adotado pela tese/publicação:

```math
\nabla \cdot (\varepsilon \nabla u) = -4\pi \rho
```

A escolha de unidade deve ser explicitada no código.

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

- Definir claramente SI versus CGS/Gaussiano.
- Evitar misturar `-rho`, `rho`, `4*pi*rho` sem documentação.
- Registrar sinal da equação de Poisson.
- Registrar como o potencial é fixado em contorno periódico.
- Para Poisson periódico, remover modo médio ou fixar gauge.
