#pragma once

#include <cstdint>
#include <vector>
#include <cstddef>

// TODO: invest some time into designing a flexible rule system that can be extended in the future
class Rule {
public:
  virtual ~Rule() = default;
  virtual uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours ) const = 0;
  virtual std::string getName() const = 0;
};
