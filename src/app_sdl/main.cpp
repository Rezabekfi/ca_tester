#include "core/rules_conway.hpp"
#include "core/engine.hpp"
#include "renderer.hpp"
#include "convex_hull/convex_hull.hpp"
#include "aditional_rules/dilation.hpp"
#include "aditional_rules/erosion.hpp"

int main(int argc, char* argv[]) {
  Engine e(50, 30, std::make_unique<ConwayRule>());

  Renderer r(e, 20);
  
  while (r.render()) {
    // loop until window closed
  }
  return 0;
}

