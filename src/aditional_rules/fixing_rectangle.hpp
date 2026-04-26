#pragma once

#include "core/rule.hpp"
#include "core/rule_registry.hpp"
#include "dilation.hpp"
#include "erosion.hpp"


// WARNING: Do not use this rule! Might no longer work and is not tested after parallelization changes. 

constexpr std::string FIXING_RECTANGLE_RULE_NAME = "Fixing Rectangle";

// Combines two rules (dilation + erosion) in phases
// Idea: dilation fills gaps, erosion trims excess → together smooth shapes
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

  // Registered for UI
  inline static AutoRegisterRule<FixingRectangleRule> auto_register{
    FIXING_RECTANGLE_RULE_NAME,
    "Rule that uses dilation and erosion in succesion to hopefully fix holes and irregularities in rectangles."
  };

private:
  // Shared phase flag → all cells use same phase each step
  // (important: this makes rule globally stateful, not purely local)
  inline static bool dilation_phase_ = true;
};
