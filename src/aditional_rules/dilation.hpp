#pragma once

#include "core/rule.hpp"
#include "core/rule_registry.hpp"

constexpr std::string DILATION_RULE_NAME = "Dilation";

class DilationRule: public Rule {
public:
  
  DilationRule() = default;
  ~DilationRule() override = default;

  uint8_t apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const override;
  uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const override;
  std::string getName() const override;

  inline static AutoRegisterRule<DilationRule> auto_register_dilation{DILATION_RULE_NAME, "Dilation rule that adds cells around existing ones."};
};
