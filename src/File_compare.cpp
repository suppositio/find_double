#include "File_compare.hpp"

#include <cstring>
#include <fstream>
#include <system_error>

namespace suppositio {

File_compare::File_compare (size_t buff_size) :
    buff_size_{buff_size},
    buff_1_{std::make_unique<char[]>(buff_size_)},
    buff_2_{std::make_unique<char[]>(buff_size_)} {}

std::optional<bool> File_compare::operator() (const fs::directory_entry& dir_entry1, const fs::directory_entry& dir_entry2) const {
    std::error_code error1;
    std::error_code error2;
    F_size f_size1 = dir_entry1.file_size(error1);
    F_size f_size2 = dir_entry2.file_size(error2);

    if (error1||error2){
        return std::nullopt;
    }

    if (f_size1!=f_size2){
        return std::make_optional<bool>(false);
    }

    if (f_size1==0){
        return std::make_optional<bool>(true);
    }

    std::ifstream file1;
    std::ifstream file2;

    file1.open(dir_entry1.path().c_str(), std::ios::binary);
    file2.open(dir_entry2.path().c_str(), std::ios::binary);

    if (!file1||!file2){
        return std::nullopt;
    }

    size_t tail_size = f_size1%buff_size_;
    size_t block_count = f_size1/buff_size_+(tail_size!=0);

    while (block_count) {
        size_t read_size = (block_count==1)&&(tail_size!=0)?tail_size:buff_size_;
        file1.read(buff_1_.get(), read_size);
        file2.read(buff_2_.get(), read_size);
        if (std::memcmp(buff_1_.get(), buff_2_.get(), read_size)){
            return std::make_optional<bool>(false);
        }
        --block_count;
    }
    return std::make_optional<bool>(true);
}

} // namespace suppositio
