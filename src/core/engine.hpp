#pragma once 

#include "grid.hpp"
#include "rule.hpp"
#include <mutex>
#include <atomic>
#include <thread>


class Engine {
public:
  Engine(std::size_t width, std::size_t height, Rule& rule);
  Engine(std::size_t width, std::size_t height, uint8_t default_state, Boundary boundary, Neighborhood neighborhood, Rule& rule);

  void step();
  void start();
  void stop();
  void toggleRunning();
  void reset();

  Grid& getGrid();
  void setSpeed(double speed); // speed in steps per second
  double getSpeed() const;
  std::size_t getIteration() const;
  const std::vector<std::vector<uint8_t>>& getHistory();
  const std::vector<uint8_t>& getStateAtIteration(std::size_t iteration);
  void setGridValues(const std::vector<uint8_t>& new_grid_values);

  ~Engine() = default;

private:
  std::jthread worker_;
  std::mutex mtx_;
  Grid grid_;
  Rule& rule_;
  std::atomic<double> speed_;
  std::atomic<double> elapsed_time_;
  std::atomic<std::size_t> iteration_;
  std::vector<std::vector<uint8_t>> history_;
};
