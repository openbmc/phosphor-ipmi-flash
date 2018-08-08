#pragma once

#include <string>

// Main entry point for the update command.
// Update uploads and verifies the image.
// throws exception on errors.
void UpdaterMain(const std::string& interface, const std::string& image,
                 const std::string& signature);
