#pragma once

#include "rule.hpp"
#include "rule_registry.hpp"
#include <cstdint>
#include <vector>

// Used as registry key + display name (must match registration)
constexpr std::string CONWAY_RULE_NAME = "Conway's";

// Classic Game of Life rule implementation
class ConwayRule : public Rule {
public:
  
  ConwayRule() = default;
  ~ConwayRule() override = default;

  // Applies Conway logic:
  // depends on count of alive neighbors (not shown here but implemented in .cpp)
  uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const override;

  // Returns name for UI / identification
  std::string getName() const override;

  // Static registration → makes rule available in registry before main()
  inline static AutoRegisterRule<ConwayRule> auto_register{CONWAY_RULE_NAME, "Classic Conway's Game of Life rule."};
};
