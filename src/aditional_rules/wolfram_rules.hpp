#pragma once

#include "core/rule.hpp"
#include "core/rule_registry.hpp"

constexpr std::string WOLFRAM_RULE_NAME = "Wolfram Rule xx";

// Wolfram Elementary Cellular Automaton Rule
// NOTE: This is a reimagination of Wolfram 1D rules to 2D rules by acting like each row is one iteration of 1D CA so each step creates a new row
// this might cause some isses when used without caution (it is used just to generate ilustrative patterns for thesis purposes)

class WolframRule: public Rule {
public:
  // the number represents the 8-bit rule number as defined in Wolfram's elementary cellular automata, where each bit corresponds to a specific neighborhood configuration (111, 110, 101, 100, 011, 010, 001, 000) and determines the next state of the cell based on that configuration
  WolframRule(uint8_t rule_number);
  WolframRule() : WolframRule(54) {}
  ~WolframRule() override = default;

  // context version needed for this rule as we simulate line by line generation which is not natural for CA
  uint8_t apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const override;
  uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const override;
  std::string getName() const override;

  // Auto-register for UI selection
  inline static AutoRegisterRule<WolframRule> auto_register{WOLFRAM_RULE_NAME, "A Wolfram Elementary Cellular Automaton Rule."};
private:

  static uint8_t getNextState(uint8_t neighbourhood_values, uint8_t rule_number);
  // Stores the 8-bit rule number that defines the behavior of the Wolfram rule
  uint8_t rule_number_;
};
