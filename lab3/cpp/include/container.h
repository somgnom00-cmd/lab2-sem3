#pragma once

#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

// Общий полиморфный интерфейс контейнеров ЛР3.
class Container {
  public:
    virtual ~Container() = default;

    virtual std::string type() const = 0;
    virtual std::size_t size() const noexcept = 0;
    virtual std::vector<std::string> values() const = 0;
    virtual void clear() noexcept = 0;

    void saveText(const std::filesystem::path &file) const;
    void loadText(const std::filesystem::path &file);
    void saveBinary(const std::filesystem::path &file) const;
    void loadBinary(const std::filesystem::path &file);

  private:
    void restore(const std::vector<std::string> &stored);

  protected:
    virtual void swapContents(Container &other) noexcept = 0;
    virtual void appendSerialized(const std::string &value) = 0;
};
