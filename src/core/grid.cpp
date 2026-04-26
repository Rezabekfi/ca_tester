#include "grid.hpp"
#include "rule_context.hpp"
#include <thread>
#include <vector>
#include <algorithm>

// Creates grid storage and fills it with the default state
Grid::Grid(std::size_t width, std::size_t height, uint8_t default_state, Boundary boundary, Neighborhood neighborhood)
  : width_(width), height_(height), boundary_(boundary), neighborhood_(neighborhood) {
  cells_.resize(width * height, default_state);
}

// Advances the whole grid by one generation
void Grid::step(const Rule& rule) {
  if (new_cells_.size() != cells_.size()) {
    new_cells_.resize(cells_.size());
  }

  // Split rows across hardware threads when possible
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
      // Thread-local context avoids sharing mutable x/y between workers
      RuleContext ctx{*this, 0, 0, neighborhood_, boundary_};

      // Reused per cell to avoid reallocating neighbor storage
      std::vector<uint8_t> neighbors;
      neighbors.reserve(8); // TODO: not always 8 once wider/custom neighborhoods matter this is safe for now since we only have moore and vonneumann but change later when we add more neighborhood types

      for (std::size_t y = y_begin; y < y_end; ++y) {
        const std::size_t row = y * width_;

        for (std::size_t x = 0; x < width_; ++x) {
          ctx.x = x;
          ctx.y = y;

          getNeighborsStatic(cells_, x, y, width_, height_, neighborhood_, boundary_, neighbors);

          const std::size_t i = row + x;
          const uint8_t current_state = cells_[i];

          // Rule reads old state and writes only this cell's next state
          new_cells_[i] = rule.apply(current_state, ctx, neighbors);
        }
      }
    });
  }

  for (auto& th : threads) {
      th.join();
  }

  // Swap buffers so all cells update at the same time
  cells_.swap(new_cells_);
}

// Safe write: ignores out-of-bounds clicks/updates
void Grid::setCell(std::size_t x, std::size_t y, uint8_t state) {
  if (x < width_ && y < height_) {
    cells_[idx(x, y, width_)] = state;
  }
}

// Safe read: out-of-bounds behaves as dead
uint8_t Grid::getCell(std::size_t x, std::size_t y) const {
  if (x < width_ && y < height_) {
    return cells_[idx(x, y, width_)];
  }
  return 0; // or some error value this is might a problem in the futurue as there is no real distinction between out-of-bounds and dead cells
}

// Direct mutable access for UI/tools that need raw grid data
std::vector<uint8_t>& Grid::getGridValues() {
  return cells_;
}

// Replace whole grid only when dimensions already match
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

// Resizes storage; new cells default to dead
void Grid::resize(std::size_t new_width, std::size_t new_height) {
  width_ = new_width;
  height_ = new_height;
  cells_.resize(new_width * new_height, 0);
}

// Changes how future neighbor lookups treat edges
void Grid::setBoundary(Boundary boundary) {
  boundary_ = boundary;
}

// Changes which neighbor shape future steps use
void Grid::setNeighborhood(Neighborhood neighborhood) {
  neighborhood_ = neighborhood;
}

// Shared neighbor sampler used by Grid and RuleContext
void Grid::getNeighborsStatic(const std::vector<uint8_t>& cells, std::size_t x, std::size_t y, std::size_t width, std::size_t height, Neighborhood neighborhood, Boundary boundary, std::vector<uint8_t>& neighbors) {
  neighbors.clear();
  auto deltas = pick_deltas(neighborhood);

  for (const auto& delta : deltas) {
    int nx = static_cast<int>(x) + delta.first;
    int ny = static_cast<int>(y) + delta.second;

    switch (boundary) {
      case Boundary::Wrap:
        // Toroidal grid: leaving one side enters from the other
        nx = (nx + static_cast<int>(width)) % static_cast<int>(width);
        ny = (ny + static_cast<int>(height)) % static_cast<int>(height);
        break;

      case Boundary::Zero:
        // Outside grid behaves like dead cells
        if (nx < 0 || nx >= static_cast<int>(width) || ny < 0 || ny >= static_cast<int>(height)) {
          neighbors.push_back(0);
          continue;
        }
        break;

      case Boundary::Reflect:
        // Clamp to nearest edge cell
        if (nx < 0) nx = 0;
        if (nx >= static_cast<int>(width)) nx = static_cast<int>(width) - 1;
        if (ny < 0) ny = 0;
        if (ny >= static_cast<int>(height)) ny = static_cast<int>(height) - 1;
        break;

      case Boundary::One:
        // Outside grid behaves like alive cells
        if (nx < 0 || nx >= static_cast<int>(width) || ny < 0 || ny >= static_cast<int>(height)) {
          neighbors.push_back(1);
          continue;
        }
        break;

      default:
        // Unknown boundary: skip neighbor rather than crashing
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
