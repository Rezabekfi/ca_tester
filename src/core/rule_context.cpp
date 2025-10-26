#include "rule_context.hpp"
#include "grid.hpp" // now we can safely include full Grid definition

std::vector<uint8_t> RuleContext::getNeighbors(std::size_t px, std::size_t py) const {
    return Grid::getNeighborsStatic(grid.getGridValues(), px, py,
                                    grid.getWidth(), grid.getHeight(),
                                    neighborhood, boundary);
}

std::vector<uint8_t> RuleContext::getEdgeNeighborhood(std::size_t x1, std::size_t y1,
                                                      std::size_t x2, std::size_t y2) const {
    auto n1 = getNeighbors(x1, y1);
    auto n2 = getNeighbors(x2, y2);
    n1.insert(n1.end(), n2.begin(), n2.end());
    return n1;
}

