#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


/*
Functions for reading and writing to sysfs files. 
These are used by the Cpu class to interact with the CPU's sysfs entries.
*/
namespace Sysfs
{
    std::string read(const std::string &path);
    bool write(const std::string &path, const std::string &val);

}