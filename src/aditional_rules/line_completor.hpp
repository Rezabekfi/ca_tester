#pragma once

#include "core/rule.hpp"
#include "core/rule_registry.hpp"

constexpr std::string LINE_COMPLETOR_RULE_NAME = "LineCompletor";

// Tries to fill gaps in lines by looking at a larger neighborhood radius
class LineCompletorRule: public Rule {
public:
  static LineCompletorRule& getInstance() {
    static LineCompletorRule instance;
    return instance;
  }
  
  LineCompletorRule() = default;

  // Allows creating rule with custom radius (not used by registry path)
  LineCompletorRule(std::size_t line_radius) : line_radius_(line_radius) {}

  ~LineCompletorRule() override = default;

  // not used
  uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const override;

  // Main version uses ctx to sample wider area (radius-based logic)
  uint8_t apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const override;

  std::string getName() const override;

  // Allows changing radius even on const instance (used from UI)
  void setLineRadius(std::size_t line_radius) const;

  // Auto-register for UI selection
  inline static AutoRegisterRule<LineCompletorRule> auto_register_line_completor{
    LINE_COMPLETOR_RULE_NAME,
    "Rule that completes lines of a certain length."
  };

private:
  // Mutable so it can be modified via const Rule reference
  // (engine stores rules as const interface)
  mutable std::size_t line_radius_ = 5;
};
