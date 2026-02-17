#pragma once

#include "core/rule.hpp"
#include "core/rule_registry.hpp"
#include <array>

constexpr std::string CLEARING_RULE_NAME = "Clearing Rule";

// configurations for Moore
// configurations are ordered in the same way as deltas_moore
// TODO: I did a mistake here all of the configs are wrong by one index because it is from top middle clockwise but I ordered them from top left clockwise
// WARNING: The rule doesnt work unless the configs are fixed
constexpr std::array<std::array<uint8_t, 8>, 14> moore_clearing_configs = {{
  // lines 
  {0,1,0,0,0,1,0,0},
  {0,0,0,1,0,0,0,1},
  // corners
  {0,0,0,1,0,1,0,0},
  {0,1,0,1,0,0,0,0},
  {0,1,0,0,0,0,0,1},
  {0,0,0,0,0,1,0,1},
  // corners but a pixel to one side (meaning this is the only one with 4 points including current)
  {0,1,1,0,0,1,0,0},
  {0,1,0,0,1,1,0,0},
  {1,0,0,1,0,0,0,1},
  {0,0,1,1,0,0,0,1},
  {0,1,0,0,0,1,1,0},
  {1,1,0,0,0,1,0,0},
  {0,0,0,1,1,0,0,1},
  {0,0,0,1,0,0,1,1}
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
