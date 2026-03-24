#include "grid.hpp"
#include "rule_context.hpp"
#include <thread>
#include <vector>
#include <algorithm>

Grid::Grid(std::size_t width, std::size_t height, uint8_t default_state, Boundary boundary, Neighborhood neighborhood)
  : width_(width), height_(height), boundary_(boundary), neighborhood_(neighborhood) {
  cells_.resize(width * height, default_state);
}

void Grid::step(const Rule& rule) {
  if (new_cells_.size() != cells_.size()) {
    new_cells_.resize(cells_.size());
  }

  std::size_t thread_count = std::thread::hardware_concurrency();
  if (thread_count == 0) {
    thread_count = 1;
  }

  thread_count = std::min(thread_count, height_);
  const std::size_t rows_per_thread = (height_ + thread_count - 1) / thread_count;

  std::vector<std::thread> threads;
  threads.reserve(thread_count);

  for (std::size_t t = 0; t < thread_count; ++t) {
    const std::size_t y_begin = t * rows_per_thread;
    const std::size_t y_end = std::min(y_begin + rows_per_thread, height_);

    if (y_begin >= y_end) {
        break;
    }

    threads.emplace_back([&, y_begin, y_end]() {
      // Each thread gets its own context
      RuleContext ctx{*this, 0, 0, neighborhood_, boundary_};
      std::vector<uint8_t> neighbors; // Reusable vector for neighbors
      neighbors.reserve(8); // TODO: change later this is not always 8 but for now good enough

      for (std::size_t y = y_begin; y < y_end; ++y) {
        const std::size_t row = y * width_;

        for (std::size_t x = 0; x < width_; ++x) {
          ctx.x = x;
          ctx.y = y;

          getNeighborsStatic(cells_, x, y, width_, height_, neighborhood_, boundary_, neighbors);

          const std::size_t i = row + x;
          const uint8_t current_state = cells_[i];
          new_cells_[i] = rule.apply(current_state, ctx, neighbors);
        }
      }
    });
  }
  for (auto& th : threads) {
      th.join();
  }

  // 2) swap instead of copying back
  cells_.swap(new_cells_);
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

void Grid::getNeighborsStatic(const std::vector<uint8_t>& cells, std::size_t x, std::size_t y, std::size_t width, std::size_t height, Neighborhood neighborhood, Boundary boundary, std::vector<uint8_t>& neighbors) {
  neighbors.clear();
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

std::size_t Grid::getIteration() const {
  return iteration_;
}
void Grid::setIteration(std::size_t iteration) {
  iteration_ = iteration;
}

void Grid::setHeight(std::size_t height) {
  height_ = height;
}

void Grid::setWidth(std::size_t width) {
  width_ = width;
}



