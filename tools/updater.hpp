#pragma once

#include "interface.hpp"

#include <ipmiblob/blob_interface.hpp>
#include <string>

namespace host_tool
{

/**
 * Attempt to update the BMC's firmware using the interface provided.
 *
 * @param[in] blob - pointer to blob interface implementation object.
 * @param[in] handler - pointer to the data interface implementation object.
 * @param[in] imagePath - the path to the image file.
 * @param[in] signaturePath - the path to the signature file.
 * @throws ToolException on failures.
 */
void updaterMain(ipmiblob::BlobInterface* blob, DataInterface* handler,
                 const std::string& imagePath,
                 const std::string& signaturePath);

} // namespace host_tool
