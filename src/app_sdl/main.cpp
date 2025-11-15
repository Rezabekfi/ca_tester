#include "core/rules_conway.hpp"
#include "core/engine.hpp"
#include "renderer.hpp"
#include "convex_hull/convex_hull.hpp"
#include "aditional_rules/all_aditional_rules.hpp"

int main(int argc, char* argv[]) {
  ConwayRule conway_rule;
  Engine e(50, 30, conway_rule.getName());
  Renderer r(e, 20);
  while (r.render()) {
    // loop until window closed
  }
  return 0;
}

