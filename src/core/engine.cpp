#include "engine.hpp"
#include <iostream>
#include <thread>

// TODO: fix magical constants
Engine::Engine(std::size_t width, std::size_t height, std::string rule_key) 
  : grid_(width, height), rule_(RuleRegistry::getInstance().make(rule_key)), speed_(1.0), elapsed_time_(0.0), iteration_(0) {
  history_.emplace_back(grid_.getGridValues());
}

Engine::Engine(std::size_t width, std::size_t height, uint8_t default_state, Boundary boundary, Neighborhood neighborhood, std::string rule_key)
  : grid_(width, height, default_state, boundary, neighborhood), rule_(RuleRegistry::getInstance().make(rule_key)), speed_(1.0), elapsed_time_(0.0), iteration_(0) {
  history_.emplace_back(grid_.getGridValues()); 
}

void Engine::step() {
  {
    std::lock_guard<std::mutex> lock(mtx_);
    if (iteration_.load(std::memory_order_relaxed) == 0) {
      history_.clear();
      history_.emplace_back(grid_.getGridValues());
    }
    if (calculating_distances_.load(std::memory_order_relaxed) && distance_calculator_) {
      distance_calculator_(grid_.getGridValues(), grid_.getWidth(), grid_.getHeight(), grid_.getNeighborhood(), grid_.getBoundary());
    }
    grid_.step(*rule_);
    history_.emplace_back(grid_.getGridValues());
  }
  iteration_.fetch_add(1, std::memory_order_relaxed);
}


void Engine::start() {
  if (worker_.joinable()) return;
  
  worker_ = std::jthread([this](std::stop_token stoken) {
    while (true) {
      if (stoken.stop_requested()) break;
      step();
      double speed = speed_.load(std::memory_order_relaxed);
      if (speed <= 0.0) speed = 1.0;
      std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1.0 / speed * 1000))); // TODO: check how fast this is and maybe change later
    }
  });
   
}

void Engine::stop() {
  if (worker_.joinable()) {
    worker_.request_stop();
    worker_.join();
  }
}

void Engine::toggleRunning() {
  (worker_.joinable()) ? stop() : start();
}

void Engine::reset() {
  stop();
  {
    std::lock_guard<std::mutex> lock(mtx_);
    if (history_.empty()) return;
    std::vector<uint8_t> initial_state = history_.front();
    grid_.setGridValues(initial_state);
    history_.clear();
    history_.emplace_back(initial_state);
    iteration_.store(0, std::memory_order_relaxed);
  }
}

void Engine::clean() {
  stop();
  {
    std::lock_guard<std::mutex> lock(mtx_);
    std::vector<uint8_t> empty_state(grid_.getWidth() * grid_.getHeight(), 0);
    history_.pop_back();
    grid_.setGridValues(empty_state);
    history_.emplace_back(empty_state);
  }
}

Grid& Engine::getGrid() {
  return grid_;
}


void Engine::setSpeed(double speed) {
  if (speed < 0.0) speed = 0.0; // TODO: decide if I should allow 0 but it is double so compare carefully
  speed_.store(speed, std::memory_order_relaxed);
}

double Engine::getSpeed() const {
  return speed_.load(std::memory_order_relaxed);
}

std::size_t Engine::getIteration() const {
  return iteration_.load(std::memory_order_relaxed);
}

const std::vector<std::vector<uint8_t>>& Engine::getHistory() {
  std::lock_guard<std::mutex> lock(mtx_);
  return history_;
}

const std::vector<uint8_t>& Engine::getStateAtIteration(std::size_t iteration) {
  std::lock_guard<std::mutex> lock(mtx_);
  if (iteration < history_.size()) {
    return history_[iteration];
  }
  throw std::out_of_range("Iteration out of range in getStateAtIteration");
}

void Engine::setGridValues(const std::vector<uint8_t>& new_grid_values) {
  std::lock_guard<std::mutex> lock(mtx_);
  grid_.setGridValues(new_grid_values);
  history_.emplace_back(new_grid_values);
}

bool Engine::goToIteration(std::size_t iteration) {
  std::lock_guard<std::mutex> lock(mtx_);
  if (iteration < history_.size()) {
    grid_.setGridValues(history_[iteration]);
    iteration_.store(iteration, std::memory_order_relaxed);
    return true;
  }
  return false;
}

void Engine::stepBack(std::size_t steps) {
  if (steps == 0) return;
  if (steps > iteration_.load(std::memory_order_relaxed)) {
    reset();
  } else {
    goToIteration(iteration_.load(std::memory_order_relaxed) - steps);
  }
}

void Engine::resizeGrid(std::size_t new_width, std::size_t new_height) {
  stop();
  {
    std::lock_guard<std::mutex> lock(mtx_);
    grid_.resize(new_width, new_height);
    history_.clear();
    history_.emplace_back(grid_.getGridValues());
    iteration_.store(0, std::memory_order_relaxed);
  }
}

void Engine::setNeighborhood(Neighborhood neighborhood) {
  std::lock_guard<std::mutex> lock(mtx_);
  grid_.setNeighborhood(neighborhood);
}

void Engine::setBoundary(Boundary boundary) {
  std::lock_guard<std::mutex> lock(mtx_);
  grid_.setBoundary(boundary);
}

void Engine::setRule(std::unique_ptr<Rule> rule) {
  std::lock_guard<std::mutex> lock(mtx_);
  rule_ = std::move(rule);
}

void Engine::setCalculatingDistances(bool calculating) {
  calculating_distances_.store(calculating, std::memory_order_relaxed);
}

Rule& Engine::getRule() const {
  return *rule_;
}

bool Engine::isCalculatingDistances() const {
  return calculating_distances_.load(std::memory_order_relaxed);
}

void Engine::setDistanceCalculator(std::function<void(std::vector<uint8_t>&, std::size_t, std::size_t, Neighborhood, Boundary)> calculator) {
  std::lock_guard<std::mutex> lock(mtx_);
  distance_calculator_ = calculator;
}

