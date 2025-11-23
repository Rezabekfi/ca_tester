#include "convex_hull.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>

  // check if you can go from main_index (meaning one point of the neighborhood of ctx.x,ctx.y) to the second_index if yes return false they are not distinct else return true
bool ConvexHull::distinct_sets(const RuleContext& ctx, std::size_t nx, std::size_t ny, std::size_t main_index, std::size_t second_index, uint8_t goal_distance) const {
  auto all_coords_to_check = ctx.getEdgeNeighborhoodWithCoordinates(ctx.x, ctx.y, nx, ny);
  // copy ctx so we can modify it
  RuleContext local_ctx = ctx;
  local_ctx.setNeighborhood(Neighborhood::Moore); // COMMENT
  auto deltas = pick_deltas(ctx.neighborhood);
  // out of bounds function
  auto out_of_bounds = [&](int x, int y) {
    return (x < 0 || y < 0 || x >= static_cast<int>(ctx.getGrid().getWidth()) || y >= static_cast<int>(ctx.getGrid().getHeight()));
  };
  std::vector<std::pair<int,int>> to_visit;
  std::vector<std::pair<int,int>> visited;
  std::pair<int,int> goal = {static_cast<int>(nx) + deltas[second_index].first, static_cast<int>(ny) + deltas[second_index].second};
  to_visit.emplace_back(static_cast<int>(ctx.x) + deltas[main_index].first, static_cast<int>(ctx.y) + deltas[main_index].second);
  while (!to_visit.empty()) {
    auto current = to_visit.back();
    to_visit.pop_back();
    if (std::find(visited.begin(), visited.end(), current) != visited.end()) {
      continue; // already visited
    }
    visited.push_back(current);
    if (current == goal) {
      return false; // found a path to the second index
    }
    // get neighbors of current
    auto neighbors = local_ctx.getNeighborhoodWithCoordinates(static_cast<std::size_t>(current.first), static_cast<std::size_t>(current.second));
    for (const auto& neighbor : neighbors) {
      // skip if out of bounds and if not in all_coords_to_check
      if (out_of_bounds(neighbor.first, neighbor.second)) {
        continue;
      }
      if (std::find(all_coords_to_check.begin(), all_coords_to_check.end(), neighbor) == all_coords_to_check.end()) {
        continue;
      }
      uint8_t neighbor_state = ctx.getGrid().getGridValues()[neighbor.second * ctx.getGrid().getWidth() + neighbor.first];
      if (get_distance(neighbor_state) == goal_distance) {
        to_visit.push_back(neighbor);
      }
    }
  }
  return true; // no path found to the second index
}

uint8_t ConvexHull::apply(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const {
  if (is_seed(current_state) || is_marked(current_state)) return current_state;

  bool mark = false;
  mark |= vertex_center(current_state, ctx, neighbours);
  mark |= edge_center(current_state, ctx, neighbours);
  mark |= back_mark(current_state, neighbours);
  mark |= exists_oposite_marked_neighbor(current_state, neighbours);
  return mark ? mark_cell(current_state) : current_state;
}

uint8_t ConvexHull::apply(uint8_t current_state, std::vector<uint8_t> neighbours) const {
  // This method is not used in this rule, but must be implemented.
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

bool ConvexHull::vertex_center(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const {
  uint8_t dist_x = get_distance(current_state);
  bool all_same = true;
  for (auto neighbour :neighbours) {
    if (get_distance(neighbour) != dist_x) {
      all_same = false;
      break;
    }
  }
  if (all_same) dist_x = get_distance(advance_distance(current_state));
  uint8_t wanted_dist = get_distance(retreat_distance(current_state)); 
  std::size_t half_size = neighbours.size() / 2;
  for (std::size_t i = 0; i < half_size; ++i) {
    if ((get_distance(neighbours[i]) == wanted_dist && get_distance(neighbours[i + half_size]) == wanted_dist) && distinct_sets(ctx, ctx.x, ctx.y, i, i + half_size, wanted_dist)) {
     return true;
    }
  }
  return false;
}

bool ConvexHull::edge_center(uint8_t current_state, const RuleContext& ctx, const std::vector<uint8_t>& neighbours) const {
  uint8_t dist_x = get_distance(current_state);
  auto deltas = pick_deltas(ctx.neighborhood);
  const auto& gridVals = ctx.getGrid().getGridValues();
  
  for (std::size_t i = 0; i < deltas.size(); ++i) {
    int nx = static_cast<int>(ctx.x) + deltas[i].first;
    int ny = static_cast<int>(ctx.y) + deltas[i].second;
    if (nx < 0 || ny < 0 || nx >= static_cast<int>(ctx.getGrid().getWidth()) || ny >= static_cast<int>(ctx.getGrid().getHeight())) {
      continue; // out of bounds
    }
    uint8_t neighbor_state = gridVals[ny * ctx.getGrid().getWidth() + nx];
    uint8_t dist_y = get_distance(neighbor_state);
    if (dist_y != dist_x) {
      continue;
    }
    uint8_t wanted_dist = get_distance(retreat_distance(current_state)); 
    auto y_neighbors = ctx.getNeighbors(static_cast<std::size_t>(nx), static_cast<std::size_t>(ny));

    // neihbours are anticlockwise starting from the top middle for moore and from the top for vonneumann 
    for (std::size_t j = 0; j < neighbours.size(); ++j) {
      if (get_distance(y_neighbors[j]) == wanted_dist) {
        std::size_t opposite_index = (j + (neighbours.size() / 2)) % neighbours.size();
        if (get_distance(neighbours[opposite_index]) == wanted_dist) {
          if (distinct_sets(ctx, nx, ny, opposite_index, j, wanted_dist)) {
            return true;
          } 
        }
      }
    }
  }
  return false;
}

bool ConvexHull::back_mark(uint8_t current_state, const std::vector<uint8_t>& neighbours) const {
  uint8_t wanted_value = create_cell(false, true, get_distance(advance_distance(current_state))); // not seed, marked, distance advanced by one
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


