#pragma once

#include <functional>
#include <memory>
#include "rule.hpp"

class RuleRegistry {
public:
  using RuleCreator = std::function<std::unique_ptr<Rule>()>;

  static RuleRegistry& getInstance() {
    static RuleRegistry instance;
    return instance;
  }

  void addRule(const std::string& key, const std::string& description, RuleCreator creator) {
    entries_.emplace(std::move(key), Entry{std::move(description), std::move(creator)});
  }

  struct EntryView { std::string key, description; };
  std::vector<EntryView> list() const {
      std::vector<EntryView> out;
      out.reserve(entries_.size());
      for (auto const& [k, e] : entries_) out.push_back({k, e.description});
      return out;
  }

  const EntryView get(const std::string& key) const {
      auto it = entries_.find(key);
      if (it == entries_.end()) throw std::runtime_error("Unknown rule: " + key);
      return EntryView{key, it->second.description};
  }

  std::unique_ptr<Rule> make(const std::string& key) const {
      auto it = entries_.find(key);
      if (it == entries_.end()) throw std::runtime_error("Unknown rule: " + key);
      return it->second.creator();
  }
private:
  struct Entry { std::string description; RuleCreator creator; };
  std::unordered_map<std::string, Entry> entries_;
};

template<class T>
class AutoRegisterRule {
public: 
  AutoRegisterRule(const std::string& key, const std::string& description) {
    RuleRegistry::getInstance().addRule(key, description, []() {
      return std::make_unique<T>();
    });
  }
};
