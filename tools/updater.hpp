#pragma once

#include "interface.hpp"

#include <ipmiblob/blob_interface.hpp>
#include <string>

namespace host_tool
{

/** Object that actually handles the update itself. */
class UpdateHandler
{
  public:
    UpdateHandler(ipmiblob::BlobInterface* blob, DataInterface* handler) :
        blob(blob), handler(handler)
    {
    }

    virtual ~UpdateHandler() = default;

    /**
     * Check if the goal firmware is listed in the blob_list and that the
     * handler's supported data type is available.
     *
     * @param[in] goalFirmware - the firmware to check /flash/image
     * /flash/tarball, etc.
     */
    virtual bool checkAvailable(const std::string& goalFirmware);

    /**
     * Send the file contents at path to the blob id, target.
     *
     * @param[in] target - the blob id
     * @param[in] path - the source file path
     * @throw ToolException on failure.
     */
    virtual void sendFile(const std::string& target, const std::string& path);

    /**
     * Trigger verification.
     *
     * @param[in] target - the verification blob id (may support multiple in the
     * future.
     * @return true if verified, false if verification errors.
     * @throw ToolException on failure (TODO: throw on timeout.)
     */
    virtual bool verifyFile(const std::string& target);

  private:
    ipmiblob::BlobInterface* blob;
    DataInterface* handler;
};

/**
 * Poll an open verification session.
 *
 * @param[in] session - the open verification session
 * @param[in] blob - pointer to blob interface implementation object.
 * @return true if the verification was successul.
 */
bool pollVerificationStatus(std::uint16_t session,
                            ipmiblob::BlobInterface* blob);

/**
 * Attempt to update the BMC's firmware using the interface provided.
 *
 * @param[in] updater - update handler object.
 * @param[in] imagePath - the path to the image file.
 * @param[in] signaturePath - the path to the signature file.
 * @throws ToolException on failures.
 */
void updaterMain(UpdateHandler* updater, const std::string& imagePath,
                 const std::string& signaturePath);

} // namespace host_tool
