#include "Sysfs.hpp"

namespace Sysfs
{

    std::string read(const std::string &path)
    {
        std::ifstream file(path);

        if (!file.is_open())
        {
            std::cerr << "Error: Could not open file " << path << "     for reading" << std::endl;
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

    bool write(const std::string &path, const std::string &val)
    {

        if(val.empty())
        {
            std::cerr << "Error: Value to write is empty" << std::endl;
            return "";
        }

        std::string temp = val;

        if (temp.back() != '\n')
            temp.pop_back();
    
        std::ofstream file(path);

        if (!file.is_open())
        {
            std::cerr << "Error: Could not open file " << path << " for writing" << std::endl;
            return "";
        }

        file << temp << "\n";
        
        return file.good();

    }

}