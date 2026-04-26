#pragma once

#include "core/rule.hpp"
#include "core/rule_registry.hpp"

constexpr std::string RENEW_ORIGIN_RULE_NAME = "Renew Origin Rule";

// Helper rule used after convex hull steps
// Revives cells that were marked as original seeds
class RenewOriginRule: public Rule {
public:
  RenewOriginRule() = default;
  ~RenewOriginRule() override = default;

  static RenewOriginRule& getInstance() {
    static RenewOriginRule instance;
    return instance;
  }
  
  // not used I left it here just in case I would need to add some more to this rule
  uint8_t apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const override;

  // Context-free version just checks origin flag and revives if set (used)
  uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const override;

  std::string getName() const override;

  // Auto-registers for use in rule pipelines / UI
  inline static AutoRegisterRule<RenewOriginRule> auto_register{
    RENEW_ORIGIN_RULE_NAME,
    "Rule that renews cells marked as origin seeds"
  };
};
