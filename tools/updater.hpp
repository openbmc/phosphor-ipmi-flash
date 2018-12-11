#pragma once

#include <string>

/**
 * Attempt to update the BMC's firmware using the interface provided.
 *
 * @param[in] interface - the interface to use.
 * @param[in] imagePath - the path to the image file.
 * @param[in] signaturePath - the path to the signature file.
 * @return non-zero on failure.
 */
int updaterMain(const std::string& interface, const std::string& imagePath,
                const std::string& signaturePath);
