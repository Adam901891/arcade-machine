#ifndef ARCADE_MACHINE_CONFIGURATION_H
#define ARCADE_MACHINE_CONFIGURATION_H

#define ARCADE_MACHINE_SCALING_FACTOR 1
#define ARCADE_MACHINE_RES_X 1920 * ARCADE_MACHINE_SCALING_FACTOR
#define ARCADE_MACHINE_RES_Y 1080 * ARCADE_MACHINE_SCALING_FACTOR

#ifdef _WIN32
#define ARCADE_MACHINE_PATH_SEP "\\"
#else
#define ARCADE_MACHINE_PATH_SEP "/"
#endif

#ifdef __arm__
#define ARCADE_MACHINE_INSTRUCTION_SET "arm"
#else
#define ARCADE_MACHINE_INSTRUCTION_SET "x86"
#endif

#ifdef _WIN32
#define ARCADE_MACHINE_OS "windows"
#define ARCADE_MACHINE_BINARY_EXT ".exe"
#elif __APPLE__
#define ARCADE_MACHINE_OS "macos"
#define ARCADE_MACHINE_BINARY_EXT ""
#else
#define ARCADE_MACHINE_OS "linux"
#define ARCADE_MACHINE_BINARY_EXT ""
#endif

#endif