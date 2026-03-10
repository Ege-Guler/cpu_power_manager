#include "Sysfs.hpp"


namespace Sysfs
{

    std::string read(const std::string &path)
    {
        std::ifstream file(path);

        if (!file.is_open()){
            std::cerr << "Error: Could not open file " << path << std::endl;
            return "";
        }

        std::stringstream ss;
        ss << file.rdbuf();

        std::string result = ss.str();

        // remove trailing newline
        if (!result.empty() && result.back() == '\n')
            result.pop_back();

        return result;
    }

}