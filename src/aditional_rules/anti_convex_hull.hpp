#pragma once

#include "core/rule.hpp"
#include "core/rule_registry.hpp"

constexpr uint8_t OUTSIDE_CELL_VALUE = 0x20;
constexpr uint8_t INSIDE_CELL_FLAG = 0x10; 
constexpr uint8_t INSIDE_CELL_VALUE = 0x01 | INSIDE_CELL_FLAG;

constexpr std::string ANTI_CONVEX_HULL_RULE_NAME = "Anti-Convex Hull Rule";

inline bool is_inside(uint8_t s) {
  return (s & INSIDE_CELL_FLAG) != 0;
}

inline uint8_t make_inside(uint8_t s) {
  return s | INSIDE_CELL_FLAG;
}

class AntiConvexHull : public Rule {
public:
  AntiConvexHull() = default;
  ~AntiConvexHull() override = default;
  
  static AntiConvexHull& getInstance() {
    static AntiConvexHull instance;
    return instance;
  }

  uint8_t apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const override;
  uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const override;
  std::string getName() const override;

  inline static AutoRegisterRule<AntiConvexHull> auto_register{ANTI_CONVEX_HULL_RULE_NAME, ""};
};
