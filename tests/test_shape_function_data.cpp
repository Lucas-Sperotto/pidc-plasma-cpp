#include <cstdlib>
#include <iostream>
#include <string>

#include "pidc/ShapeFunctionData.hpp"

namespace {

void require(bool condition, const std::string& message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(1);
    }
}

} // namespace

int main()
{
    // Caso 1: struct vazia e válida (0 == 0 == 0).
    {
        const pidc::ShapeFunctionData empty{};
        require(pidc::is_valid(empty), "empty ShapeFunctionData must be valid");
    }

    // Caso 2: tamanhos consistentes (n=3).
    {
        pidc::ShapeFunctionData d;
        d.neighbor_ids = {0, 1, 2};
        d.phi          = {0.5, 0.3, 0.2};
        d.grad_phi     = {{1.0, 0.0}, {0.0, 1.0}, {-1.0, -1.0}};
        require(pidc::is_valid(d), "consistent size-3 ShapeFunctionData must be valid");
    }

    // Caso 3: neighbor_ids maior que phi → inválido.
    {
        pidc::ShapeFunctionData d;
        d.neighbor_ids = {0, 1, 2};
        d.phi          = {0.5, 0.5};
        d.grad_phi     = {{1.0, 0.0}, {-1.0, 0.0}};
        require(!pidc::is_valid(d), "neighbor_ids.size() != phi.size() must be invalid");
    }

    // Caso 4: grad_phi menor que phi → inválido.
    {
        pidc::ShapeFunctionData d;
        d.neighbor_ids = {0, 1};
        d.phi          = {0.5, 0.5};
        d.grad_phi     = {{1.0, 0.0}};
        require(!pidc::is_valid(d), "phi.size() != grad_phi.size() must be invalid");
    }

    std::cout << "shape_function_data test passed\n";
    return 0;
}
