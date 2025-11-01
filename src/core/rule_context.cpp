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

std::vector<std::pair<int, int>> RuleContext::getEdgeNeighborhoodWithCoordinates(std::size_t x1, std::size_t y1,
                                                                                  std::size_t x2, std::size_t y2) const {
    std::vector<std::pair<int, int>> coords;
    auto deltas = pick_deltas(neighborhood);
    for (const auto& delta : deltas) {
        int nx1 = static_cast<int>(x1) + delta.first;
        int ny1 = static_cast<int>(y1) + delta.second;
        coords.emplace_back(nx1, ny1);
    }
    for (const auto& delta : deltas) {
        int nx2 = static_cast<int>(x2) + delta.first;
        int ny2 = static_cast<int>(y2) + delta.second;
        coords.emplace_back(nx2, ny2);
    }
    return coords;
}

std::vector<std::pair<int, int>> RuleContext::getNeighborhoodWithCoordinates(std::size_t x, std::size_t y) const {
    std::vector<std::pair<int, int>> coords;
    auto deltas = pick_deltas(neighborhood);
    for (const auto& delta : deltas) {
        int nx = static_cast<int>(x) + delta.first;
        int ny = static_cast<int>(y) + delta.second;
        coords.emplace_back(nx, ny);
    }
    return coords;
}

