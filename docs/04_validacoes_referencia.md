# 04 — Validações e soluções de referência

## Filosofia

O projeto só deve avançar para o plasma PIDC depois que os blocos isolados estiverem validados.

## Validação 1 — Partição da unidade

Para pontos de avaliação internos:

```math
\sum_i \phi_i(x) = 1
```

### Saída esperada

```text
max_abs(sum_phi - 1) < tolerancia
```

## Validação 2 — Reprodução constante

Se `u_i = c`, então:

```math
u^h(x) = c
```

## Validação 3 — Reprodução linear

Se:

```math
u(x,y) = a + bx + cy
```

então:

```math
u^h(x,y) \approx u(x,y)
```

e:

```math
\nabla u^h(x,y) \approx [b, c]
```

## Validação 4 — Conservação de carga PIDC

Para cada partícula:

```math
q_p = \sum_i \phi_i(x_p)q_p
```

Globalmente:

```math
\sum_p q_p = \sum_i Q_i
```

## Validação 5 — Poisson MMS

Domínio:

```text
[0,1] x [0,1]
```

Solução:

```math
u(x,y) = \sin(\pi x)\sin(\pi y)
```

Fonte:

```math
\rho(x,y) = -2\pi^2\sin(\pi x)\sin(\pi y)
```

Campo:

```math
E_x = -\pi\cos(\pi x)\sin(\pi y)
```

```math
E_y = -\pi\sin(\pi x)\cos(\pi y)
```

Métricas:

- L2 de `u`;
- L∞ de `u`;
- L2 de `E`;
- L∞ de `E`.

## Validação 6 — Campo manufaturado

Escolher potencial linear:

```math
u(x,y) = a + bx + cy
```

Campo exato:

```math
\mathbf{E} = [-b, -c]
```

Objetivo: validar diretamente `grad_phi`.

## Validação 7 — Oscilação de Langmuir

Perturbar elétrons contra fundo iônico fixo/quase fixo.

Métrica:

```math
\omega_{observado} \approx \omega_{pe}
```

## Validação 8 — Comparação PIC versus PIDC

Rodar o mesmo caso simples com:

- PIC regular;
- PIDC.

Comparar:

- potencial;
- campo elétrico;
- energia;
- carga;
- frequência dominante;
- custo computacional.

## Validação 9 — Reprodução qualitativa da tese

Parâmetros inspirados na tese:

- duas espécies;
- elétrons e prótons;
- `T_e = 10000 K`;
- `T_p = 1000 K`;
- domínio retangular da ordem de `100 lambda_D`;
- contorno periódico;
- `dt = 0.2 / omega_pe`;
- visualização de potencial e campo.

Rodar em três escalas:

```text
small  : 1.000 + 1.000 partículas
medium : 10.000 + 10.000 partículas
large  : 180.000 + 180.000 partículas
```
