#import "convex_hull.hpp"

uint8_t ConvexHull::apply(uint8_t current_state, std::vector<uint8_t> neighbours) const {
  if ((current_state & 0x01) == 1) {
    return current_state;
  }
  bool mark = false;
  mark |= even_center(current_state, neighbours);
  mark |= odd_center(current_state, neighbours);
  mark |= back_mark(current_state, neighbours);
  mark |= exists_oposite_marked_neighbor(current_state, neighbours);
  if (mark) {
    return (current_state + 1);
  }
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

bool ConvexHull::even_center(uint8_t current_state, const std::vector<uint8_t>& neighbours) const {
  uint8_t dist_x = current_state / 10;
  uint8_t wanted_dist = 0;
  (dist_x == 0) ? wanted_dist = 2 : wanted_dist = dist_x - 1;
  std::size_t half_size = neighbours.size() / 2;
  for (std::size_t i = 0; i < half_size; ++i) {
    if (neighbours[i] / 10 == wanted_dist && neighbours[i + half_size] / 10 == wanted_dist) {
      return true;
    }
  }
  return false;
}

bool ConvexHull::odd_center(uint8_t current_state, const std::vector<uint8_t>& neighbours) const {
  //TODO: implement low prio
  return false;
}

bool ConvexHull::back_mark(uint8_t current_state, const std::vector<uint8_t>& neighbours) const {
  uint8_t wanted_value = ((current_state + 10) % 30) + 1; // we want distance + 1 and marked
  for (auto neighbour : neighbours) {
    if (neighbour == wanted_value) {
      return true;
    }
  }
  return false;
}

bool ConvexHull::exists_oposite_marked_neighbor(uint8_t current_state, const std::vector<uint8_t>& neighbours) const {
  std::size_t half_size = neighbours.size() / 2;
  for (std::size_t i = 0; i < half_size; ++i) {
    if ((neighbours[i] & 0x01) == 1 && (neighbours[i + half_size] & 0x01) == 1) {
      return true;
    }
  }
  return false;
}


std::string ConvexHull::getName() const {
  return CONVEX_HULL_RULE_NAME;
}


