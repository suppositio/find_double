# find_double
## About the program
This is a very basic command-line program written as an exercise, mainly to get acquainted with the features of C++ 17-20 `filesystem` library. It searches for duplicate files within a specified directory and its subdirectories and outputs the list of files with identical content to the console.
## Supported platforms
As of the current version, the program has been configured and tested for Linux desktop. Although it works on Windows, its use is limited due to lack of Unicode pathnames support.
## Building the program
You will need CMake v. 3.10 or above to build this program. The script files are so configured that the compiled executable will be placed in a `bin` subdirectory of the build directory you specify.
## Using the program
Run without parameters, the program will start its search from your current directory. To specify a different start directory, supply its path on the command line.