#ifndef ROM_STRUCTURES_H
#define ROM_STRUCTURES_H

#include <QObject>
#include <QtWidgets/QApplication>
#include <QVector3D>
#include <QQuaternion>
#include <string>
#include <stdexcept>

#pragma once

inline const char* ROM_COLOR_GREEN = "\033[1;32m";  // Bright green
inline const char* ROM_COLOR_RESET = "\033[0m";     // Reset to default
inline const char* ROM_COLOR_RED = "\033[1;31m";    // Bright red
inline const char* ROM_COLOR_YELLOW = "\033[1;33m"; // Bright yellow
inline const char* ROM_COLOR_BLUE = "\033[1;34m";   // Bright blue
inline const char* ROM_COLOR_MAGENTA = "\033[1;35m"; // Bright magenta
inline const char* ROM_COLOR_CYAN = "\033[1;36m";   // Bright cyan
inline const char* ROM_COLOR_WHITE = "\033[1;37m";  // Bright white
inline const char* ROM_COLOR_BLACK = "\033[1;30m";  // Bright black
inline const char* ROM_COLOR_BOLD = "\033[1m";     // Bold text
inline const char* ROM_COLOR_UNDERLINE = "\033[4m"; // Underlined text
inline const char* ROM_COLOR_REVERSED = "\033[7m"; // Reversed text
inline const char* ROM_COLOR_BOLD_GREEN = "\033[1;32m"; // Bold bright green
inline const char* ROM_COLOR_BOLD_RED = "\033[1;31m";   // Bold bright red
inline const char* ROM_COLOR_BOLD_YELLOW = "\033[1;33m"; // Bold bright yellow
inline const char* ROM_COLOR_BOLD_BLUE = "\033[1;34m";   // Bold bright blue
inline const char* ROM_COLOR_BOLD_MAGENTA = "\033[1;35m"; // Bold bright magenta
inline const char* ROM_COLOR_BOLD_CYAN = "\033[1;36m";   // Bold bright cyan

enum class Mode {
    ssh,          // default
    ros2_control,
    ekf,
    carto,
    nav2_1,
    nav2_2,
    nav2_3,
    bt,
    topic,
    log
};
inline std::string ModeToString(Mode mode)
{
    switch (mode) {
        case Mode::ssh:
            return "ssh";
        case Mode::ros2_control:
            return "ros2_control";
        case Mode::ekf:
            return "ekf";
        case Mode::carto:
            return "carto";
        case Mode::nav2_1:
            return "nav2_1";
        case Mode::nav2_2:
            return "nav2_2";
        case Mode::nav2_3:
            return "nav2_3";
        case Mode::bt:
            return "bt";
        case Mode::topic:
            return "topic";
        case Mode::log:
            return "log";
        default:
            // Handle unexpected values (good practice)
            throw std::runtime_error("Unknown Mode value.");
    }
}


#endif 
