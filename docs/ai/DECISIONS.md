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

---

## DEC-0017 — Framework de testes inicial

Status: aceita
Proposta por: Codex — 2026-05-08
Implementada por: Codex — 2026-05-08

Contexto:
O projeto já possui vários testes executáveis simples registrados no CTest.
Ainda não há necessidade concreta de fixtures complexas, parametrização ou
relatórios avançados que justifiquem uma dependência externa.

Decisão:
Usar CTest com executáveis C++ simples como framework inicial. Centralizar
asserções mínimas em `tests/test_utils.hpp`, com `pidc::test::require(...)` e
`pidc::test::approx_equal(...)`. Adiar Catch2/doctest até a suíte exigir
recursos que o helper simples não ofereça.

Justificativa:
Mantém o bootstrap leve, transparente e suficiente para validar os blocos
numéricos iniciais. Também evita introduzir uma dependência de teste antes de
consolidar as primeiras interfaces matemáticas.

Impacto no código:
Testes novos devem incluir `tests/test_utils.hpp` e ser registrados com
`add_test(...)` no CMake.

Impacto na validação:
A suíte atual continua passando com 8/8 testes, agora usando helpers comuns
para falhas e comparações numéricas.

---

## DEC-0018 — Estratégia de quadratura para EFG: células retangulares com Gauss 2×2

Status: aceita
Proposta por: Claude — 2026-05-08 (T-019)
Aceita por: Claude — 2026-05-08 (auditoria Fases 0/A/B/C, após T-022 confirmar T-Poisson desbloqueada). Desbloqueia T-Poisson (Codex).

Contexto:
A formulação de Galerkin (DEC-0006) exige integração numérica de
$K_{ij} = \int_\Omega \nabla\phi_i \cdot \nabla\phi_j\,d\Omega$.
Para o caso de validação MMS com domínio $[0,1]^2$ e nuvem regular $N \times N$,
é necessário decidir a estrutura de integração antes de implementar o assembler EFG (R-012).

Decisão:
Usar células de integração retangulares de background, independentes da nuvem de nós.
Para uma nuvem $N \times N$ com passo $h_g = 1/(N-1)$, criar $(N-1)^2$ células
retangulares de lado $h_g$. Aplicar quadratura de Gauss 2×2 em cada célula (4 pontos,
precisão exata para polinômios de grau ≤ 3).

Justificativa:

- Células de background são a abordagem padrão em EFG (Liu & Gu, 2005).
- Desacopla a malha de integração da nuvem de nós: a quadratura não depende de
  `Node::volume`, eliminando a ambiguidade de R-009.
- Gauss 2×2 é suficiente para integrandos que envolvem produtos de gradientes de
  funções MLS de base linear (grau efetivo do integrando ≈ 2).
- A mesma estrutura de célula pode ser reutilizada por PIDC (deposição de carga).

Impacto no código:

- Criar `include/pidc/efg/GaussCell2D.hpp`: struct com `origin`, `size`, e vetor de
  `{point, weight}` gerado por `gauss2x2_cells(domain, n_cells_x, n_cells_y)`.
- `EFGPoissonSolver` recebe `std::vector<GaussCell2D>` e `const NodeCloud&`.
- `Node::volume` pode ser removido ou mantido como campo auxiliar sem uso em quadratura.

Impacto na validação:
O teste MMS com $u(x,y) = \sin(\pi x)\sin(\pi y)$ deve convergir quando $h_g \to 0$,
validando a quadratura e o assembler simultaneamente.

---

## DEC-0019 — Semântica de Node::volume para quadratura EFG

Status: proposta
Proposta por: Claude — 2026-05-08 (auditoria Fases 0/A/B/C)

Contexto:
R-009 e R-012. `Node::volume` existe mas sua semântica nunca foi documentada.
DEC-0018 (aceita) adota células de integração explícitas (`GaussCell2D`);
portanto `Node::volume` não é necessário para montar K.

Decisão proposta:
Setar `Node::volume = dx * dy` para nuvens regulares (área da célula de
background). Documentar explicitamente que o campo é **informativo** e NÃO
usado em nenhum cálculo de K nem de deposição de carga. Se Gemini/Professor
confirmarem que não há uso planejado, remover o campo de `Node` completamente.

Justificativa:
Manter um campo sem semântica definida é fonte de bug silencioso (R-009).
A decisão de remoção ou documentação deve vir antes de T-Poisson para que
o assembler não introduza usos ad hoc de `Node::volume`.

Impacto no código:

- `include/pidc/Node.hpp`: documentar ou remover `volume`.
- `include/pidc/geometry/RegularNodeCloud2D.hpp`: preencher `volume = dx*dy`
  se campo for mantido.

Impacto na validação:
Nenhum impacto nos testes existentes. Clarifica contrato de `Node` antes
de T-Poisson.

Responsável pela decisão: Professor + Gemini (T-023).

---

## DEC-0020 — `NeighborSearchGrid` v1 não periódico

Status: aceita
Proposta por: Codex — 2026-05-08
Implementada por: Codex — 2026-05-08 (T-025)

Contexto:
A avaliação MLS atual ainda faz busca linear sobre todos os nós. Antes de
integrar uma otimização ao MLS/EFG/PIDC, é necessário validar uma estrutura
espacial pequena e determinística para a Fase B.

Decisão:
Criar `NeighborSearchGrid` em `include/pidc/geometry/NeighborSearchGrid.hpp`
como grade 2D não periódica. A API inicial é:

```cpp
NeighborSearchGrid(const NodeCloud& cloud, const Domain2D& domain, double cell_size);
std::vector<std::size_t> query_radius(Vec2 point, double radius) const;
```

O construtor valida `cell_size` positivo e finito. `query_radius` valida raio
não negativo e finito. A resposta é filtrada por distância euclidiana exata e
ordenada por índice global de nó.

Justificativa:
A busca por grade reduz custo assintótico em etapas futuras, mas a versão v1
permanece isolada e testável. A ordenação evita dependência de ordem aleatória
nos testes e consumidores.

Impacto no código:
`NeighborSearchGrid` guarda ponteiro para `NodeCloud`; portanto, a nuvem deve
viver mais que a grade. Nós na borda superior do domínio são aceitos e
indexados por clamp na última célula. A estrutura ainda não é usada por
`mls_evaluate`.

Impacto na validação:
`tests/test_neighbor_search_grid.cpp` compara a busca da grade contra busca
bruta em pontos internos, de borda e externos. Também valida entradas inválidas.

---

## DEC-0021 — `PeriodicBoundary2D` como helper geométrico

Status: aceita
Proposta por: Codex — 2026-05-08
Implementada por: Codex — 2026-05-08 (T-026)

Contexto:
`Domain2D` já sabe embrulhar posições periódicas, mas a menor imagem periódica
será necessária para distâncias e deslocamentos sem acoplar geometria a
partículas, MLS ou PIDC.

Decisão:
Criar `PeriodicBoundary2D` em `include/pidc/geometry/PeriodicBoundary2D.hpp`
como helper geométrico separado. A API inicial é:

```cpp
explicit PeriodicBoundary2D(Domain2D domain);
Vec2 wrap(Vec2 point) const noexcept;
Vec2 minimum_image(Vec2 displacement) const noexcept;
```

`wrap` delega para `Domain2D::wrapPeriodic`. `minimum_image` reduz cada
componente para a menor imagem e preserva empates exatos em `+/-L/2`.

Justificativa:
Mantém `Domain2D` simples e evita reacoplar a geometria a `Particle`. A menor
imagem fica disponível para futura busca periódica e cálculo de distâncias.

Impacto no código:
Nenhum consumidor existente foi alterado. `NeighborSearchGrid` continua não
periódico nesta versão.

Impacto na validação:
`tests/test_periodic_boundary2d.cpp` cobre múltiplas voltas, deslocamentos
positivos/negativos e empates de meia-caixa.
