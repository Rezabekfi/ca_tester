#include "core/rules_conway.hpp"
#include "core/engine.hpp"
#include "renderer.hpp"
#include "convex_hull/convex_hull.hpp"
#include "aditional_rules/dilation.hpp"
#include "aditional_rules/erosion.hpp"

int main(int argc, char* argv[]) {
  ConwayRule conway_rule;
  Engine e(50, 30, conway_rule.getName());
  // e.setCalculatingDistances(true);
  // e.setDistanceCalculator(ConvexHull::calculateDistances);
  Renderer r(e, 20);
  while (r.render()) {
    // loop until window closed
  }
  return 0;
}

