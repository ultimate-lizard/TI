#include "File.h"

#include <fstream>
#include <streambuf>

std::string readFileToString(const std::string& path)
{
    std::ifstream file(path);
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}
