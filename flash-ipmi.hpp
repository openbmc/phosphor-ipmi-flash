#pragma once

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

    virtual bool start(uint32_t) = 0;
    virtual bool flashData(uint32_t offset,
                           const std::vector<uint8_t>& bytes) = 0;
};

class FlashUpdate : public UpdateInterface
{
  public:
    FlashUpdate() = default;
    ~FlashUpdate() = default;
    FlashUpdate(const FlashUpdate&) = default;
    FlashUpdate& operator=(const FlashUpdate&) = default;
    FlashUpdate(FlashUpdate&&) = default;
    FlashUpdate& operator=(FlashUpdate&&) = default;

    /**
     * Prepare to receive a BMC image and then a signature.
     *
     * @param[in] packet - pointer to the StartTx structure.
     * @return true on success, false otherwise.
     */
    bool start(uint32_t length) override;

    /**
     * Attempt to write the bytes at the offset.
     *
     * @param[in] offset - the 0-based byte offset into the flash image.
     * @param[in] bytes - the bytes to write.
     * @return true on success, false otherwise.
     */
    bool flashData(uint32_t offset, const std::vector<uint8_t>& bytes) override;

  private:
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
};
