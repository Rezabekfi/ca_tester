#include "edge_detection.hpp"
#include "core/grid.hpp"
#include "anti_convex_hull.hpp"

// not used, this rule needs context for the neighborhood checks
uint8_t EdgeDetectionRule::apply(uint8_t current_state, std::vector<uint8_t> neighbours) const {
  return current_state;
}

// Rule that detects edges and then it grows them in that direction
// WARNING: this rule is modified to work only within the inside space marked by rule "anti_convex_hull" so using it without it will not do anything
uint8_t EdgeDetectionRule::apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const {
  auto deltas = pick_deltas(ctx.getNeighborhood());
  auto grid_values = ctx.getGrid().getGridValues();

  if (current_state == OUTSIDE_CELL_VALUE) {
    return current_state; // if the cell is already outside, it stays outside
  }

  // load current neighborhood in configuration
  if (ctx.getNeighborhood() == Neighborhood::Moore) {
    deltas = pick_deltas(Neighborhood::Moore);
    std::array<uint8_t, 8> configuration{};
    for (std::size_t i = 0; i < deltas.size(); ++i) {
      int nx = static_cast<int>(ctx.x) + deltas[i].first; 
      int ny = static_cast<int>(ctx.y) + deltas[i].second;
      if (nx < 0 || ny < 0 || nx >= static_cast<int>(ctx.getGrid().getWidth()) || ny >= static_cast<int>(ctx.getGrid().getHeight())) {
        return 0; // out of bounds, kill the cell
      }
      configuration[i] = grid_values[ny * ctx.getGrid().getWidth() + nx];
    }
    bool match;
    // try to match against edge horizontal patterns
    for (const auto& config : moore_horizontal_lines) {
      if (is_dead(current_state)) break;
      match = true;
      for (std::size_t i = 0; i < deltas.size(); ++i) {
        if (config[i] != J && (configuration[i] & 0x01) != (config[i] & 0x01)) { // only compare alive/dead state for vertical lines
          match = false;
          break;
        }
      }
      if (match) {
        current_state = add_horizontal_edge_bit(current_state);
        break;
      }
    }
    // ty to match against vertical line patterns
    for (const auto& config : moore_vertical_lines) {
      if (is_dead(current_state)) break;
      match = true;
      for (std::size_t i = 0; i < deltas.size(); ++i) {
        if (config[i] != J && (configuration[i] & 0x01) != (config[i] & 0x01)) { // only compare alive/dead state for vertical lines
          match = false;
          break;
        }
      } 
      if (match) {
        current_state = add_vertical_edge_bit(current_state);
        break;
      }
    }
    // spread the edge bits

    if (is_vertical_edge(configuration[0]) || is_vertical_edge(configuration[4])) {
      current_state = add_vertical_edge_bit(current_state);
    }
    if (is_horizontal_edge(configuration[2]) || is_horizontal_edge(configuration[6])) {
      current_state = add_horizontal_edge_bit(current_state);
    }

    if ((is_vertical_edge(current_state) || is_horizontal_edge(current_state)) && is_inside(current_state)) {
      return current_state | 0x01; // set alive bit if it is an edge and was inside before
    }

    return make_dead(current_state); // does not match any configuration, kill the cell but keep the edge bits if they were set
  } else if (ctx.getNeighborhood() == Neighborhood::VonNeumann) { 

  }
  return current_state;

}

std::string EdgeDetectionRule::getName() const {
    return EDGE_DETECTION_RULE_NAME;
}
