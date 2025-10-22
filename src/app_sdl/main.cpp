#include "core/rules_conway.hpp"
#include "core/engine.hpp"
#include "renderer.hpp"
#include "convex_hull/convex_hull.hpp"

Rule& chooseRule(bool use_conway) {
  static ConwayRule conway_rule;
  static ConvexHull convex_hull_rule;
  if (use_conway) {
    return conway_rule;
  } else {
    return convex_hull_rule;
  }
}

int main(int argc, char* argv[])
{
  bool use_conway = true;
  if (argc > 1) {
    std::string arg_rule = argv[1];
    if (arg_rule == "convex_hull") {
      use_conway = false;
    }
  }
  Rule& rule = chooseRule(use_conway);
  Engine e(50, 30, rule);
  if (!use_conway) {
    e.setDistanceCalculator(ConvexHull::calculateDistances);
    e.setCalculatingDistances(true);
  }
   Renderer r(e, 20);
  
  while (r.render()) {
    // loop until window closed
  }
  return 0;
}

