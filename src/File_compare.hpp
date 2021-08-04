#pragma once

#include <filesystem>
#include <memory>
#include <optional>

namespace fs = std::filesystem;

namespace suppositio {

class File_compare {
public:
    using F_size = std::uintmax_t;
protected:
    size_t buff_size_;
    std::unique_ptr<char[]> buff_1_;
    std::unique_ptr<char[]> buff_2_;
public:
    File_compare (size_t buff_size = 0x10000);
    std::optional<bool> operator() (const fs::directory_entry& dir_entry1, const fs::directory_entry& dir_entry2) const;
    ~File_compare() = default;
};

} // namespace suppositio