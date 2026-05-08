#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "pidc/Particle.hpp"
#include "pidc/Species.hpp"

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
    // Caso 1: defaults
    {
        const pidc::Species s{};
        require(s.charge == 0.0, "default Species charge must be 0.0");
        require(s.mass == 1.0, "default Species mass must be 1.0");
        require(s.label.empty(), "default Species label must be empty");
    }

    // Caso 2: valores físicos realistas
    {
        const pidc::Species electron{-1.602e-19, 9.109e-31, "electron"};
        require(electron.charge < 0.0, "electron charge must be negative");
        require(electron.mass > 0.0, "electron mass must be positive");
        require(electron.label == "electron", "electron label must match");

        const pidc::Species proton{1.602e-19, 1.672e-27, "proton"};
        require(proton.charge > 0.0, "proton charge must be positive");
        require(proton.mass > electron.mass, "proton must be heavier than electron");
    }

    // Caso 3: Particle referencia espécie por índice
    {
        const std::vector<pidc::Species> species_list{
            {-1.602e-19, 9.109e-31, "electron"},
            { 1.602e-19, 1.672e-27, "proton"},
        };

        pidc::Particle p{};
        p.species_id = 0;
        require(species_list[p.species_id].charge < 0.0,
                "species_id=0 must reference negative charge");

        p.species_id = 1;
        require(species_list[p.species_id].charge > 0.0,
                "species_id=1 must reference positive charge");
    }

    std::cout << "species test passed\n";
    return 0;
}
