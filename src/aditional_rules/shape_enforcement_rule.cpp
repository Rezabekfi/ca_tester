#include "shape_enforcement_rule.hpp"
#include "core/grid.hpp"

// context free version not used
uint8_t ShapeEnforcement::apply(uint8_t current_state, std::vector<uint8_t> neighbours) const {
  return current_state;
}

// Keeps cells only if their local shape matches a known line/corner pattern meaning it preserves shapes that could represent rectangles (used for thesis)
uint8_t ShapeEnforcement::apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const {
  if (current_state == 0) { 
    return 0; // dead stays dead
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
        return 0; // border cells are unsafe for this pattern check
      }

      configuration[i] = grid_values[ny * ctx.getGrid().getWidth() + nx];
    }

    // Match against allowed local shapes; J means "don't care"
    for (const auto& config : moore_clearing_configs) {
      bool match = true;

      for (std::size_t i = 0; i < deltas.size(); ++i) {
        if (config[i] != J && (configuration[i] & 0x03) != (config[i] & 0x03)) {
          match = false;
          break;
        }
      }

      if (match) return current_state;
    }

    return current_state & 0xFC; // clear alive/mark bits, keep metadata
  } else if (ctx.getNeighborhood() == Neighborhood::VonNeumann) { 
    // TODO: add Von Neumann cleanup patterns this isnt useful for us right now
  }

  return current_state;
}

std::string ShapeEnforcement::getName() const {
    return SHAPE_ENFORCEMENT_RULE;
}
