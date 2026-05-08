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

Status: proposta

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
