#include "Duplicate_finder.hpp"

#include <algorithm>
#include <optional>
#include <set>
#include <stdexcept>
#include <system_error>

#include "File_compare.hpp"

namespace suppositio {

Duplicate_finder::Duplicate_finder (const fs::path& start_path)
    : start_dir_{start_path} {
        if (!start_dir_.exists()){
            throw (std::invalid_argument("Invalid start path."));
        }
}

void Duplicate_finder::collect_files() {
    fs::recursive_directory_iterator dir_it(start_dir_, fs::directory_options::skip_permission_denied);
    while (dir_it!=fs::end(dir_it)){
        std::error_code error;
        if (fs::is_regular_file(*dir_it)){
            F_size f_size = fs::file_size(*dir_it, error);
            if (!error){
                dc_[f_size].emplace_back(*dir_it, 0);
            }
            ++iterated_files_;
        }
        ++dir_it;
    }
}

void Duplicate_finder::remove_size_singles(){
    auto rem_it = dc_.begin();
    while (rem_it!=dc_.end()){
        const auto& [f_size, dir_batch] = *rem_it;
        if (dir_batch.size()<=1){
            rem_it=dc_.erase(rem_it);
        } else {
            ++rem_it;
        }
    }
}

void Duplicate_finder::tag_content_doubles(){
    File_compare file_compare;
    size_t group_counter{0};
    for (auto& [f_size, dir_batch]:dc_){
        if (f_size == 0){
            ++group_counter;
            for (auto& [file, res_tag] : dir_batch){
                res_tag=group_counter;
            }
            continue;
        }
        auto dir_rec_it1 = dir_batch.begin();
        while (dir_rec_it1!=dir_batch.end()){
            auto& [file1, res_tag1] = *dir_rec_it1; 
            if (res_tag1==0){
                auto dir_rec_it2=dir_rec_it1;
                bool first_find{true};
                while (dir_rec_it2!=dir_batch.end()){
                    dir_rec_it2=std::find_if(dir_rec_it2+1, dir_batch.end(),
                        [&file_compare, &dir_rec_it1, this](const Dir_rec& dir_rec_c){
                            const auto& [file_c, res_tag_c] = dir_rec_c;
                            const auto& [file1, res_tag_1] = *dir_rec_it1;
                            bool answer{false};
                            if (res_tag_c==0){
                                auto compare_result = file_compare(file1, file_c);
                                if (compare_result){
                                    answer = *compare_result;
                                }
                            }
                            return answer;
                        });
                    if (dir_rec_it2!=dir_batch.end()){
                        if (first_find){
                            first_find=false;
                            ++group_counter;
                            res_tag1 = group_counter;
                        }
                        auto& [file2, res_tag2] = *dir_rec_it2;
                        res_tag2 = group_counter;
                    }
                }
            }
            ++dir_rec_it1;
        }
    }
}

std::ostream& operator<<(std::ostream& os, const Duplicate_finder& df){
    std::map<size_t, std::set<fs::path>> output;
    size_t file_counter{0};
    for (const auto& [f_size, dir_batch] : df.dc_){
        for (const auto& [dir_entry, res_tag] : dir_batch){
            if (res_tag){
                output[res_tag].insert(fs::absolute(dir_entry.path()));
                ++file_counter;
            }
        }
    }
    os << std::endl <<df.iterated_files_<<" files iterated."<<std::endl;
    if (output.empty()){
        os<<"No duplicate files found in "<<fs::absolute(df.start_dir_.path())<<" and subdirectories."<<std::endl;
        return os;
    } else {
        os<<file_counter<<" duplicate files found in "<<fs::absolute(df.start_dir_.path())<<" and subdirectories."<<std::endl<<std::endl;
        for (const auto& [res_tag, group]: output){
            os << "Group "<<res_tag<<": "<<group.size()<<" files"<<std::endl<<std::endl;
            for (const auto& file_path:group){
                os<<file_path<<std::endl;
            }
            os << std::endl;
        }
    }
    return os;
}

} // namespace suppositio
