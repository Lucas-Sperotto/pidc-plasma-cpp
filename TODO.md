# TODO — Reconstrução científica e computacional do PIDC

Este arquivo é o mapa mestre de pendências do repositório.

## 0. Estado inicial

- [x] Planejamento conceitual do repositório.
- [x] Separação das fases: EFG/MLS, Poisson, PIC, PIDC e reprodução da tese.
- [x] Criação do protocolo de cooperação entre Gemini, Codex e Claude.
- [x] Implementação do esqueleto CMake real.
- [ ] Implementação do núcleo matemático.
- [x] Primeiros testes unitários.

---

## 1. Fase A — Base do repositório

### Objetivo

Criar um repositório limpo, compilável e preparado para desenvolvimento incremental.

### Tarefas

- [x] Criar `CMakeLists.txt` funcional.
- [x] Definir padrão C++17.
- [x] Adicionar dependência Eigen.
- [ ] Adicionar opção futura para `nlohmann_json`.
- [x] Criar diretórios `include/pidc`, `src`, `apps`, `tests`, `configs`, `scripts`.
- [x] Criar script `scripts/build.sh`.
- [x] Criar script `scripts/run_tests.sh`.
- [x] Criar primeiro executável `apps/smoke.cpp`.

### Critério de aceite

```bash
cmake -S . -B build
cmake --build build -j
ctest --test-dir build --output-on-failure
```

deve executar sem erro.

---

## 2. Fase B — Núcleo geométrico e partículas

### Objetivo

Criar as estruturas mínimas para nós, partículas, domínio e busca de vizinhança.

### Tarefas

- [x] `Vec2`
- [x] `Particle`
- [x] `Species`
- [x] `Node`
- [x] `Domain2D`
- [x] `NodeCloud`
- [x] `PeriodicBoundary2D`
- [x] `RegularNodeCloud2D`
- [x] `NeighborSearchGrid`

### Critérios de aceite

- [x] Uma partícula que sai pela direita deve reaparecer pela esquerda.
- [x] Uma partícula que sai pelo topo deve reaparecer pela base.
- [x] A busca de vizinhança deve retornar todos os nós cujo domínio de influência contém o ponto de avaliação.
- [x] Nenhum teste deve depender de ordem aleatória.

---

## 3. Fase C — MLS/EFG

### Objetivo

Implementar funções de forma e gradientes.

### Tarefas

- [x] Implementar base linear 2D: `p(x,y) = [1, x, y]`.
- [x] Implementar função peso compacta.
- [ ] Implementar domínios circulares e retangulares.
- [x] Implementar cálculo de `phi_i(x)` — `mls_evaluate` em MLSShapeFunction.hpp (T-017).
- [x] Implementar cálculo de `grad(phi_i)(x)` — diferenciação implícita em mls_evaluate (T-017).
- [x] Monitorar cardinalidade local `n >= m` — lança `std::runtime_error` se n < 3 (T-017).
- [x] Monitorar condicionamento da matriz MLS — teste explícito em `mls_robustness` (T-024).

### Testes obrigatórios

- [x] Partição da unidade: `sum_i phi_i(x) = 1` — test_mls_shape_function (T-017).
- [x] Reprodução constante: `u(x,y)=c` — coberta por partição da unidade (T-017).
- [x] Reprodução linear: `u(x,y)=a+bx+cy` — test_mls_shape_function (T-017).
- [x] Gradiente de função linear: `grad(u) = [b, c]` — gradientes de LR em test_mls_shape_function (T-017).
- [x] Nenhum `NaN` ou `Inf` — teste explícito em `mls_robustness` (T-024).
- [x] Falha controlada se `n < m` — lança runtime_error com < 3 vizinhos (T-017).

### Critério de aceite

Erro de reprodução linear menor que tolerância definida em `docs/validation/VALIDATION_PLAN.md`.

---

## 4. Fase D — Solver EFG de Poisson

### Objetivo

Resolver a equação de Poisson para casos manufaturados.

### Tarefas

- [x] Definir células de integração.
- [x] Implementar quadratura de Gauss 2D.
- [x] Montar matriz de rigidez `K`.
- [x] Montar vetor de fontes `b`.
- [x] Impor Dirichlet por substituição direta no caso interpolante.
- [x] Resolver inicialmente com matriz densa.
- [ ] Migrar para matriz esparsa após validação.

### Caso de referência

```math
u(x,y) = \sin(\pi x)\sin(\pi y)
```

```math
\nabla^2 u = -2\pi^2\sin(\pi x)\sin(\pi y)
```

### Métricas

- [x] erro L2 do potencial;
- [ ] erro L∞ do potencial;
- [ ] erro L2 do campo;
- [ ] erro L∞ do campo;
- [x] estudo de convergência ao refinar a nuvem.

---

## 5. Fase E — PIC eletrostático de referência

### Objetivo

Implementar um PIC simples para comparação com o PIDC.

### Tarefas

- [ ] Deposição de carga em grade regular.
- [ ] Solver de Poisson por diferenças finitas ou FFT.
- [ ] Interpolação do campo para partículas.
- [ ] Leap-frog.
- [ ] Contorno periódico.
- [ ] Caso físico de oscilação de Langmuir 1D.

### Critério de aceite

A frequência dominante observada deve se aproximar da frequência plasma-elétron dentro da tolerância definida.

---

## 6. Fase F — PIDC completo

### Objetivo

Implementar o ciclo completo Particle-In-Diffuse-Cell.

### Ciclo

```text
para cada passo de tempo:
    zerar cargas nodais

    para cada partícula:
        localizar célula difusa
        calcular phi_i(xp) e grad_phi_i(xp)
        depositar q_p * phi_i(xp)

    montar/recalcular vetor de fontes b
    resolver K u = b

    para cada partícula:
        interpolar E(xp) = -sum_i u_i grad_phi_i(xp)
        calcular aceleração
        avançar leap-frog
        aplicar contorno periódico
```

### Tarefas

- [ ] `DiffuseCell`
- [ ] `PIDCChargeDepositor`
- [ ] `PIDCFieldInterpolator`
- [ ] `PIDCLoop`
- [ ] Reaproveitar `ShapeFunctionData` entre deposição e interpolação.
- [ ] Manter `K` constante quando a nuvem não muda.
- [ ] Recalcular apenas `b` por passo de tempo.

### Testes

- [ ] Conservação de carga por partícula.
- [ ] Conservação de carga global.
- [ ] Interpolação correta para campo manufaturado.
- [ ] Estabilidade do leap-frog em caso simples.
- [ ] Contorno periódico conservando número de partículas.

---

## 7. Fase G — Soluções de referência

### Casos

- [x] Poisson MMS 2D.
- [ ] Deposição de carga com uma partícula no centro.
- [ ] Deposição de carga com partículas aleatórias e semente fixa.
- [ ] Campo elétrico manufaturado.
- [ ] Oscilação de Langmuir 1D.
- [ ] Comparação PIC-FD versus PIDC.
- [ ] Reprodução qualitativa da tese.

---

## 8. Fase H — Reprodução da tese

### Caso pequeno

- [ ] 1.000 elétrons + 1.000 prótons.
- [ ] Temperaturas iniciais baseadas na tese.
- [ ] Domínio da ordem de 100 comprimentos de Debye.
- [ ] Contorno periódico.
- [ ] `dt = 0.2 / omega_pe`.
- [ ] Saída em `data/output/thesis_small/`.

### Caso médio

- [ ] 10.000 elétrons + 10.000 prótons.
- [ ] Mesmo contrato do caso pequeno.

### Caso grande

- [ ] 180.000 elétrons + 180.000 prótons.
- [ ] Reproduzir visualizações dos instantes iniciais.
- [ ] Exportar potencial, campo e estatísticas.

---

## 9. Fase I — Relatórios e artigo técnico

- [ ] `docs/reproduction/01_reproducao_qualitativa.md`
- [ ] `docs/reproduction/02_validacao_quantitativa.md`
- [ ] `docs/reproduction/03_limites_do_modelo.md`
- [ ] `docs/reproduction/04_comparacao_pic_pidc.md`
- [ ] Preparar material didático para alunos.
- [ ] Preparar artigo/nota técnica futura.

---

## 10. Regra de ouro

Nenhuma fase posterior deve mascarar erro de fase anterior.

Se MLS falha, o Poisson falha.
Se Poisson falha, o PIDC falha.
Se a deposição não conserva carga, a física do plasma fica comprometida.
