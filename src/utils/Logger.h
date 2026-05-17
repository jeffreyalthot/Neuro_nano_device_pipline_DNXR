// Logger.h

#pragma once

#include <string>

namespace dnxr {

class Logger {
public:
    /// Log informatif (état normal système)
    static void info(const std::string& msg);

    /// Log avertissement (dégradation non critique)
    static void warn(const std::string& msg);

    /// Log critique (erreur ou état dangereux)
    static void error(const std::string& msg);
};

} // namespace dnxr