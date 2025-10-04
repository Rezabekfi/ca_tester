#pragma once

#include "rule.hpp"
#include <cstdint>
#include <vector>



class ConwayRule : public Rule {
public:
  ConwayRule() = default;
  ~ConwayRule() override = default;

  uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const override;
};


