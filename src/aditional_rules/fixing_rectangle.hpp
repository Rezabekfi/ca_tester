#pragma once

#include "core/rule.hpp"
#include "core/rule_registry.hpp"
#include "dilation.hpp"
#include "erosion.hpp"

constexpr std::string FIXING_RECTANGLE_RULE_NAME = "Fixing Rectangle";


class FixingRectangleRule: public Rule {
public:
  FixingRectangleRule() = default;
  ~FixingRectangleRule() override = default;

  static FixingRectangleRule& getInstance() {
    static FixingRectangleRule instance;
    return instance;
  }

  uint8_t apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const override;
  uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const override;
  std::string getName() const override;

  inline static AutoRegisterRule<FixingRectangleRule> auto_register{FIXING_RECTANGLE_RULE_NAME, "Rule that uses dilation and erosion in succesion to hopefully fix holes and irregularities in rectangles."};
private:
  inline static bool dilation_phase_ = true;
};
