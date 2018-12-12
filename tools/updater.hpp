#pragma once

#include "blob_interface.hpp"
#include "interface.hpp"

#include <string>

/**
 * Attempt to update the BMC's firmware using the interface provided.
 *
 * @param[in] blob - pointer to blob interface implementation object.
 * @param[in] handler - pointer to the data interface implementation object.
 * @param[in] imagePath - the path to the image file.
 * @param[in] signaturePath - the path to the signature file.
 * @return non-zero on failure.
 */
int updaterMain(BlobInterface* blob, DataInterface* handler,
                const std::string& imagePath, const std::string& signaturePath);
