#pragma once

#include "core/rule.hpp"
#include "core/rule_registry.hpp"
#include <array>

constexpr std::string CLEARING_RULE_NAME = "Clearing Rule";

// J = JOKER
constexpr uint8_t J = 2; // value used in configurations to indicate that the state of the cell does not matter

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
  // corners but a pixel to one side (meaning this is the only one with 4 points including current)
  {0,0,1,1,J,J,1,0},
  {0,0,1,J,J,1,1,0},
  {1,1,J,J,1,0,0,0},
  {1,J,J,1,1,0,0,0},
  {J,J,1,0,0,0,1,1},
  {J,1,1,0,0,0,1,J},
  {1,0,0,0,1,1,J,J},
  {1,0,0,0,1,J,J,1}
}};


constexpr std::array<std::array<uint8_t, 8>, 4> moore_only_corners_clearing_configs = {{
  {0,0,0,0,1,J,1,0},
  {0,0,1,J,1,0,0,0},
  {1,J,1,0,0,0,0,0},
  {1,0,0,0,0,0,1,J}
}};


class ClearingRule: public Rule {
public:
  static ClearingRule& getInstance() {
    static ClearingRule instance;
    return instance;
  }
  
  ClearingRule() = default;
  ~ClearingRule() override = default;

  uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const override;
  uint8_t apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const override;
  std::string getName() const override;

  inline static AutoRegisterRule<ClearingRule> auto_register_clearing{CLEARING_RULE_NAME, "Rule that kills any cell that is not part of a line or corner"};
};
