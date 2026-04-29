#pragma once

#include "core/rule.hpp"
#include "core/rule_registry.hpp"
#include <array>

inline constexpr const char* SHAPE_ENFORCEMENT_RULE = "Shape Enforcement";

// J = wildcard means "don't care" in pattern matching
constexpr uint8_t J = 2;

// Predefined Moore neighborhood patterns that are considered valid
// Used to preserve only structured shapes (lines/corners) of a rectangle
constexpr std::array<std::array<uint8_t, 8>, 16> moore_clearing_configs = {{
  // lines 
  {J,J,1,0,0,0,1,J},
  {0,0,1,J,J,J,1,0},
  {1,J,J,J,1,0,0,0},
  {1,0,0,0,1,J,J,J},

  // corners
  {0,0,0,0,1,J,1,0},
  {0,0,1,J,1,0,0,0},
  {1,J,1,0,0,0,0,0},
  {1,0,0,0,0,0,1,J},

  // shifted corner variants (include current cell indirectly)
  {0,0,1,1,J,J,1,0},
  {0,0,1,J,J,1,1,0},
  {1,1,J,J,1,0,0,0},
  {1,J,J,1,1,0,0,0},
  {J,J,1,0,0,0,1,1},
  {J,1,1,0,0,0,1,J},
  {1,0,0,0,1,1,J,J},
  {1,0,0,0,1,J,J,1}
}};

// Smaller subset: only strict corner patterns
// This is no longer used kept for future experimentation
constexpr std::array<std::array<uint8_t, 8>, 4> moore_only_corners_clearing_configs = {{
  {0,0,0,0,1,J,1,0},
  {0,0,1,J,1,0,0,0},
  {1,J,1,0,0,0,0,0},
  {1,0,0,0,0,0,1,J}
}};

// Rule that removes "invalid" cells based on neighborhood patterns currently the pattern is tailored to preserve only shapes that could represent rectangle
class ShapeEnforcement: public Rule {
public:
  static ShapeEnforcement& getInstance() {
    static ShapeEnforcement instance;
    return instance;
  }
  
  ShapeEnforcement() = default;
  ~ShapeEnforcement() override = default;

  // basic version not used
  uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const override;

  // used version with context for neighborhood pattern matching
  uint8_t apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const override;

  std::string getName() const override;

  // Auto-register for UI
  inline static AutoRegisterRule<ShapeEnforcement> auto_register{
    SHAPE_ENFORCEMENT_RULE,
    "Rule that removes cells that don't fit specific neighborhood patterns, enforcing structured shapes."
  };
};
