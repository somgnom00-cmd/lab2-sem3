#include "container.h"
#include "database.h"

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <limits>
#include <stdexcept>

namespace {
constexpr char binaryMagic[] = "LAB3BIN1";
constexpr char textMagic[] = "LAB3TXT1";
constexpr std::uint32_t maxSerializedString = 16U * 1024U * 1024U;
constexpr std::uint32_t maxSerializedItems = 1'000'000U;

void writeString(std::ostream &output, const std::string &value) {
    if (value.size() > maxSerializedString) {
        throw std::runtime_error("строка слишком велика для сериализации");
    }
    const auto length = static_cast<std::uint32_t>(value.size());
    output.write(reinterpret_cast<const char *>(&length), sizeof(length));
    output.write(value.data(), static_cast<std::streamsize>(value.size()));
}

std::string readString(std::istream &input) {
    std::uint32_t length = 0;
    input.read(reinterpret_cast<char *>(&length), sizeof(length));
    if (!input || length > maxSerializedString) {
        throw std::runtime_error("повреждённый бинарный файл");
    }
    std::string value(length, '\0');
    input.read(value.data(), static_cast<std::streamsize>(length));
    if (!input)
        throw std::runtime_error("повреждённый бинарный файл");
    return value;
}
} // namespace

void Container::saveText(const std::filesystem::path &file) const {
    std::ofstream output(file);
    if (!output)
        throw std::runtime_error("не удалось открыть текстовый файл");

    const auto stored = values();
    output << textMagic << '\n' << std::quoted(type()) << '\n' << stored.size() << '\n';
    for (const auto &value : stored)
        output << std::quoted(value) << '\n';
    if (!output)
        throw std::runtime_error("ошибка записи текстового файла");
}

void Container::loadText(const std::filesystem::path &file) {
    std::ifstream input(file);
    if (!input)
        throw std::runtime_error("не удалось открыть текстовый файл");

    std::string magic;
    std::string storedType;
    std::size_t count = 0;
    input >> magic >> std::quoted(storedType) >> count;
    if (!input || magic != textMagic || storedType != type() || count > maxSerializedItems) {
        throw std::runtime_error("неверный формат или тип текстового файла");
    }

    std::vector<std::string> stored;
    stored.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        std::string value;
        input >> std::quoted(value);
        if (!input)
            throw std::runtime_error("повреждённый текстовый файл");
        stored.push_back(std::move(value));
    }

    restore(stored);
}

void Container::saveBinary(const std::filesystem::path &file) const {
    std::ofstream output(file, std::ios::binary);
    if (!output)
        throw std::runtime_error("не удалось открыть бинарный файл");

    output.write(binaryMagic, sizeof(binaryMagic) - 1);
    writeString(output, type());
    const auto stored = values();
    if (stored.size() > maxSerializedItems) {
        throw std::runtime_error("слишком много элементов для сериализации");
    }
    const auto count = static_cast<std::uint32_t>(stored.size());
    output.write(reinterpret_cast<const char *>(&count), sizeof(count));
    for (const auto &value : stored)
        writeString(output, value);
    if (!output)
        throw std::runtime_error("ошибка записи бинарного файла");
}

void Container::loadBinary(const std::filesystem::path &file) {
    std::ifstream input(file, std::ios::binary);
    if (!input)
        throw std::runtime_error("не удалось открыть бинарный файл");

    char magic[sizeof(binaryMagic) - 1]{};
    input.read(magic, sizeof(magic));
    if (!input || std::string(magic, sizeof(magic)) != binaryMagic) {
        throw std::runtime_error("неверный формат бинарного файла");
    }
    const std::string storedType = readString(input);
    if (storedType != type())
        throw std::runtime_error("тип контейнера в файле не совпадает");

    std::uint32_t count = 0;
    input.read(reinterpret_cast<char *>(&count), sizeof(count));
    if (!input || count > maxSerializedItems)
        throw std::runtime_error("повреждённый бинарный файл");

    std::vector<std::string> stored;
    stored.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i)
        stored.push_back(readString(input));

    restore(stored);
}


void Container::restore(const std::vector<std::string> &stored) {
    auto next = makeContainer(type());
    for (const auto &value : stored)
        next->appendSerialized(value);
    swapContents(*next);
}
