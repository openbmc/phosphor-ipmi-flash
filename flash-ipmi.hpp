#pragma once

#include <cstdio>
#include <string>
#include <vector>

#include "host-ipmid/ipmid-api.h"

/* Clearer way to represent the subcommand size. */
#define SUBCMD_SZ sizeof(uint8_t)

/*
 * flashStartTransfer -- starts file upload.
 * flashDataBlock -- adds data to image file.
 * flashDataFinish -- closes the file.
 *
 * flashStartHash -- starts uploading hash.
 * flashDataHash -- adds data to the hash.
 * flashDataVerify -- triggers verification.
 *
 * flashAbort -- abort everything.
 *
 * flashVerifyCheck -- Check if the verification has completed.
 *
 * flashVersion -- Get the version of this OEM Handler.
 *
 * flashRequestRegion -- Request the physical address for decode (bridge)
 * flashDataExtBlock -- Provide image data via a bridge
 * flashHashExtData -- Provide hash data via a bridge
 */
enum FlashSubCmds
{
    /* Start a transfer. */
    flashStartTransfer = 0,
    /* Data block. */
    flashDataBlock = 1,
    /* Close file. */
    flashDataFinish = 2,

    /* Start a hash transfer. */
    flashStartHash = 3,
    /* Add data to the hash. */
    flashHashData = 4,
    /* Close out the hash file. */
    flashHashFinish = 5,

    /* Verify the flash image against the hast sent. */
    flashDataVerify = 6,

    /* Abort. */
    flashAbort = 7,

    /*
     * Check if the verification is ready and was successful.
     * If the response from the IPMI command is OK, check the
     * response bytes to know if it's ready or still computing,
     * or failed.
     */
    flashVerifyCheck = 8,

    flashVersion = 9,
    flashRequestRegion = 10,
    flashDataExtBlock = 11,
    flashHashExtData = 12,
    flashMapRegionLpc = 13,
};

/*
 * StartTransfer expects a basic structure providing some information.
 */
struct StartTx
{
    uint8_t cmd;
    uint32_t length; /* Maximum image length is 4GiB (little-endian) */
} __attribute__((packed));

struct ChunkHdr
{
    uint8_t cmd;
    uint32_t offset; /* 0-based write offset */
    uint8_t data[];
} __attribute__((packed));

class UpdateInterface
{
  public:
    virtual ~UpdateInterface() = default;

    /**
     * Prepare to receive a BMC image and then a signature.
     *
     * @param[in] length - the size of the flash image.
     * @return true on success, false otherwise.
     */
    virtual bool start(uint32_t length) = 0;

    /**
     * Attempt to write the bytes at the offset.
     *
     * @param[in] offset - the 0-based byte offset into the flash image.
     * @param[in] bytes - the bytes to write.
     * @return true on success, false otherwise.
     */
    virtual bool flashData(uint32_t offset,
                           const std::vector<uint8_t>& bytes) = 0;

    /**
     * Called to indicate the host is done sending the flash bytes.
     */
    virtual bool flashFinish() = 0;

    /**
     * Prepare to receive a BMC image signature.
     *
     * @param[in] length - length of signature in bytes.
     * @return true on success, false otherwise.
     */
    virtual bool startHash(uint32_t length) = 0;

    /**
     * Attempt to write the bytes at the offset.
     *
     * @param[in] offset - the 0-based byte offset into the flash image.
     * @param[in] bytes - the bytes to write.
     * @return true on success, false otherwise.
     */
    virtual bool hashData(uint32_t offset,
                          const std::vector<uint8_t>& bytes) = 0;

    /**
     * Called to indicate the host is done sending the hash bytes.
     */
    virtual bool hashFinish() = 0;

    /**
     * Kick off the flash image verification process.
     *
     * @return true if it was started succesfully.
     */
    virtual bool startDataVerification() = 0;

    /**
     * Attempt to abort everything.
     *
     * @return true if aborted, false if unable or failed.
     */
    virtual bool abortUpdate() = 0;
};

class FlashUpdate : public UpdateInterface
{
  public:
    FlashUpdate(std::string stagingPath, std::string hash = "") :
        flashLength(0), flashFd(nullptr), tmpPath(stagingPath), hashLength(0),
        hashFd(nullptr), hashPath(hash){};
    ~FlashUpdate();

    FlashUpdate(const FlashUpdate&) = default;
    FlashUpdate& operator=(const FlashUpdate&) = default;
    FlashUpdate(FlashUpdate&&) = default;
    FlashUpdate& operator=(FlashUpdate&&) = default;

    bool start(uint32_t length) override;
    bool flashData(uint32_t offset, const std::vector<uint8_t>& bytes) override;
    bool flashFinish() override;

    bool startHash(uint32_t length) override;
    bool hashData(uint32_t offset, const std::vector<uint8_t>& bytes) override;
    bool hashFinish() override;

    bool startDataVerification() override;
    bool abortUpdate() override;

  private:
    /**
     * Attempt to write the bytes at the offset.
     *
     * @param[in] fd - the file stream pointer.
     * @param[in] offset - the 0-based byte offset into the flash image.
     * @param[in] bytes - the bytes to write.
     * @return true on success, false otherwise.
     */
    bool writeBlock(std::FILE* fd, uint32_t offset,
                    const std::vector<uint8_t>& bytes);

    /**
     * Tries to close out everything.
     */
    void closeEverything();

    /**
     * Tries to close out and delete anything staged.
     */
    void abortEverything();

    /**
     * Open all staged file handles you expect to use.
     *
     * @return false on failure.
     */
    bool openEverything();

    /* The length of the flash image in bytes. */
    uint32_t flashLength;

    /* The file handle to the flash staging file. */
    std::FILE* flashFd;

    /* Where the bytes are written before verification. */
    std::string tmpPath;

    /* The length of the hash in bytes. */
    uint32_t hashLength;

    /* The file handle to the hash file. */
    std::FILE* hashFd;

    /* Where we write the hash bytes.  Only required if your verification
     * process uses a separate signature.
     */
    std::string hashPath;
};
