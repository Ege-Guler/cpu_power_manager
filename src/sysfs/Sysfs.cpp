#include "Sysfs.hpp"

namespace Sysfs {

std::string read(const std::string& path)
{
    std::ifstream file(path);

    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open file " + path + " for reading");
    }

    std::stringstream ss;
    ss << file.rdbuf();

    std::string result = ss.str();

    // remove trailing newline
    if (!result.empty() && result.back() == '\n')
        result.pop_back();

    return result;
}

bool write(const std::string& path, const std::string& val)
{
    if (val.empty()) {
        throw std::invalid_argument("Value to write cannot be empty");
    }

    std::ofstream file(path);

    if (!file.is_open()) {

        throw std::runtime_error("Error: Could not open file " + path +
                                 " for writing. Check if the file exists and you have the necessary permissions.");
    }

    // Write the value.
    // Most sysfs files prefer a single trailing newline.
    file << val << "\n";

    // Explicitly flush and close to ensure the kernel receives it immediately
    file.flush();

    // Check if the file operation actually succeeded
    if (!file) {

        throw std::runtime_error("Error: Failed to write value '" + val + "' to file " + path);
    }

    // RAII handles file.close()
    return true;
}

} // namespace Sysfs