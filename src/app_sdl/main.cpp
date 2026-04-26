#include "core/rules_conway.hpp"
#include "core/engine.hpp"
#include "renderer.hpp"
#include "convex_hull/convex_hull.hpp"

// Important:
#include "aditional_rules/all_aditional_rules.hpp" // include all additional rules to ensure they are registered in the registry and available in the UI

// Main just boots the app and runs the render loop
int main(int argc, char* argv[]) {

  // Instantiating ensures rule is linked + registered (via static AutoRegisterRule)
  ConwayRule conway_rule;

  // Engine uses rule name to fetch instance from registry
  // This might be later changed to not contain magic constants but for now it is good enough
  Engine e(50, 30, conway_rule.getName());

  // Renderer drives UI + indirectly drives stepping via Engine
  // Cell size of 20 is a good default for 50x30 grid, but this can be adjusted as needed
  Renderer r(e, 20);

  // Main loop is fully owned by renderer (SDL loop inside)
  while (r.render()) {
    // loop until window closed
  }

  return 0;
}
