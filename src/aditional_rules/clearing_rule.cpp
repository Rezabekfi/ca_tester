#include "clearing_rule.hpp"
#include "core/grid.hpp"

uint8_t ClearingRule::apply(uint8_t current_state, std::vector<uint8_t> neighbours) const {
  return current_state;
}

uint8_t ClearingRule::apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const {
  // the logic is basically "if I am in the line or corner I will stay on otherwise I will die"
  if (current_state == 0) {
    return 0; // if the cell is already dead, it stays dead
  }
  auto deltas = pick_deltas(ctx.getNeighborhood());
  auto grid_values = ctx.getGrid().getGridValues();

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
    // compare structure of neighbours to the possible 14 configurations of line and corner in moore neighborhood and if it doesnt match any of them kill the cell
    for (const auto& config : moore_clearing_configs) {
      bool match = true;
      for (std::size_t i = 0; i < deltas.size(); ++i) {
        if (config[i] != J && configuration[i] != config[i]) {
          match = false;
          break;
        }
      }
      if (match) return current_state;
    }
    return current_state & 0xFC; 
  } else if (ctx.getNeighborhood() == Neighborhood::VonNeumann) { 

  }
  return current_state;
}

std::string ClearingRule::getName() const {
    return CLEARING_RULE_NAME;
}
