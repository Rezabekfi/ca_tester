#pragma once

#include "core/rule.hpp"
#include "core/rule_registry.hpp"

constexpr std::string LINE_COMPLETOR_RULE_NAME = "LineCompletor";

class LineCompletorRule: public Rule {
public:
  static LineCompletorRule& getInstance() {
    static LineCompletorRule instance;
    return instance;
  }
  
  LineCompletorRule() = default;
  LineCompletorRule(std::size_t line_radius) : line_radius_(line_radius) {}
  ~LineCompletorRule() override = default;

  uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const override;
  uint8_t apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const override;
  std::string getName() const override;

  void setLineRadius(std::size_t line_radius) const;

  inline static AutoRegisterRule<LineCompletorRule> auto_register_line_completor{LINE_COMPLETOR_RULE_NAME, "Rule that completes lines of a certain length."};
private:
  mutable std::size_t line_radius_ = 4; // todo: delete this later
};
