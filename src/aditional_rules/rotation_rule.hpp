#pragma once

#include "core/rule.hpp"
#include "core/rule_registry.hpp"

constexpr std::string ROTATION_RULE_NAME = "Rotation Rule";

class RotationRule: public Rule {
public:
  
  RotationRule() = default;
  ~RotationRule() override = default;

  static RotationRule& getInstance() {
    static RotationRule instance;
    return instance;
  }

  uint8_t apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const override;
  uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const override;
  std::string getName() const override;

  inline static AutoRegisterRule<RotationRule> auto_register{ROTATION_RULE_NAME, "A rule that rotates cell states based on their neighbors."};
private:
  std::pair<std::size_t, std::size_t> fixed_point_{25, 15};
  double rotation_degree_{10.0};
  std::pair<int, int> rotate_point(int x, int y, double degree) const;
};
