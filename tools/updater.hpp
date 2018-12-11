#pragma once

#include "blob_interface.hpp"

#include <string>

/**
 * Attempt to update the BMC's firmware using the interface provided.
 *
 * @param[in] blob - pointer to blob interface implementation object.
 * @param[in] interface - the interface to use.
 * @param[in] imagePath - the path to the image file.
 * @param[in] signaturePath - the path to the signature file.
 * @return non-zero on failure.
 */
int updaterMain(BlobInterface* blob, const std::string& interface,
                const std::string& imagePath, const std::string& signaturePath);
