#pragma once
#include "core/rule.hpp"
#include "core/grid.hpp"

// Bit layout:
// b0: SEED, b1: MARK, b2..b3: DIST (0..2; value 3 is unused)
struct CellBits {
  static constexpr uint8_t SEED_MASK = 0x01;
  static constexpr uint8_t MARK_MASK = 0x02;
  static constexpr uint8_t DIST_MASK = 0x0C; // bits 2..3
  static constexpr uint8_t DIST_SHIFT = 2;
};

inline constexpr bool is_seed(uint8_t s)              { return (s & CellBits::SEED_MASK) != 0; }
inline constexpr bool is_marked(uint8_t s)            { return (s & CellBits::MARK_MASK) != 0; }
inline constexpr uint8_t raw_distance(uint8_t s)      { return (s & CellBits::DIST_MASK) >> CellBits::DIST_SHIFT; }

inline constexpr uint8_t get_distance(uint8_t s)      { return raw_distance(s) % 3; }

inline constexpr uint8_t with_seed(uint8_t s, bool v) {
  return v ? (s | CellBits::SEED_MASK) : (s & ~CellBits::SEED_MASK);
}
inline constexpr uint8_t mark_cell(uint8_t s)         { return s |  CellBits::MARK_MASK; }
inline constexpr uint8_t unmark_cell(uint8_t s)       { return s & ~CellBits::MARK_MASK; }

inline constexpr uint8_t set_distance(uint8_t s, uint8_t d) {
  d = static_cast<uint8_t>(d % 3);
  s = static_cast<uint8_t>(s & ~CellBits::DIST_MASK);
  return static_cast<uint8_t>(s | (d << CellBits::DIST_SHIFT));
}

// Advance distance one step (mod 3), preserving seed/mark bits.
// If seed: keep distance at 0.
inline constexpr uint8_t advance_distance(uint8_t s) {
  if (is_seed(s)) return set_distance(s, 0);
  return set_distance(s, static_cast<uint8_t>(get_distance(s) + 1));
}

inline constexpr uint8_t retreat_distance(uint8_t s) {
  if (is_seed(s)) return set_distance(s, 0);
  return set_distance(s, static_cast<uint8_t>((get_distance(s) + 2) % 3));
}

inline constexpr uint8_t create_cell(bool seed, bool marked, uint8_t distance) {
  uint8_t s = 0;
  if (seed)   s |= CellBits::SEED_MASK;
  if (marked) s |= CellBits::MARK_MASK;
  return set_distance(s, distance);
}

constexpr std::string CONVEX_HULL_RULE_NAME = "Convex Hull"; 

class ConvexHull : public Rule {
public:
  
  ConvexHull() = default;
  ~ConvexHull() override = default;

  uint8_t apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const override;
  uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const override;
  std::string getName() const override;
  
  static void calculateDistances(std::vector<uint8_t>& grid, std::size_t width, std::size_t height, Neighborhood neighborhood, Boundary boundary);
private:
  bool edge_center(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const;
  bool vertex_center(uint8_t current_state, const std::vector<uint8_t>& neighbours) const;
  bool back_mark(uint8_t current_state, const std::vector<uint8_t>& neighbours) const;
  bool exists_oposite_marked_neighbor(uint8_t current_state, const std::vector<uint8_t>& neighbours) const;
  std::pair<std::vector<uint8_t>, std::vector<uint8_t>> getDistinctNeighbourhoods(std::size_t nx, std::size_t ny, const RuleContext& ctx) const;
};
