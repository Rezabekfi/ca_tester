#pragma once

// necessary includes for rule implementation
#include "core/rule.hpp"
#include "core/rule_registry.hpp"

// Change this to your desired rule name, it will appear in the UI and be used for registration
// Change even the name of the constant if you want but make sure to update it in the getName() method of the rule class as well
constexpr std::string EXAMPLE_RULE_NAME = "Example Rule";

// Minimal reference implementation for writing new rules
// Shows both APIs: simple (neighbors only) and full (with context)
// When changing the name make sure to change it in every single instance of this class (getName, auto_register, etc.) to ensure it is registered and appears in the UI correctly
class ExampleRule: public Rule {
public:

  // Singleton helper for places that want a shared rule instance
  // Do not change anything about this other than the class name
  static ExampleRule& getInstance() {
    static ExampleRule instance;
    return instance;
  }
  
  // keep these default
  ExampleRule() = default;
  ~ExampleRule() override = default;

  // Basic version:
  // - You only get current state + neighbor values
  // - Good for classic CA (Game of Life, etc.)
  uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const override;

  // Extended version:
  // - Gives access to position (ctx.x, ctx.y), grid, iteration, etc.
  // - Use this if rule depends on location, time, or custom sampling
  uint8_t apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const override;

  // Name used by registry/UI
  std::string getName() const override;

  // Registers this rule automatically so it appears in the app
  inline static AutoRegisterRule<ExampleRule> auto_register{
    EXAMPLE_RULE_NAME,
    "Example rule that serves as a template for creating new rules."
  };
};
