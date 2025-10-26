#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include "rule_context.hpp"

class Rule {
public:
  virtual ~Rule() = default;
  virtual uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const = 0;

  virtual uint8_t apply(uint8_t current_state, const RuleContext& ctx,
                        const std::vector<uint8_t>& neighbours) const {
    return apply(current_state, neighbours);
  }

  virtual std::string getName() const = 0;
};

