#import "convex_hull.hpp"

uint8_t ConvexHull::apply(uint8_t current_state, std::vector<uint8_t> neighbours) const {
  bool mark = (current_state == 1);

  return current_state;
}

void ConvexHull::calculateDistances(std::vector<uint8_t>& grid, std::size_t width, std::size_t height, Neighborhood neighborhood, Boundary boundary) const {
  std::vector<uint8_t> old_grid = grid; // make a copy of the original grid
  for (std::size_t y = 0; y < height; ++y) {
    for (std::size_t x = 0; x < width; ++x) {
      if (grid[y * width + x] == 1) {
        continue; // skip alive cells
      }
      std::vector<uint8_t> neighbors = Grid::getNeighborsStatic(old_grid, x, y, width, height, neighborhood, boundary);
      bool all_same = true;
      uint8_t value = old_grid[y * width + x]; 
      for (auto neighbor : neighbors) {
        if (neighbor != value) {
          all_same = false;
          break;
        } 
      }
      if (!all_same) {
        grid[y * width + x] = (grid[y * width + x] + 10) % 30;
      }
    }
  }

}

std::string ConvexHull::getName() const {
  return CONVEX_HULL_RULE_NAME;
}
