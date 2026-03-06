#pragma once

#include "core/rule.hpp"
#include "core/rule_registry.hpp"

constexpr std::string RENEW_ORIGIN_RULE_NAME = "Renew Origin Rule";

class RenewOriginRule: public Rule {
public:
  RenewOriginRule() = default;
  ~RenewOriginRule() override = default;
  static RenewOriginRule& getInstance() {
    static RenewOriginRule instance;
    return instance;
  }
  
  uint8_t apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const override;
  uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const override;
  std::string getName() const override;

  inline static AutoRegisterRule<RenewOriginRule> auto_register{RENEW_ORIGIN_RULE_NAME, "Rule that renews cells marked as origin seeds"};
};
