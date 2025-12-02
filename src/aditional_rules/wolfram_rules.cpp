#include "wolfram_rules.hpp"
#include "core/grid.hpp"

WolframRule::WolframRule(uint8_t rule_number)
  : rule_number_(rule_number) {}

uint8_t WolframRule::apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const {
  if (ctx.y == 0) {
    return current_state; // first row remains unchanged
  }
  uint8_t neighbourhood_values = 0; // last three bits represent left, center, right
  if (ctx.x != 0) {
    neighbourhood_values |= (ctx.getGrid().getCell(ctx.x - 1, ctx.y - 1) & 0x01) << 2; 
  }
  if (ctx.x != ctx.getGrid().getWidth() - 1) {
    neighbourhood_values |= (ctx.getGrid().getCell(ctx.x + 1, ctx.y - 1) & 0x01) << 0;
  }
  neighbourhood_values |= (ctx.getGrid().getCell(ctx.x, ctx.y - 1) & 0x01) << 1;

  return getNextState(neighbourhood_values, rule_number_);
}

uint8_t WolframRule::getNextState(uint8_t neighbourhood_values, uint8_t rule_number) {
  switch (neighbourhood_values) {
    case 0b111: return (rule_number & 0b10000000) ? 1 : 0;
    case 0b110: return (rule_number & 0b01000000) ? 1 : 0;
    case 0b101: return (rule_number & 0b00100000) ? 1 : 0;
    case 0b100: return (rule_number & 0b00010000) ? 1 : 0;
    case 0b011: return (rule_number & 0b00001000) ? 1 : 0;
    case 0b010: return (rule_number & 0b00000100) ? 1 : 0;
    case 0b001: return (rule_number & 0b00000010) ? 1 : 0;
    case 0b000: return (rule_number & 0b00000001) ? 1 : 0;
    default: return 0; // should never happen
  }
}

uint8_t WolframRule::apply(uint8_t current_state, std::vector<uint8_t> neighbours) const {
  return current_state;
}

std::string WolframRule::getName() const {
    return WOLFRAM_RULE_NAME + " " + std::to_string(static_cast<int>(rule_number_));
}
