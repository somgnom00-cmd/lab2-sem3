#pragma once

#include "container.h"

#include <map>
#include <memory>
#include <string>

std::unique_ptr<Container> makeContainer(const std::string &type);

class Database {
  public:
    Container &create(const std::string &name, const std::string &type);
    Container &get(const std::string &name);
    const Container &get(const std::string &name) const;
    bool erase(const std::string &name) noexcept;
    std::size_t size() const noexcept { return items_.size(); }

  private:
    std::map<std::string, std::unique_ptr<Container>> items_;
};
