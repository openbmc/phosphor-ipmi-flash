#pragma once

#include "interface.hpp"

#include <ipmiblob/blob_interface.hpp>
#include <string>

namespace host_tool
{

class UpdateHandlerInterface
{
  public:
    virtual ~UpdateHandlerInterface() = default;

    /**
     * Check if the goal firmware is listed in the blob_list and that the
     * handler's supported data type is available.
     *
     * @param[in] goalFirmware - the firmware to check /flash/image
     * /flash/tarball, etc.
     */
    virtual bool checkAvailable(const std::string& goalFirmware) = 0;

    /**
     * Send the file contents at path to the blob id, target.
     *
     * @param[in] target - the blob id
     * @param[in] path - the source file path
     */
    virtual void sendFile(const std::string& target,
                          const std::string& path) = 0;

    /**
     * Trigger verification.
     *
     * @param[in] target - the verification blob id (may support multiple in the
     * future.
     * @return true if verified, false if verification errors.
     */
    virtual bool verifyFile(const std::string& target) = 0;

    /**
     * Cleanup the artifacts by triggering this action.
     */
    virtual void cleanArtifacts() = 0;
};

/** Object that actually handles the update itself. */
class UpdateHandler : public UpdateHandlerInterface
{
  public:
    UpdateHandler(ipmiblob::BlobInterface* blob, DataInterface* handler) :
        blob(blob), handler(handler)
    {
    }

    ~UpdateHandler() = default;

    bool checkAvailable(const std::string& goalFirmware) override;

    /**
     * @throw ToolException on failure.
     */
    void sendFile(const std::string& target, const std::string& path) override;

    /**
     * @throw ToolException on failure (TODO: throw on timeout.)
     */
    bool verifyFile(const std::string& target) override;

    void cleanArtifacts() override;

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
bool pollStatus(std::uint16_t session, ipmiblob::BlobInterface* blob);

/**
 * Attempt to update the BMC's firmware using the interface provided.
 *
 * @param[in] updater - update handler object.
 * @param[in] imagePath - the path to the image file.
 * @param[in] signaturePath - the path to the signature file.
 * @throws ToolException on failures.
 */
void updaterMain(UpdateHandlerInterface* updater, const std::string& imagePath,
                 const std::string& signaturePath);

} // namespace host_tool
