# DECISIONS — Registro de decisões técnicas

## DEC-0001 — Linguagem base

Status: aceita

Contexto:
O usuário deseja código C/C++ para reconstrução do PIDC.

Decisão:
Usar C++17 como linguagem principal.

Justificativa:
C++17 oferece boa estruturação, desempenho e integração com Eigen/CMake, mantendo compatibilidade com ambiente acadêmico.

Impacto no código:
`CMakeLists.txt` deve definir `CMAKE_CXX_STANDARD 17`.

Impacto na validação:
Nenhum impacto matemático direto.

---

## DEC-0002 — Não começar pelo caso grande

Status: aceita

Contexto:
A tese apresenta resultado com 180.000 elétrons e 180.000 prótons, mas a reconstrução precisa de validação incremental.

Decisão:
Não implementar/rodar o caso grande antes das validações de MLS, Poisson, deposição de carga, interpolação e leap-frog.

Justificativa:
Figuras qualitativas podem mascarar erro matemático.

Impacto no código:
Os primeiros apps devem ser `smoke`, `mls`, `poisson_mms`, `pidc_small`.

Impacto na validação:
Aumenta rastreabilidade e confiabilidade.

---

## DEC-0003 — Matriz densa primeiro, esparsa depois

Status: proposta

Contexto:
Matriz densa facilita depuração inicial, mas não serve para casos grandes.

Decisão proposta:
Implementar Poisson EFG primeiro com matriz densa em casos pequenos. Depois migrar para `Eigen::SparseMatrix`.

Justificativa:
Separar validação matemática de otimização.

Impacto no código:
Criar interface que permita trocar backend de matriz.

Impacto na validação:
Mesmos testes devem rodar nos dois backends.

---

## DEC-0004 — Convenção de unidades

Status: proposta

Contexto:
A tese/publicação usa expressões com `4*pi*rho` em alguns pontos; código moderno pode preferir SI.

Decisão proposta:
Criar um documento `docs/math/UNITS.md` e uma enumeração de sistema de unidades antes da simulação física.

Justificativa:
Evitar erro de escala e sinal.

Impacto no código:
Constantes físicas devem ficar centralizadas.

Impacto na validação:
Poisson MMS deve testar a convenção escolhida.

---

## DEC-0005 — Adoção do Sistema Internacional (SI) de unidades

Status: aceita

Contexto:
A ambiguidade no fator `4π` da equação de Poisson (`∇²u = -ρ/ε₀` vs. `∇²u = -4πρ`) precisa ser resolvida. Esta é a questão `Q-0001`.

Decisão:
Adotar o Sistema Internacional (SI) de unidades como padrão para todas as equações físicas e implementações. A equação de Poisson será `∇²u = -ρ/ε₀`.

Justificativa:
O SI é o padrão na engenharia e em muitos códigos computacionais modernos, tornando a integração com outras ferramentas e a verificação de valores mais diretas. Evita os fatores `4π` que são comuns no sistema CGS Gaussiano.

Impacto no código:
Um arquivo de constantes (`include/pidc/physics/constants.hpp`) deve ser criado para definir `ε₀` e outras constantes do SI. O solver de Poisson usará a forma `∇²u = -ρ/ε₀`.

Impacto na validação:
O teste de Poisson MMS deve ser construído com uma fonte `ρ` consistente com a solução `u` e a forma SI da equação.

---

## DEC-0006 — Formulação variacional (fraca) para Poisson EFG

Status: proposta

Contexto:
Para implementar o solver EFG para a equação de Poisson, é preciso definir a forma fraca (variacional) que leva ao sistema linear `K u = b`.

Decisão:
Usar a formulação de Galerkin padrão. Para a equação `∇²u = -f` (onde `f = ρ/ε₀`), a forma fraca é: `∫ (∇w ⋅ ∇u) dΩ = ∫ w f dΩ`.
Aproximando `u` por `u^h = Σ u_j φ_j` e usando `w = φ_i`, os componentes da matriz de rigidez `K` e do vetor de fontes `b` serão:
`K_ij = ∫ (∇φ_i ⋅ ∇φ_j) dΩ`
`b_i = ∫ φ_i f dΩ`

Justificativa:
Esta é a formulação padrão e mais direta para o método de Galerkin, garantindo simetria da matriz `K` e facilitando a validação.

Impacto no código:
O módulo de montagem do sistema EFG implementará essas integrais numericamente (e.g., via quadratura de Gauss).

Impacto na validação:
O teste de Poisson MMS validará a correta implementação desta formulação.

---

## DEC-0007 — Estratégia para Poisson periódico

Status: proposta

Contexto:
A questão `Q-0002` trata da necessidade de fixar o gauge do potencial em domínios periódicos.

Decisão:
Para a fase inicial de validação, focar em problemas com condições de contorno de Dirichlet, como o caso MMS em `TODO.md`. A implementação do solver periódico será feita em uma fase posterior. Quando for implementado, a estratégia inicial será forçar a média do potencial a ser zero (`Σ u_i = 0`), pois é uma condição mais robusta do que fixar um único nó.

Justificativa:
Adiar a complexidade do solver periódico permite validar o núcleo do EFG em um cenário mais simples (Dirichlet). A escolha de `Σ u_i = 0` é uma prática comum e robusta em métodos espectrais e outros solvers.

Impacto no código:
Inicialmente, o solver de Poisson só precisa lidar com Dirichlet. A implementação futura do solver periódico exigirá uma etapa adicional para modificar o sistema linear `K u = b` para impor a restrição.

Impacto na validação:
O teste MMS com Dirichlet valida `K` e `b`. Um novo teste será necessário para o solver periódico.

---

## DEC-0008 — Remover `applyPeriodic(Particle&)` de `Domain2D`

Status: aceita
Proposta por: Claude — 2026-05-08

Contexto:
`Domain2D::applyPeriodic(Particle&)` cria dependência de uma primitiva geométrica sobre o tipo de partícula (R-004). O método `wrapPeriodic(Vec2) -> Vec2` já existe e é suficiente.

Decisão:
Remover `applyPeriodic(Particle&)` de `Domain2D`. Chamadores devem usar:

```cpp
particle.position = domain.wrapPeriodic(particle.position);
```

Justificativa:
`Domain2D` é geometria pura. Não deve saber da existência de `Particle`. O único acoplamento legítimo é com `Vec2`.

Impacto no código:

- Remover declaração de `applyPeriodic` em `Domain2D.hpp` e implementação em `Domain2D.cpp`.
- Atualizar `test_periodic_boundary.cpp` para usar `wrapPeriodic` diretamente.
- O include de `Particle.hpp` em `Domain2D.hpp` desaparece.

Impacto na validação:
O teste existente de contorno periódico deve ser adaptado e continuar passando.

---

## DEC-0009 — Definir `ShapeFunctionData` como contrato entre MLS, EFG e PIDC

Status: aceita
Proposta por: Claude — 2026-05-08
Implementada por: Claude — 2026-05-08

Contexto:
O módulo MLS produz $\phi_i(\mathbf{x})$ e $\nabla\phi_i(\mathbf{x})$ para um ponto $\mathbf{x}$. Esse resultado é consumido por EFG (montagem de `K` e `b`) e por PIDC (deposição de carga e interpolação de campo). Sem struct definida, os módulos terão representações incompatíveis (R-005).

Decisão:
Criar `include/pidc/mls/ShapeFunctionData.hpp` com a struct:

```cpp
namespace pidc {

struct ShapeFunctionData {
    std::vector<std::size_t> neighbor_ids; // índices globais dos nós vizinhos
    std::vector<double>      phi;          // phi_i(x), i in neighbor_ids
    std::vector<Vec2>        grad_phi;     // grad(phi_i)(x), i in neighbor_ids
};

} // namespace pidc
```

`neighbor_ids`, `phi` e `grad_phi` devem ter sempre o mesmo tamanho. Invariante a ser verificada em debug.

Justificativa:
Essa struct é a fronteira explícita entre o núcleo MLS e todos os seus consumidores. Definida antes de qualquer implementação evita reescritas de interface.

Impacto no código:

- `MLSShapeFunction::evaluate(Vec2, ...)` retorna `ShapeFunctionData`.
- `EFGPoissonSolver` recebe dados já calculados para montar `b` (mas pode pré-calcular para todos os pontos de quadratura ao montar `K`).
- `PIDCChargeDepositor` e `PIDCFieldInterpolator` recebem `ShapeFunctionData` por partícula.

Impacto na validação:
Teste de MLS deve verificar que `phi.size() == neighbor_ids.size() == grad_phi.size()` e que $\sum_i \phi_i = 1$ e $\sum_i \nabla\phi_i = 0$.

---

## DEC-0010 — Interface de `EFGPoissonSolver` com separação explícita entre montagem e solução

Status: proposta
Proposta por: Claude — 2026-05-08

Contexto:
A matriz de rigidez `K` é constante enquanto a nuvem de nós não muda. No PIDC, `K` é montada uma vez e apenas `b` muda a cada passo de tempo. Sem interface que separe `assemble()` de `solve()`, o chamador tende a reconstruir `K` desnecessariamente (R-006).

Decisão:
A interface mínima de `EFGPoissonSolver` deve ser:

```cpp
class EFGPoissonSolver {
public:
    // Chamado uma vez (ou quando a nuvem muda). Monta K e realiza fatoração.
    void assemble(const NodeCloud& nodes, const Domain2D& domain,
                  const DirichletBC& bc);

    // Chamado a cada passo de tempo. Recebe rhs já computado, retorna potencial nodal.
    Eigen::VectorXd solve(const Eigen::VectorXd& rhs) const;

    // Para inspeção e testes.
    const Eigen::MatrixXd& stiffnessMatrix() const;
};
```

Justificativa:
Separar montagem de solução é a única forma de reutilizar a fatoração de `K`. Expor `stiffnessMatrix()` permite verificar simetria, positividade e condicionamento em testes.

Impacto no código:
`EFGPoissonSolver` deve armazenar internamente a matriz e a fatoração (Eigen `LDLT` ou `LLT` para densa, `SimplicialLLT` para esparsa).

Impacto na validação:
Teste MMS deve chamar `assemble()` uma vez e `solve()` com o rhs manufaturado. Um segundo teste deve verificar que chamar `solve()` com rhs diferente dá resultado diferente sem precisar remontar.

---

## DEC-0011 — Introduzir `Species` para evitar charge/mass por instância

Status: aceita
Proposta por: Claude — 2026-05-08
Implementada por: Claude — 2026-05-08

Contexto:
`Particle::charge` e `Particle::mass` são armazenados por instância (R-007). Em uma simulação de plasma, todos os elétrons têm a mesma carga e massa. O armazenamento por instância usa memória desnecessária e permite inconsistência por bug de inicialização.

Decisão:
Criar `include/pidc/particles/Species.hpp`:

```cpp
namespace pidc {

struct Species {
    double charge{0.0};   // carga em Coulombs (SI)
    double mass{1.0};     // massa em kg (SI)
    std::string label{};  // "electron", "proton", etc.
};

} // namespace pidc
```

`Particle` mantém uma referência (índice inteiro) para a espécie na lista global:

```cpp
struct Particle {
    Vec2        position{};
    Vec2        velocity{};
    std::size_t species_id{0};
};
```

A lista de espécies fica em `Simulation` ou `NodeCloud`. Conversão `q/m` fica em `Species`.

Justificativa:
Reduz memória, elimina classe de bug de inicialização e reflete a física correta: espécie é propriedade do tipo, não da instância.

Impacto no código:
Alterar `Particle.hpp`. Criar `Species.hpp`. Atualizar `test_periodic_boundary.cpp` para não passar `charge`/`mass` no construtor.

Impacto na validação:
O teste de contorno periódico não usa `charge`/`mass` — impacto mínimo.

---

## DEC-0012 — `NodeCloud` como proprietário canônico da lista de nós

Status: aceita
Proposta por: Claude — 2026-05-08
Implementada por: Codex — 2026-05-08

Contexto:
`EFGPoissonSolver`, `MLSShapeFunction`, `PIDCChargeDepositor` e `PIDCFieldInterpolator` todos precisam da lista de nós (R-008). Sem uma entidade proprietária clara, surgem cópias ou ponteiros com tempo de vida ambíguo.

Decisão:
Criar `include/pidc/geometry/NodeCloud.hpp` como proprietário da lista de `Node`:

```cpp
namespace pidc {

class NodeCloud {
public:
    explicit NodeCloud(std::vector<Node> nodes);

    std::size_t         size() const noexcept;
    const Node&         operator[](std::size_t i) const;
    const std::vector<Node>& nodes() const noexcept;

private:
    std::vector<Node> nodes_;
};

} // namespace pidc
```

Todos os outros módulos recebem `const NodeCloud&` — nunca uma cópia.

Justificativa:
Propriedade clara evita cópias acidentais e define tempo de vida. `NodeCloud` pode futuramente conter a busca de vizinhança (`NeighborSearchGrid`) de forma coesa.

Impacto no código:
`Node.hpp` permanece como está. `NodeCloud` é o agregador. `EFGPoissonSolver::assemble` e `MLSShapeFunction::evaluate` recebem `const NodeCloud&`.

Impacto na validação:
Testes de MLS e EFG devem criar um `NodeCloud` explícito — isso é bom para legibilidade dos testes.

---

## DEC-0013 — Função peso compacta: spline quártica

Status: aceita

Contexto:
O MLS requer uma função peso compacta $w(r) \geq 0$ definida no raio normalizado
$r = \|\mathbf{x} - \mathbf{x}_i\| / r_{\max}$, com $w(r) = 0$ para $r \geq 1$.

Decisão:
Adotar a **spline quártica**:

$$w(r) = \begin{cases} 1 - 6r^2 + 8r^3 - 3r^4 & r \leq 1 \\ 0 & r > 1 \end{cases}$$

Derivada: $w'(r) = -12r(1-r)^2$ para $r \leq 1$, $0$ caso contrário.

Implementada como funções livres `weight_quartic(r)` e `weight_quartic_deriv(r)`
em `include/pidc/mls/WeightFunction.hpp`.

Justificativa:
Função padrão em métodos EFG/MEFG (Belytschko et al. 1994). Compacta, $C^1$
contínua, positiva em $[0,1]$, monotonicamente decrescente. Pesos positivos
garantem que a matriz de momento $A$ seja positiva definida quando há nós
suficientes.

Impacto no código:
`MLSShapeFunction::evaluate` (futura) usará `weight_quartic` para ponderar
cada vizinho.

Impacto na validação:
Teste `weight_function` verifica propriedades matemáticas exatas. O teste de
partição da unidade (futuro) validará indiretamente a função peso no contexto
MLS completo.

---

## DEC-0014 — Base polinomial: linear 2D, m=3

Status: aceita
Proposta por: Claude — 2026-05-08

Contexto:
O MLS requer uma base polinomial completa de grau $k$. Para $k=1$ (linear) em
2D, $m=3$ e $\mathbf{p}(\mathbf{x}) = [1, x, y]^T$.

Decisão:
Adotar a base linear como padrão inicial. Implementada como funções livres
`linear_basis(Vec2)`, `linear_basis_dx()`, `linear_basis_dy()` em
`include/pidc/mls/PolynomialBasis.hpp`. Interface: `std::array<double, 3>`.

Justificativa:
Base linear é suficiente para reprodução linear exata e é o caso descrito
na tese de Marques. Base quadrática ($m=6$) pode ser adicionada futuramente
se os testes de reprodução revelarem necessidade.

Impacto no código:
`MLSShapeFunction::evaluate` (futura) usa `linear_basis` para montar $A$
e avaliar $\phi_i(\mathbf{x}) = \mathbf{p}^T(\mathbf{x}) A^{-1} \mathbf{b}_i$.

Impacto na validação:
Teste de reprodução linear ($u = a + bx + cy$) exige base linear completa.
Teste de reprodução constante ($u = c$) é subconjunto e passará automaticamente.

---

## DEC-0015 — Dependência Eigen via alvo CMake importado

Status: aceita
Proposta por: Gemini — 2026-05-08
Implementada por: Codex — 2026-05-08

Contexto:
As próximas etapas de MLS/EFG e Poisson dependem de álgebra linear densa e, depois, esparsa. O projeto já usa CMake e C++17.

Decisão:
Adicionar Eigen ao CMake com `find_package(Eigen3 3.3 REQUIRED NO_MODULE)` e propagar `Eigen3::Eigen` como dependência pública de `pidc_core`.

Justificativa:
Eigen é header-only, amplamente disponível e adequada para as primeiras validações densas antes da migração esparsa.

Impacto no código:
Alvos consumidores de `pidc_core` herdam os includes de Eigen sem configuração adicional.

Impacto na validação:
O teste `eigen_dependency` confirma que um consumidor de `pidc_core` consegue incluir `<Eigen/Dense>` e executar uma solução linear pequena.

---

## DEC-0016 — Interface de MLSShapeFunction: função livre `mls_evaluate`

Status: aceita

Data: 2026-05-08
Responsável: Claude (T-017)

Contexto:
Com WeightFunction, PolynomialBasis e Eigen disponíveis, é necessário definir a
interface pública da avaliação MLS antes de implementar a montagem EFG.

Decisão:
Implementar como função livre header-only
`pidc::mls_evaluate(Vec2 x, const NodeCloud& cloud, double support_radius) -> ShapeFunctionData`
em `include/pidc/mls/MLSShapeFunction.hpp`.

Justificativa:

- Função livre evita estado interno desnecessário neste estágio.
- Retornar `ShapeFunctionData` respeita o contrato DEC-0009.
- `PartialPivLU` (3×3) é reusada para os três RHS (c, dc_dx, dc_dy),
  minimizando fatorações de matriz.
- Busca linear sobre todos os nós é suficiente para validação inicial;
  estrutura espacial é trabalho futuro.

Impacto no código:
`include/pidc/mls/MLSShapeFunction.hpp` criado.
`tests/test_mls_shape_function.cpp` cobre partição da unidade, reprodução
linear, gradientes e exceção com vizinhos insuficientes.

Impacto na validação:
Partição da unidade ($\sum\phi_i = 1$) e reprodução linear
($\sum\phi_i\mathbf{x}_i = \mathbf{x}$) e seus gradientes validados com
tolerância $10^{-10}$ em nuvem regular 5×5. 8/8 testes passando.
