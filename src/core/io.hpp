#pragma once

#include "engine.hpp"

constexpr std::string DEFAULT_SAVE_FOLDER = "saves/";

// Singleton class for handling input/output of the grid state + other usefull information to/from json files

/*
* JSON strucutre:
* width: <grid width>
* height: <grid height>
* grid_values: [<list of grid values>]
* rule: <rule name>
* neighborhood: <neighborhood type>
* boundary: <boundary type>
*/

constexpr bool USE_DEFAULT_SAVE_FOLDER = true;

class IO {
public: 
  static IO& instance() {
    static IO instance;
    return instance;
  }

  bool saveGridToFile(const Engine& engine, const std::string& filename, bool use_default_folder = USE_DEFAULT_SAVE_FOLDER);
  bool loadGridFromFile(Engine& engine, const std::string& filename);

private:
  IO() = default;

};
