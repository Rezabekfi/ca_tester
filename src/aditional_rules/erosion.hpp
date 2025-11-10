#pragma once

#include "core/rule.hpp"
#include "core/rule_registry.hpp"

constexpr std::string EROSION_RULE_NAME = "Erosion";

class ErosionRule : public Rule {
public:
  
  ErosionRule() = default;
  ~ErosionRule() override = default;

  uint8_t apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const override;
  uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const override;
  std::string getName() const override;

  inline static AutoRegisterRule<ErosionRule> auto_register{EROSION_RULE_NAME, "Erosion rule that removes isolated cells."};
};
