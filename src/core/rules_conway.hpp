#pragma once

#include "rule.hpp"
#include <cstdint>
#include <vector>

constexpr std::string CONWAY_RULE_NAME = "Conway's";

class ConwayRule : public Rule {
public:
  
  ConwayRule() = default;
  ~ConwayRule() override = default;

  uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const override;
  std::string getName() const override;
};


