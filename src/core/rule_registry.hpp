#pragma once

#include <functional>
#include <memory>
#include "rule.hpp"

// Central storage for all available rules (basically a factory + registry)
// Used to decouple rule creation from the rest of the app
// Whenever a new rule is via AutoRegisterRule make sure it is included in main.cpp directly or indirectly (e.g. via a all_rules.hpp)
class RuleRegistry {
public:
  using RuleCreator = std::function<std::unique_ptr<Rule>()>;

  // Singleton access (one global registry for the whole app)
  static RuleRegistry& getInstance() {
    static RuleRegistry instance; // lazy init, also ensures correct init order
    return instance;
  }

  // Registers a rule under a key
  // key = unique identifier, description = UI/help text
  void addRule(const std::string& key, const std::string& description, RuleCreator creator) {
    entries_.emplace(std::move(key), Entry{std::move(description), std::move(creator)});
  }

  struct EntryView { std::string key, description; };

  // Returns lightweight list for UI (avoids exposing creators)
  std::vector<EntryView> list() const {
      std::vector<EntryView> out;
      out.reserve(entries_.size());
      for (auto const& [k, e] : entries_) out.push_back({k, e.description});
      return out;
  }

  // Get metadata for a rule (throws if not found)
  const EntryView get(const std::string& key) const {
      auto it = entries_.find(key);
      if (it == entries_.end()) throw std::runtime_error("Unknown rule: " + key);
      return EntryView{key, it->second.description};
  }

  // Factory: create a new rule instance by key
  std::unique_ptr<Rule> make(const std::string& key) const {
      auto it = entries_.find(key);
      if (it == entries_.end()) throw std::runtime_error("Unknown rule: " + key);
      return it->second.creator(); // fresh instance each time (rules assumed stateless)
  }

private:
  struct Entry { std::string description; RuleCreator creator; };
  std::unordered_map<std::string, Entry> entries_;
};

// Helper for static auto-registration of rules
// Typically used as a static member inside rule classes
template<class T>
class AutoRegisterRule {
public: 
  AutoRegisterRule(const std::string& key, const std::string& description) {
    // Runs during static initialization → registers rule before main()
    RuleRegistry::getInstance().addRule(key, description, []() {
      return std::make_unique<T>();
    });
  }
};
