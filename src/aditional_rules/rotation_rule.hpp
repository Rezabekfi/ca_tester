#pragma once

#include "core/rule.hpp"
#include "core/rule_registry.hpp"

constexpr std::string ROTATION_RULE_NAME = "Rotation Rule";


// NOTE: current implementation is cheating a bit because we take advantage of knowing whole grid instead of just neighbors which is not allowed by CA definition
// This stays instead of implementing the rule from PHD thesis as pursuing more complex rules for rotation wasnt advised by supervisor because even this version already shows bad results and the more complex version would be even more expensive and not necessarily better

// This rule imitates a rule described in the PHD thesis by Chowdhury Debashis
// The imitation is done by classic rotation of points around a fixed point

class RotationRule: public Rule {
public:
  
  RotationRule() = default;
  ~RotationRule() override = default;

  // Singleton pattern to ensure only one instance (for shared parameters and registry)
  static RotationRule& getInstance() {
    static RotationRule instance;
    return instance;
  }


  // context version needed for this rule
  uint8_t apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const override;
  uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const override;
  std::string getName() const override;

  inline static AutoRegisterRule<RotationRule> auto_register{ROTATION_RULE_NAME, "A rule that rotates cell states based on their neighbors."};
private:
  // Fixed point for rotation (relative to cell position), can be tuned for different effects
  std::pair<std::size_t, std::size_t> fixed_point_{25, 15};
  // Rotation angle in degrees, can be tuned for different effects
  double rotation_degree_{5.0};
  std::pair<int, int> rotate_point(int x, int y, double degree) const;
};
