#pragma once

#include "blob_interface.hpp"
#include "ipmi_interface.hpp"

namespace host_tool
{

class BlobHandler : public BlobInterface
{
  public:
    enum BlobOEMCommands
    {
        bmcBlobGetCount = 0,
        bmcBlobEnumerate = 1,
        bmcBlobOpen = 2,
        bmcBlobRead = 3,
        bmcBlobWrite = 4,
        bmcBlobCommit = 5,
        bmcBlobClose = 6,
        bmcBlobDelete = 7,
        bmcBlobStat = 8,
        bmcBlobSessionStat = 9,
        bmcBlobWriteMeta = 10,
    };

    explicit BlobHandler(IpmiInterface* ipmi) : ipmi(ipmi){};

    /**
     * Retrieve the blob count.
     *
     * @return the number of blob_ids found (0 on failure).
     */
    int getBlobCount();

    /**
     * Given an index into the list of blobs, return the name.
     *
     * @param[in] index - the index into the list of blob ids.
     * @return the name as a string or empty on failure.
     */
    std::string enumerateBlob(std::uint32_t index);

    /**
     * @throws BlobException.
     */
    void writeMeta(std::uint16_t session, std::uint32_t offset,
                   const std::vector<std::uint8_t>& bytes) override;

    /**
     * @throw BlobException.
     */
    void writeBytes(std::uint16_t session, std::uint32_t offset,
                    const std::vector<std::uint8_t>& bytes) override;

    std::vector<std::string> getBlobList() override;

    /**
     * @throws BlobException.
     */
    StatResponse getStat(const std::string& id) override;

    /**
     * @throws BlobException.
     */
    std::uint16_t
        openBlob(const std::string& id,
                 blobs::FirmwareBlobHandler::UpdateFlags handlerFlags) override;

    void closeBlob(std::uint16_t session) override;

    /**
     * @throws BlobException.
     */
    std::vector<std::uint8_t> readBytes(std::uint16_t session,
                                        std::uint32_t offset,
                                        std::uint32_t length) override;

  private:
    /**
     * Send the contents of the payload to IPMI, this method handles wrapping
     * with the OEN, subcommand and CRC.
     *
     * @param[in] command - the blob command.
     * @param[in] payload - the payload bytes.
     * @return the bytes returned from the ipmi interface.
     * @throws BlobException.
     */
    std::vector<std::uint8_t>
        sendIpmiPayload(BlobOEMCommands command,
                        const std::vector<std::uint8_t>& payload);

    /**
     * Generic blob byte writer.
     *
     * @param[in] command - the command associated with this write.
     * @param[in] session - the session id.
     * @param[in] offset - the offset for the metadata to write.
     * @param[in] bytes - the bytes to send.
     * @throws BlobException on failure.
     */
    void writeGeneric(BlobOEMCommands command, std::uint16_t session,
                      std::uint32_t offset,
                      const std::vector<std::uint8_t>& bytes);

    IpmiInterface* ipmi;
};

} // namespace host_tool
