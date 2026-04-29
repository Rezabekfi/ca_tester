#pragma once

#include "core/rule.hpp"
#include "core/rule_registry.hpp"
#include "shape_enforcement_rule.hpp" // for J wildcard and pattern matching
#include <array>

inline constexpr const char* EDGE_DETECTION_RULE_NAME = "Edge Detection Rule";

// Pattern joker reused from shape enforcement rule for flexible matching
// // J means "don't care" in neighborhood configs

// Cell bit layout:
// b0 = alive/dead, b2 = vertical edge, b3 = horizontal edge

constexpr std::array<std::array<uint8_t, 8>, 2> moore_horizontal_lines = {{
  {J,J,1,0,0,0,1,J},
  {0,0,1,J,J,J,1,0},
}};

constexpr std::array<std::array<uint8_t, 8>, 2> moore_vertical_lines = {{
  {1,0,0,0,1,J,J,J},
  {1,J,J,J,1,0,0,0},
}};

constexpr uint8_t HORIZONTAL_EDGE_MASK = 0x08; 
constexpr uint8_t VERTICAL_EDGE_MASK = 0x04;

// Tags cell as part of a horizontal edge
inline uint8_t add_horizontal_edge_bit(uint8_t state) {
  return state | HORIZONTAL_EDGE_MASK; 
}

// Tags cell as part of a vertical edge
inline uint8_t add_vertical_edge_bit(uint8_t state) {
  return state | VERTICAL_EDGE_MASK;
}

inline bool is_horizontal_edge(uint8_t state) {
  return (state & HORIZONTAL_EDGE_MASK) != 0;
}

inline bool is_vertical_edge(uint8_t state) {
  return (state & VERTICAL_EDGE_MASK) != 0;
}

// Alive state still lives in the lowest bit
inline bool is_alive(uint8_t state) {
  return (state & 0x01) != 0;
}

inline bool is_dead(uint8_t state) {
  return !is_alive(state);
}

// A corner belongs to both edge directions
inline bool is_corner(uint8_t state) {
  return is_horizontal_edge(state) && is_vertical_edge(state);
}

// Clears only alive bit, keeps edge metadata around
inline uint8_t make_dead(uint8_t state) {
  return state & 0xFE;
}

// Detects straight edge patterns and grows them outward
class EdgeDetectionRule: public Rule {
public:
  static EdgeDetectionRule& getInstance() {
    static EdgeDetectionRule instance;
    return instance;
  }

  EdgeDetectionRule() = default;
  ~EdgeDetectionRule() override = default;
  
  // not used
  uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const override;

  // Main version uses context for direction/position-aware growth
  uint8_t apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const override;

  std::string getName() const override;

  // Auto-registers rule for UI selection
  inline static AutoRegisterRule<EdgeDetectionRule> auto_register_edge_detection{
    EDGE_DETECTION_RULE_NAME,
    ""
  };
};
