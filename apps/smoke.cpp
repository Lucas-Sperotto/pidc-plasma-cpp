#include <iostream>

#include "pidc/Domain2D.hpp"
#include "pidc/Node.hpp"
#include "pidc/Particle.hpp"

int main()
{
    const pidc::Domain2D domain{{0.0, 0.0}, {1.0, 1.0}};
    pidc::Particle particle{{1.25, -0.25}, {0.0, 0.0}, 0};
    const pidc::Node node{0, {0.5, 0.5}, 1.0};

    particle.position = domain.wrapPeriodic(particle.position);

    std::cout << "pidc smoke\n";
    std::cout << "domain: " << domain.width() << " x " << domain.height() << "\n";
    std::cout << "particle: (" << particle.position.x << ", " << particle.position.y << ")\n";
    std::cout << "node " << node.id << ": (" << node.position.x << ", " << node.position.y << ")\n";

    return domain.contains(particle.position) ? 0 : 1;
}
