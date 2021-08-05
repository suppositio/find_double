#pragma once

#include <filesystem>
#include <map>
#include <tuple>
#include <vector>

namespace fs = std::filesystem;

namespace suppositio {

class Duplicate_finder {
public:
    using F_size = std::uintmax_t;
    using Dir_rec = std::tuple<fs::directory_entry, size_t>;
    using Dir_batch = std::vector<Dir_rec>;
    using Dir_container = std::map<F_size, Dir_batch>;
protected:
    fs::directory_entry start_dir_;
    size_t iterated_files_{0};
    Dir_container dc_;
public:
    explicit Duplicate_finder(const fs::path& start_path);
    void process();
    ~Duplicate_finder() = default;
protected:
    void collect_files();
    void remove_size_singles();
    void tag_content_doubles();
    friend std::ostream& operator<<(std::ostream& os, const Duplicate_finder& df);
 };

} // namespace suppositio