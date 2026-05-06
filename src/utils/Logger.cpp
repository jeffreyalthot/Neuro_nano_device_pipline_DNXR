#include "Logger.h"
#include <iostream>
namespace dnxr { void Logger::info(const std::string& msg){ std::cout<<"[INFO] "<<msg<<"\n";} void Logger::warn(const std::string& msg){ std::cout<<"[WARN] "<<msg<<"\n";} void Logger::error(const std::string& msg){ std::cerr<<"[ERROR] "<<msg<<"\n";} }
