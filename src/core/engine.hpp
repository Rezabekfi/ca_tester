#pragma once 

#include "grid.hpp"
#include "rule.hpp"
#include "rule_registry.hpp"
#include <mutex>
#include <atomic>
#include <thread>
#include <functional>
#include <memory>

// Manages simulation lifecycle (threading, stepping, history, etc.)
// Acts as bridge between UI and core grid/rule logic
class Engine {
public:
  // Basic constructor: creates grid + rule from registry key
  Engine(std::size_t width, std::size_t height, std::string rule_key);

  // Full config constructor (used when restoring or customizing setup)
  Engine(std::size_t width, std::size_t height, uint8_t default_state, Boundary boundary, Neighborhood neighborhood, std::string rule_key); 

  // Advances one step (thread-safe wrapper around Grid::step)
  void step();

  // Starts background simulation loop (uses jthread)
  void start();

  // Stops simulation loop
  void stop();

  // Convenience toggle for UI
  void toggleRunning();

  // Reset simulation to initial state (clears history and resets iteration counter) but keeps users 0th iteration meaning the drawn input
  void reset();

  // Clears grid state (but keeps config) gets rid of everything in the grid
  void clean();

  // Direct grid access (mutable → caller must sync if needed)
  Grid& getGrid();

  // Returns a copy (safe but potentially expensive)
  Grid getGridConst() const;

  // Control simulation speed (steps per second)
  void setSpeed(double speed);
  double getSpeed() const;

  // Access current rule (owned by engine)
  Rule& getRule() const;

  // Current iteration counter (mirrors grid but tracked separately)
  std::size_t getIteration() const;

  // Full history of grid states (can get big fast so be careful of that, but it isnt usually an issue)
  const std::vector<std::vector<uint8_t>>& getHistory();

  // Access specific snapshot
  const std::vector<uint8_t>& getStateAtIteration(std::size_t iteration);

  // Replace current grid state
  void setGridValues(const std::vector<uint8_t>& new_grid_values);

  // Runtime config changes
  void setNeighborhood(Neighborhood neighborhood);
  void setBoundary(Boundary boundary);

  // Swap rule dynamically
  void setRule(std::unique_ptr<Rule> rule);

  // Toggle special mode (likely modifies how step behaves)
  void setCalculatingDistances(bool calculating);
  bool isCalculatingDistances() const;

  // Clears any stored distance data
  void resetDistances();

  // Navigate simulation history backwards
  void stepBack(std::size_t steps = 1);

  // Jump to specific iteration if available in history
  bool goToIteration(std::size_t iteration);

  // Inject custom distance computation (pluggable behavior) usef only for convex hull for now
  // WARNING: here is an design issue: rules currently dont carry any information if they need any distance calculation or not
  // so forexapmle for convex hull there is in renderer a check if the rule is convex. This is certainly a design flaw so if any progress will be made on this in the future this is a first task on the todo list.
  void setDistanceCalculator(std::function<void(std::vector<uint8_t>&, std::size_t, std::size_t, Neighborhood, Boundary)> calculator);
  
  // Resize grid (likely resets or invalidates history)
  void resizeGrid(std::size_t new_width, std::size_t new_height);

  ~Engine() = default;

private:
  std::jthread worker_; // background loop (auto-joins on destruction)

  std::mutex mtx_; // protects grid/history during concurrent access

  Grid grid_;
  std::unique_ptr<Rule> rule_; // active rule instance

  std::atomic<double> speed_;        // steps/sec
  std::atomic<double> elapsed_time_; // for timing loop

  std::atomic<std::size_t> iteration_; // global iteration counter

  std::vector<std::vector<uint8_t>> history_; // stores past states (memory-heavy)

  std::atomic<bool> calculating_distances_{false}; // mode flag

  // Optional injected algorithm (keeps engine flexible without hardcoding logic)
  std::function<void(std::vector<uint8_t>&, std::size_t, std::size_t, Neighborhood, Boundary)> distance_calculator_;
};
