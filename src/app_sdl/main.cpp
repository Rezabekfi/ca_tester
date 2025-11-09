#include "core/rules_conway.hpp"
#include "core/engine.hpp"
#include "renderer.hpp"
#include "convex_hull/convex_hull.hpp"
#include "aditional_rules/dilation.hpp"
#include "aditional_rules/erosion.hpp"

// TODO: very very very ugly code use factory pattern (most likely singleton which holds all rules and can return them by name or id)
Rule& chooseRule(bool use_conway) {
  static ConwayRule conway_rule;
  static ConvexHull convex_hull_rule;
  static DilationRule dilation_rule;
  static ErosionRule erosion_rule;
  if (use_conway) {
    return dilation_rule;
  } else {
    return convex_hull_rule;
  }
}

std::vector<Rule*> all_available_rules() {
  static ConwayRule conway_rule;
  static ConvexHull convex_hull_rule;
  static DilationRule dilation_rule;
  static ErosionRule erosion_rule;
  return {&conway_rule, &convex_hull_rule, &dilation_rule, &erosion_rule};
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
  std::vector<Rule*> rules = all_available_rules();
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

