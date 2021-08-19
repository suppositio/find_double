#include "File_compare.hpp"
#include "gtest/gtest.h"

#include <filesystem>
#include <fstream>
#include <limits>
#include <optional>
#include <random>
#include <vector>

namespace fs = std::filesystem;
using ::testing::Test;

struct FileCompareTest: public Test{
    std::random_device rd;
    std::mt19937 mt;
    std::uniform_int_distribution<char> char_distr;
    std::uniform_int_distribution<size_t> size_distr;

    size_t equal_size;
    size_t unequal_size;

    std::vector<char> master_non_empty_content;
    std::vector<char> different_equal_size_content;
    std::vector<char> different_unequal_size_content; 

    fs::path test_data_path;
    fs::path master_empty;
    fs::path same_empty;
    fs::path master_non_empty;
    fs::path same_non_empty;
    fs::path different_equal_size;
    fs::path different_unequal_size;
    fs::path nonexistent1;
    fs::path nonexistent2;

    suppositio::File_compare fc;

    FileCompareTest() :
        rd(),
        mt(rd()),
        char_distr(std::numeric_limits<char>::min(), std::numeric_limits<char>::max()),
        size_distr (0x000001, 0xFFFFFF),

        equal_size(size_distr(mt)),
        unequal_size(size_distr(mt)),

        master_non_empty_content(equal_size),
        different_equal_size_content (equal_size),
        different_unequal_size_content (unequal_size), 

        test_data_path (fs::temp_directory_path() / "File_compare_test_data"),
        master_empty (test_data_path / "master_empty"),
        same_empty (test_data_path / "same_empty"),
        master_non_empty (test_data_path / "master_non_empty"),
        same_non_empty (test_data_path / "same_non_empty"),
        different_equal_size (test_data_path / "different_equal_size"),
        different_unequal_size (test_data_path / "different_unequal_size"),
        nonexistent1 (test_data_path / "nonexistent1"),
        nonexistent2 (test_data_path / "nonexistent2"),
        
        fc()
        {
            fs::create_directory(test_data_path);
        }

    void TearDown() override {
        fs::remove_all(test_data_path);
    }

    ~FileCompareTest() noexcept = default;
    
    void fill_master_non_empty_content(){
        fill_content(master_non_empty_content);
    }

    void fill_different_equal_size_content(){
        fill_content(different_equal_size_content);
    }

    void fill_different_unequal_size_content(){
        fill_content(different_unequal_size_content);
    }

    void create_master_empty(){
        create_file(master_empty);
    }

    void create_same_empty(){
        create_master_empty();
        create_file(same_empty);
    }

    void create_master_non_empty(){
        fill_master_non_empty_content();
        create_file (master_non_empty, master_non_empty_content);
    }

    void create_same_non_empty(){
        create_master_non_empty();
        create_file (same_non_empty, master_non_empty_content);
    }

    void create_different_equal_size(){
        fill_different_equal_size_content();
        create_file (different_equal_size, different_equal_size_content);       
    }

    void create_different_unequal_size(){
        fill_different_unequal_size_content();
        create_file (different_unequal_size, different_unequal_size_content);
    }

private:
    void fill_content (std::vector<char>& container){
        for (auto& elem:container){
            elem = char_distr(mt);
        }
    }

    void create_file (const fs::path& file_path, const std::vector<char>& container = std::vector<char>()){
        std::ofstream file;
        file.open(file_path, std::ios::binary);
        if (!container.empty()){
            file.write(container.data(), container.size());
        }
        file.close(); 
    }
};

TEST_F(FileCompareTest, EmptyEmpty){
    create_same_empty();
    fs::directory_entry master_empty_dir{master_empty};
    fs::directory_entry same_empty_dir{same_empty};
    auto result = fc(master_empty_dir, same_empty_dir);
    EXPECT_TRUE(*result);
}

TEST_F(FileCompareTest, EmptyNonEmpty){
    create_master_non_empty();
    create_master_empty();
    fs::directory_entry master_non_empty_dir{master_non_empty};
    fs::directory_entry master_empty_dir{master_empty};
    auto result = fc(master_non_empty_dir, master_empty_dir);    
    EXPECT_FALSE(*result);
}

TEST_F(FileCompareTest, SameNonEmpty){
    create_same_non_empty();
    fs::directory_entry master_non_empty_dir{master_non_empty};
    fs::directory_entry same_non_empty_dir{same_non_empty};
    auto result = fc(master_non_empty_dir, same_non_empty_dir);   
    EXPECT_TRUE(*result);
}

TEST_F(FileCompareTest, DifferentEqualSize){
    create_master_non_empty();
    create_different_equal_size();
    fs::directory_entry master_non_empty_dir{master_non_empty};
    fs::directory_entry different_equal_size_dir{different_equal_size};
    auto result = fc(master_non_empty_dir, different_equal_size_dir);    
    EXPECT_FALSE(*result);
}

TEST_F(FileCompareTest, DifferentUnequalSize){
    create_master_non_empty();
    create_different_unequal_size();
    fs::directory_entry master_non_empty_dir{master_non_empty};
    fs::directory_entry different_unequal_size_dir{different_unequal_size};
    auto result = fc(master_non_empty_dir, different_unequal_size_dir);    
    EXPECT_FALSE(*result);
}

TEST_F(FileCompareTest, NonexistentOne){
    create_master_non_empty();
    fs::directory_entry master_non_empty_dir{master_non_empty};
    fs::directory_entry nonexistent1_dir{nonexistent1};    
    auto result = fc(master_non_empty_dir, nonexistent1_dir); 
    EXPECT_FALSE(*result);
}

TEST_F(FileCompareTest, NonexistentBoth){
    fs::directory_entry nonexistent1_dir{nonexistent1};
    fs::directory_entry nonexistent2_dir{nonexistent2};
    auto result = fc(nonexistent1_dir, nonexistent2_dir);
    EXPECT_FALSE(result);
}
