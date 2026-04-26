#include "line_completor.hpp"
#include "core/grid.hpp"

// Context-free version does nothing; this rule needs position + wider grid access
uint8_t LineCompletorRule::apply(uint8_t current_state, std::vector<uint8_t> neighbours) const {
  return current_state;
}

// Fills cells that look like they belong inside a horizontal or vertical line
uint8_t LineCompletorRule::apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const {
  std::size_t active_count_line = 0;
  std::size_t active_count_column = 0;
  std::size_t dead_count_line = 0;
  std::size_t dead_count_column = 0;

  // Radius can fall back to context when explicitly left unset
  if (line_radius_ == 0) {
    line_radius_ = ctx.getRadius();
  }

  auto grid_values = ctx.getGrid().getGridValues();

  // Keeps std::size_t math from wrapping around at borders
  auto in_bounds = [&](int x, int y) {
    return x >= 0 && x < static_cast<int>(ctx.getGrid().getWidth()) && y >= 0 && y < static_cast<int>(ctx.getGrid().getHeight());
  };

  // Scan both sides horizontally and vertically
  for (std::size_t i = 1; i <= line_radius_; ++i) {
    (in_bounds(static_cast<int>(ctx.x) + static_cast<int>(i), static_cast<int>(ctx.y)) && grid_values[ctx.y * ctx.getGrid().getWidth() + ctx.x + i] > 0) ? ++active_count_line : ++dead_count_line;
    (in_bounds(static_cast<int>(ctx.x) - static_cast<int>(i), static_cast<int>(ctx.y)) && grid_values[ctx.y * ctx.getGrid().getWidth() + ctx.x - i] > 0) ? ++active_count_line : ++dead_count_line;
    (in_bounds(static_cast<int>(ctx.x), static_cast<int>(ctx.y) + static_cast<int>(i)) && grid_values[(ctx.y + i) * ctx.getGrid().getWidth() + ctx.x] > 0) ? ++active_count_column : ++dead_count_column;
    (in_bounds(static_cast<int>(ctx.x), static_cast<int>(ctx.y) - static_cast<int>(i)) && grid_values[(ctx.y - i) * ctx.getGrid().getWidth() + ctx.x] > 0) ? ++active_count_column : ++dead_count_column;
  }

  // If either axis has enough support, turn this cell alive
  return (active_count_line > line_radius_ || active_count_column > line_radius_) ? (current_state | 0x01) : current_state; 
}

std::string LineCompletorRule::getName() const {
    return LINE_COMPLETOR_RULE_NAME;
}

// this ended up being not used but it is for sure useful to add it later so for now I just left it here
// in this state
void LineCompletorRule::setLineRadius(std::size_t line_radius_) const {
  line_radius_ = line_radius_;
}
