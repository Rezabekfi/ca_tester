#pragma once 

#include "grid.hpp"
#include "rule.hpp"

class Engine {
public:
  Engine(std::size_t width, std::size_t height, Rule rule);
  Engine(std::size_t width, std::size_t height, uint8_t default_state, Boundary boundary, Neighborhood neighborhood, Rule rule);

  void step();
  void start();
  void pause();
  void reset();

  Grid& getGrid();
  bool isRunning() const;
  void setSpeed(double speed); // speed in steps per second
  double getSpeed() const;
  std::size_t getIteration() const;
  const std::vector<std::vector<uint8_t>>& getHistory() const;
  const std::vector<uint8_t>& getStateAtIteration(std::size_t iteration) const;
  void setGridValues(const std::vector<uint8_t>& new_grid_values);

  ~Engine() = default;

private:
  Grid grid_;
  Rule rule_;
  bool running_;
  double speed_; // steps per second
  double elapsed_time_; // time accumulator for stepping
  std::size_t iteration_;
  std::vector<std::vector<uint8_t>> history_;
};
