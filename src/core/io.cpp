#include "io.hpp"
#include <fstream>
#include <exception>
#include "json.hpp"
#include <filesystem>
#include "convex_hull/convex_hull.hpp"

// Saves the current grid state and settings to a JSON file. Returns true on success, false on failure.
// This is currently a bit "hardcoded" but for the app it is for now good enough. In the future this might be a place to look at
bool IO::saveGridToFile(const Engine& engine, const std::string& filename, bool use_default_folder) {
  auto grid = engine.getGridConst();
  nlohmann::json j;
  j["width"] = grid.getWidth();
  j["height"] = grid.getHeight();
  j["grid_values"] = grid.getGridValues();
  j["rule"] = engine.getRule().getName();
  j["neighborhood"] = static_cast<int>(grid.getNeighborhood());
  j["boundary"] = static_cast<int>(grid.getBoundary());
  std::string full_path = filename;

  try {
    if (use_default_folder) {
      std::filesystem::create_directories(DEFAULT_SAVE_FOLDER);
      full_path = DEFAULT_SAVE_FOLDER + filename;
    }
    std::ofstream file(full_path);
    file.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    file << j.dump(4); // pretty print with 4 spaces indentation
    file.close();
  } catch (const std::exception& e) {
    return false;
  }
  return true;
}

// Loads the grid state and settings from a JSON file. Returns true on success, false on failure.
// Same issue with this as mentioned above
bool IO::loadGridFromFile(Engine& engine, const std::string& filename) {
  nlohmann::json j;
  try {
    std::ifstream file(filename);
    file >> j;
    file.close();
  } catch (const std::exception& e) {
    return false;
  }
  try {
    engine.reset();
    std::size_t width = j.at("width").get<std::size_t>();
    std::size_t height = j.at("height").get<std::size_t>();
    std::vector<uint8_t> grid_values = j.at("grid_values").get<std::vector<uint8_t>>();
    std::string rule_name = j.at("rule").get<std::string>();
    Neighborhood neighborhood = static_cast<Neighborhood>(j.at("neighborhood").get<int>());
    Boundary boundary = static_cast<Boundary>(j.at("boundary").get<int>());

    engine.resizeGrid(width, height);
    engine.setGridValues(grid_values);
    engine.setNeighborhood(neighborhood);
    engine.setBoundary(boundary);
    engine.setRule(RuleRegistry::getInstance().make(rule_name));

    // TODO: handle this differently
    if (rule_name == CONVEX_HULL_RULE_NAME) {
      engine.setCalculatingDistances(true);
      engine.setDistanceCalculator(ConvexHull::calculateDistances);
    } else {
      engine.setCalculatingDistances(false);
      engine.resetDistances();
    }


  } catch (const std::exception& e) {
    return false;
  }
  return true;
}
