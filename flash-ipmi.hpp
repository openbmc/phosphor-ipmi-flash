#pragma once

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
