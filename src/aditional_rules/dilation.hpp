#pragma once

#include "core/rule.hpp"
#include "core/rule_registry.hpp"

inline constexpr const char* DILATION_RULE_NAME = "Dilation";

// Morphological dilation: grows active cells outward
class DilationRule: public Rule {
public:

  static DilationRule& getInstance() {
    static DilationRule instance;
    return instance;
  }
  
  DilationRule() = default;
  ~DilationRule() override = default;

  // Turns cells on based on nearby active neighbors
  uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const override;

  std::string getName() const override;

  // Auto-registers so the rule appears in the app
  inline static AutoRegisterRule<DilationRule> auto_register_dilation{
    DILATION_RULE_NAME,
    "Dilation rule that adds cells around existing ones."
  };
};
