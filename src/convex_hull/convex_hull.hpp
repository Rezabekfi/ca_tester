#pragma once
#include "core/rule.hpp"
#include "core/grid.hpp"
#include "core/rule_registry.hpp"

// This rule is taken from chapter Convex Hulls and Metric Gabriel Graphs (by Luidnel Maignan & Frédéric Gruau) of the book Cellular Automata in Image Processing and Geometry with some adjustments to fit the von neumann neighborhood

// Encodes multiple CA states into one byte (bit packing)
// This is key: rule isn't binary anymore → multiple "layers" of info per cell
struct CellBits {
  static constexpr uint8_t SEED_MASK = 0x01;   // initial input points - seeds
  static constexpr uint8_t MARK_MASK = 0x02;   // cells belonging to hull - marked
  static constexpr uint8_t DIST_MASK = 0x0C;   // wavefront distance (2 bits) - dist
  static constexpr uint8_t DIST_SHIFT = 2;

  // Extra flags used by intermediate steps of the algorithm
  static constexpr uint8_t CENTER_CELL_MASK = 0x10;
  static constexpr uint8_t UNUSED_CENTER_CELL_MASK = 0x20;
  static constexpr uint8_t ORIGIN_MASK = 0x40; // this is used for reactivating seeds in another rule
};

// --- Bit helpers ---
inline constexpr bool is_seed(uint8_t s)              { return (s & CellBits::SEED_MASK) != 0; }
inline constexpr bool is_marked(uint8_t s)            { return (s & CellBits::MARK_MASK) != 0; }

inline constexpr uint8_t raw_distance(uint8_t s)      { return (s & CellBits::DIST_MASK) >> CellBits::DIST_SHIFT; }

// Distance wraps mod 3
inline constexpr uint8_t get_distance(uint8_t s)      { return raw_distance(s) % 3; }

inline constexpr uint8_t with_seed(uint8_t s, bool v) {
  return v ? (s | CellBits::SEED_MASK) : (s & ~CellBits::SEED_MASK);
}

inline constexpr uint8_t mark_cell(uint8_t s)         { return s |  CellBits::MARK_MASK; }
inline constexpr uint8_t unmark_cell(uint8_t s)       { return s & ~CellBits::MARK_MASK; }

// Origin flag for seeds (used to reactivate them in another rule)
inline constexpr uint8_t mark_origin(uint8_t s)       { return s |  CellBits::ORIGIN_MASK; }
inline constexpr uint8_t is_origin(uint8_t s)         { return (s & CellBits::ORIGIN_MASK) != 0; }

// Center-related flags used in intermediate geometric checks
// This is added on top of the algorithm from the book
inline constexpr uint8_t mark_center_cell(uint8_t s)    { return s |  CellBits::CENTER_CELL_MASK; }
inline constexpr uint8_t unmark_center_cell(uint8_t s)  { return s & ~CellBits::CENTER_CELL_MASK; }
inline constexpr bool is_center_cell(uint8_t s)         { return (s & CellBits::CENTER_CELL_MASK) != 0; }

inline constexpr uint8_t mark_unused_center_cell(uint8_t s)    { return s |  CellBits::UNUSED_CENTER_CELL_MASK; }
inline constexpr uint8_t unmark_unused_center_cell(uint8_t s)  { return s & ~CellBits::UNUSED_CENTER_CELL_MASK; }
inline constexpr bool is_unused_center_cell(uint8_t s)         { return (s & CellBits::UNUSED_CENTER_CELL_MASK) != 0; }

// Sets distance while preserving other flags
inline constexpr uint8_t set_distance(uint8_t s, uint8_t d) {
  d = static_cast<uint8_t>(d % 3); // algorithm relies on 3-state cycling
  s = static_cast<uint8_t>(s & ~CellBits::DIST_MASK);
  return static_cast<uint8_t>(s | (d << CellBits::DIST_SHIFT));
}

// Distance propagation step (wavefront expansion)
// Seeds stay at distance 0 → they act as sources
inline constexpr uint8_t advance_distance(uint8_t s) {
  if (is_seed(s)) return set_distance(s, 0);
  return set_distance(s, static_cast<uint8_t>(get_distance(s) + 1));
}

// Reverse wave (used in backtracking phase)
inline constexpr uint8_t retreat_distance(uint8_t s) {
  if (is_seed(s)) return set_distance(s, 0);
  return set_distance(s, static_cast<uint8_t>((get_distance(s) + 2) % 3));
}

// Helper to construct a full cell state
inline constexpr uint8_t create_cell(bool seed, bool marked, uint8_t distance) {
  uint8_t s = 0;
  if (seed)   s |= CellBits::SEED_MASK;
  if (marked) s |= CellBits::MARK_MASK;
  return set_distance(s, distance);
}

// Registry name
constexpr std::string CONVEX_HULL_RULE_NAME = "Convex Hull"; 

// Complex CA rule: builds convex hull from seed points
// Uses multi-phase logic encoded in bits (distance + marks)
class ConvexHull : public Rule {
public:
  
  ConvexHull() = default;
  ~ConvexHull() override = default;

  // Main rule using full context (needed for geometry-like checks)
  uint8_t apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const override;

  // unused simple version
  uint8_t apply(uint8_t current_state, std::vector<uint8_t> neighbours) const override;

  std::string getName() const override;
  
  // Preprocessing step run outside normal CA step (Engine hook)
  // Computes distance layers before rule logic kicks in
  static void calculateDistances(std::vector<uint8_t>& grid, std::size_t width, std::size_t height, Neighborhood neighborhood, Boundary boundary);

  // Auto-register rule
  static inline AutoRegisterRule<ConvexHull> auto_register_convex_hull{
    CONVEX_HULL_RULE_NAME,
    "Convex Hull rule that computes the convex hull of a set of seed cells."
  };

private:
  
  // all of these all from the book but I changed them a little bit
  
  // Detects edge centers (part of hull construction)
  uint8_t edge_center(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const;

  // Detects vertex centers (part of hull construction)
  uint8_t vertex_center(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const;

  // Backward marking phase (propagates hull selection)
  bool back_mark(uint8_t current_state, const std::vector<uint8_t>& neighbours) const;

  // Checks if opposite neighbors are marked
  bool exists_oposite_marked_neighbor(uint8_t current_state, const std::vector<uint8_t>& neighbours) const;

  // Checks if two regions belong to distinct wavefronts condition given by the book for edge center and vertex center detection
  bool distinct_sets(const RuleContext& ctx, std::size_t nx, std::size_t ny, std::size_t index_x, std::size_t index_y, uint8_t goal_distance) const;

  // Helper to extract neighbor states for the distinct sets check (used by edge/vertex center logic)
  std::pair<std::vector<uint8_t>, std::vector<uint8_t>> getDistinctNeighbourhoods(std::size_t nx, std::size_t ny, const RuleContext& ctx) const;
};
