#pragma once

#include "core/rule.hpp"
#include "core/rule_registry.hpp"
#include "clearing_rule.hpp"
#include <array>

constexpr std::string EDGE_DETECTION_RULE_NAME = "Edge Detection Rule";

// J = JOKER

// bit setup:
// last bit is alive or dead, second last bit is whether the cell is horizontal edge, third last bit is whether the cell is vertical edge

constexpr std::array<std::array<uint8_t, 8>, 2> moore_horizontal_lines = {{
  {J,J,1,0,0,0,1,J},
  {0,0,1,J,J,J,1,0},
}};

constexpr std::array<std::array<uint8_t, 8>, 2> moore_vertical_lines = {{
  {1,0,0,0,1,J,J,J},
  {1,J,J,J,1,0,0,0},
}};

constexpr uint8_t HORIZONTAL_EDGE_MASK = 0x08; 
constexpr uint8_t VERTICAL_EDGE_MASK = 0x04; // third last bit

// change the magic numbers later
inline uint8_t add_horizontal_edge_bit(uint8_t state) {
  return state | HORIZONTAL_EDGE_MASK; 
}

inline uint8_t add_vertical_edge_bit(uint8_t state) {
  return state | VERTICAL_EDGE_MASK;
}

inline bool is_horizontal_edge(uint8_t state) {
  return (state & HORIZONTAL_EDGE_MASK) != 0;
}

inline bool is_vertical_edge(uint8_t state) {
  return (state & VERTICAL_EDGE_MASK) != 0;
}

inline bool is_alive(uint8_t state) {
  return (state & 0x01) != 0; // last bit is alive or dead
}

inline bool is_dead(uint8_t state) {
  return !is_alive(state);
}

inline bool is_corner(uint8_t state) {
  return is_horizontal_edge(state) && is_vertical_edge(state);
}

inline uint8_t make_dead(uint8_t state) {
  return state & 0xFE; // set last bit to 0
}


class EdgeDetectionRule: public Rule {
public:
  static EdgeDetectionRule& getInstance() {
    static EdgeDetectionRule instance;
    return instance;
  }
  EdgeDetectionRule() = default;
  ~EdgeDetectionRule() override = default;
  

  uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const override;
  uint8_t apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const override;
  std::string getName() const override;

  inline static AutoRegisterRule<EdgeDetectionRule> auto_register_edge_detection{EDGE_DETECTION_RULE_NAME, ""};
};
