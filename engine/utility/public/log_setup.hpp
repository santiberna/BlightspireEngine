#pragma once
#include <string>

namespace bb
{
// After this is called, the logs are written to a file, instead of the console.
// TODO: currently does nothing and does NOT overwrite the default logger
void setupDefaultLogger();

// After this is called, the logs are written to a file, instead of the console. Overwrites the default logger
void setupFileLogger();

// Returns the name of the running system
std::string getOsName();
}
