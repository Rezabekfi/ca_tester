#pragma once

#include "all_aditional_rules.hpp"

constexpr std::string FIX_ROTATE_FIX_RULE_NAME = "Fix Rotate Fix";

class FixRotateFix : public Rule {
public:
  FixRotateFix() = default;
  ~FixRotateFix() override = default;

  uint8_t apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const override;
  uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const override;
  std::string getName() const override;

  inline static AutoRegisterRule<FixRotateFix> auto_register{FIX_ROTATE_FIX_RULE_NAME, "A rule that first applies fixing rectangle, then rotation, then fixing rectangle again."};

private:
  inline static uint8_t state = 0; // 0,1: fixing, 2: rotating, 3,4: fixing
  inline static uint8_t step(uint8_t state) {
    return (state + 1) % 5;
  }
};


