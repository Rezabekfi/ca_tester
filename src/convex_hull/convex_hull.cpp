#include "convex_hull.hpp"
#include <iostream>

uint8_t ConvexHull::apply(uint8_t current_state, std::vector<uint8_t> neighbours) const {
  if (is_seed(current_state) || is_marked(current_state)) {
    return current_state;
  }
  bool mark = false;
  mark |= even_center(current_state, neighbours);
  mark |= odd_center(current_state, neighbours);
  mark |= back_mark(current_state, neighbours);
  mark |= exists_oposite_marked_neighbor(current_state, neighbours);
  if (mark) {
    return mark_cell(current_state);
  }
  return current_state;
}


void ConvexHull::calculateDistances(std::vector<uint8_t>& grid, std::size_t width, std::size_t height, Neighborhood neighborhood, Boundary boundary) {
  std::vector<uint8_t> old_grid = grid; // make a copy of the original grid
  for (std::size_t y = 0; y < height; ++y) {
    for (std::size_t x = 0; x < width; ++x) {
      if (is_seed(old_grid[y * width + x])) {
        continue; // skip seeds 
      }
      std::vector<uint8_t> neighbors = Grid::getNeighborsStatic(old_grid, x, y, width, height, neighborhood, boundary);
      bool all_same = true;
      uint8_t distance = get_distance(grid[y * width + x]); 
      for (uint8_t neighbor : neighbors) {
        if (get_distance(neighbor) != distance) {
          all_same = false;
          break;
        } 
      }
      if (all_same) { 
        grid[y * width + x] = advance_distance(grid[y * width + x]);
      }
    }
  }
}

bool ConvexHull::even_center(uint8_t current_state, const std::vector<uint8_t>& neighbours) const {
  uint8_t dist_x = get_distance(current_state);
  uint8_t wanted_dist = 0;
  (dist_x == 0) ? wanted_dist = get_distance(set_distance(wanted_dist, 2)) : wanted_dist =  get_distance(set_distance(wanted_dist, dist_x - 1));
  std::size_t half_size = neighbours.size() / 2;
  for (std::size_t i = 0; i < half_size; ++i) {
    if (get_distance(neighbours[i]) == wanted_dist && get_distance(neighbours[i + half_size]) == wanted_dist) {
     return true;
    }
  }
  return false;
}

bool ConvexHull::odd_center(uint8_t current_state, const std::vector<uint8_t>& neighbours) const {
  return false;
}

bool ConvexHull::back_mark(uint8_t current_state, const std::vector<uint8_t>& neighbours) const {
  uint8_t wanted_value = create_cell(false, true, advance_distance(current_state)); // not seed, marked, distance advanced by one
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
    if ((is_marked(neighbours[i]) || is_seed(neighbours[i])) && (is_marked(neighbours[i + half_size]) || is_seed(neighbours[i + half_size]))) {
      return true;
    }
  }
  return false;
}


std::string ConvexHull::getName() const {
  return CONVEX_HULL_RULE_NAME;
}


