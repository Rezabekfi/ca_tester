#include "grid.hpp"

Grid::Grid(std::size_t width, std::size_t height, uint8_t default_state, Boundary boundary, Neighborhood neighborhood)
  : width_(width), height_(height), boundary_(boundary), neighborhood_(neighborhood) {
  cells_.resize(width * height, default_state);
}

void Grid::step(const Rule& rule) {
  new_cells_ = cells_;
  for (std::size_t y = 0; y < height_; ++y) {
    for (std::size_t x = 0; x < width_; ++x) {
      std::vector<uint8_t> neighbors = getNeighbors(x, y);
      uint8_t current_state = cells_[idx(x, y, width_)];
      new_cells_[idx(x, y, width_)] = rule.apply(current_state, neighbors);
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

const std::vector<uint8_t>& Grid::getGridValues() const {
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

std::vector<uint8_t> Grid::getNeighbors(std::size_t x, std::size_t y) const {
  std::vector<uint8_t> neighbors;
  // TODO: add switch for different neighborhood types (for now only one will be implemented)
  
  // we always want to take the points anticlockwise from the top 
  int dxs[8] = {0, -1, -1, -1, 0, 1, 1, 1};
  int dys[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
  for (int i = 0; i < 8; ++i) {
    // wrap for all for now
    std::size_t nx = (x + dxs[i] + width_) % width_;
    std::size_t ny = (y + dys[i] + height_) % height_;
    neighbors.push_back(cells_[idx(nx, ny, width_)]);
  }
  return neighbors;
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
  switch (neighborhood) {
    case Neighborhood::Moore: {
      neighbors.clear();
      int dxs[8] = {0, -1, -1, -1, 0, 1, 1, 1};
      int dys[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
      for (int i = 0; i < 8; ++i) {
        std::size_t nx = (x + dxs[i] + width) % width;
        std::size_t ny = (y + dys[i] + height) % height;
        neighbors.push_back(cells[idx(nx, ny, width)]);
      }
      return neighbors;
    }
    case Neighborhood::VonNeumann: {
      neighbors.clear();
      int dxs[4] = {0, -1, 1, 0};
      int dys[4] = {-1, 0, 0, 1};
      for (int i = 0; i < 4; ++i) {
        std::size_t nx = (x + dxs[i] + width) % width;
        std::size_t ny = (y + dys[i] + height) % height;
        neighbors.push_back(cells[idx(nx, ny, width)]);
      }
      return neighbors;
    }
    default:
      throw std::invalid_argument("Unsupported neighborhood type in getNeighborsStatic");
  }
}


