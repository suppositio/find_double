#include <exception>
#include <filesystem>
#include <iostream>

#include "Duplicate_finder.hpp"

int main(int argc, char* argv[]){
    auto start_path = argc>1?fs::path(argv[1]):fs::current_path();
    try {
        suppositio::Duplicate_finder df(start_path);
        df.process();
        std::cout<<df;
    } catch (const std::exception& ex){
        std::cerr<<ex.what();
        return 1;
    }
    return 0;
}