#pragma once

#include "core/rule.hpp"
#include "core/rule_registry.hpp"

// Extra cell states used by anti-hull logic
constexpr uint8_t OUTSIDE_CELL_VALUE = 0x20;
constexpr uint8_t INSIDE_CELL_FLAG = 0x10; 
constexpr uint8_t INSIDE_CELL_VALUE = 0x01 | INSIDE_CELL_FLAG;

constexpr std::string ANTI_CONVEX_HULL_RULE_NAME = "Anti-Convex Hull Rule";

// Checks the inside layer, separate from regular alive/seed bit
inline bool is_inside(uint8_t s) {
  return (s & INSIDE_CELL_FLAG) != 0;
}

// Marks a cell as inside while preserving other bits
inline uint8_t make_inside(uint8_t s) {
  return s | INSIDE_CELL_FLAG;
}

// Rule for marking cells that are not alive as outside and rest inside
class AntiConvexHull : public Rule {
public:
  AntiConvexHull() = default;
  ~AntiConvexHull() override = default;
  
  // Singleton helper for places that want a shared rule instance
  static AntiConvexHull& getInstance() {
    static AntiConvexHull instance;
    return instance;
  }
  
  // This is not used it is basically legacy code because it was needed earlier in the dev
  uint8_t apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const override;

  // Context-free version is used for this rule
  uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const override;

  std::string getName() const override;

  // Auto-registers so the UI can list/create this rule
  inline static AutoRegisterRule<AntiConvexHull> auto_register{ANTI_CONVEX_HULL_RULE_NAME, ""};
};
