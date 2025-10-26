#include "grid.hpp"
#include "rule_context.hpp"

Grid::Grid(std::size_t width, std::size_t height, uint8_t default_state, Boundary boundary, Neighborhood neighborhood)
  : width_(width), height_(height), boundary_(boundary), neighborhood_(neighborhood) {
  cells_.resize(width * height, default_state);
}


void Grid::step(const Rule& rule) {
  new_cells_ = cells_;
  RuleContext ctx{*this, 0, 0, neighborhood_, boundary_};

  for (std::size_t y = 0; y < height_; ++y) {
    for (std::size_t x = 0; x < width_; ++x) {
      ctx.x = x;
      ctx.y = y;
      std::vector<uint8_t> neighbours =
          getNeighborsStatic(cells_, x, y, width_, height_, neighborhood_, boundary_);
      uint8_t current_state = cells_[idx(x, y, width_)];
      new_cells_[idx(x, y, width_)] = rule.apply(current_state, ctx, neighbours);
    }
  }
  cells_ = new_cells_;
}

void Grid::setCell(std::size_t x, std::size_t y, uint8_t state) {
  if (x < width_ && y < height_) {
    cells_[idx(x, y, width_)] = state;
  }
}

uint8_t Grid::getCell(std::size_t x, std::size_t y) const {
  if (x < width_ && y < height_) {
    return cells_[idx(x, y, width_)];
  }
  return 0; // or some error value
}

std::vector<uint8_t>& Grid::getGridValues() {
  return cells_;
}

void Grid::setGridValues(const std::vector<uint8_t>& values) {
  if (values.size() == cells_.size()) {
    cells_ = values;
  }
}

std::size_t Grid::getWidth() const {
  return width_;
}

std::size_t Grid::getHeight() const {
  return height_;
}

void Grid::resize(std::size_t new_width, std::size_t new_height) {
  width_ = new_width;
  height_ = new_height;
  cells_.resize(new_width * new_height, 0);
}

void Grid::setBoundary(Boundary boundary) {
  boundary_ = boundary;
}

void Grid::setNeighborhood(Neighborhood neighborhood) {
  neighborhood_ = neighborhood;
}

std::vector<uint8_t> Grid::getNeighborsStatic(const std::vector<uint8_t>& cells, std::size_t x, std::size_t y, std::size_t width, std::size_t height, Neighborhood neighborhood, Boundary boundary) {
  std::vector<uint8_t> neighbors;
  auto deltas = pick_deltas(neighborhood);
  for (const auto& delta : deltas) {
    int nx = static_cast<int>(x) + delta.first;
    int ny = static_cast<int>(y) + delta.second;
    switch (boundary) {
      case Boundary::Wrap:
        nx = (nx + static_cast<int>(width)) % static_cast<int>(width);
        ny = (ny + static_cast<int>(height)) % static_cast<int>(height);
        break;
      case Boundary::Zero:
        if (nx < 0 || nx >= static_cast<int>(width) || ny < 0 || ny >= static_cast<int>(height)) {
          neighbors.push_back(0);
          continue;
        }
        break;
      case Boundary::Reflect:
        if (nx < 0) nx = 0;
        if (nx >= static_cast<int>(width)) nx = static_cast<int>(width) - 1;
        if (ny < 0) ny = 0;
        if (ny >= static_cast<int>(height)) ny = static_cast<int>(height) - 1;
        break;
      case Boundary::One:
        if (nx < 0 || nx >= static_cast<int>(width) || ny < 0 || ny >= static_cast<int>(height)) {
          neighbors.push_back(1);
          continue;
        }
        break;
      default:
        // TODO: handle error for now we just dont add neighbours which should technically work but shouldnt happen
        continue;
    }
    neighbors.push_back(cells[idx(static_cast<std::size_t>(nx), static_cast<std::size_t>(ny), width)]);
  }
  return neighbors;
}

Boundary Grid::getBoundary() const {
  return boundary_;
}

Neighborhood Grid::getNeighborhood() const {
  return neighborhood_;
}

const std::vector<uint8_t>& Grid::getGridValues() const {
  return cells_;
}
