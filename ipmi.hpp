#pragma once

#include "flash-ipmi.hpp"

#include <functional>

#include "host-ipmid/ipmid-api.h"

using IpmiFlashHandler =
    std::function<ipmi_ret_t(UpdateInterface* updater, const uint8_t* reqBuf,
                             uint8_t* replyBuf, size_t* dataLen)>;

/**
 * Retrieve the IPMI command handler.
 *
 * @param[in] subcommand - the command
 * @return the function to call or nullptr on error.
 */
IpmiFlashHandler getCommandHandler(FlashSubCmds command);

/**
 * Validate the minimum request length if there is one.
 *
 * @param[in] subcommand - the command
 * @param[in] requestLength - the length of the request
 * @return bool - true if valid.
 */
bool validateRequestLength(FlashSubCmds command, size_t requestLen);

/**
 * Prepare to receive a BMC image and then a signature.
 *
 * @param[in] updater - Pointer to Updater object.
 * @param[in] reqBuf - the IPMI packet.
 * @param[in] replyBuf - Pointer to buffer for any response.
 * @param[in,out] dataLen - Initially reqBuf length, set to replyBuf
 * length when done.
 * @return corresponding IPMI return code.
 */
ipmi_ret_t startTransfer(UpdateInterface* updater, const uint8_t* reqBuf,
                         uint8_t* replyBuf, size_t* dataLen);

/**
 * Receive a flash image data block and store it.
 *
 * @param[in] updater - Pointer to Updater object.
 * @param[in] reqBuf - the IPMI packet.
 * @param[in] replyBuf - Pointer to buffer for any response.
 * @param[in,out] dataLen - Initially reqBuf length, set to replyBuf
 * length when done.
 * @return corresponding IPMI return code.
 */
ipmi_ret_t dataBlock(UpdateInterface* updater, const uint8_t* reqBuf,
                     uint8_t* replyBuf, size_t* dataLen);

/**
 * Indicate all flash data has been sent.
 *
 * @param[in] updater - Pointer to Updater object.
 * @param[in] reqBuf - the IPMI packet.
 * @param[in] replyBuf - Pointer to buffer for any response.
 * @param[in,out] dataLen - Initially reqBuf length, set to replyBuf
 * length when done.
 * @return corresponding IPMI return code.
 */
ipmi_ret_t dataFinish(UpdateInterface* updater, const uint8_t* reqBuf,
                      uint8_t* replyBuf, size_t* dataLen);

/**
 * Prepare to receive a BMC image signature.
 *
 * @param[in] updater - Pointer to Updater object.
 * @param[in] reqBuf - the IPMI packet.
 * @param[in] replyBuf - Pointer to buffer for any response.
 * @param[in,out] dataLen - Initially reqBuf length, set to replyBuf
 * length when done.
 * @return corresponding IPMI return code.
 */
ipmi_ret_t startHash(UpdateInterface* updater, const uint8_t* reqBuf,
                     uint8_t* replyBuf, size_t* dataLen);

/**
 * Receive a flash hash data block and store it.
 *
 * @param[in] updater - Pointer to Updater object.
 * @param[in] reqBuf - the IPMI packet.
 * @param[in] replyBuf - Pointer to buffer for any response.
 * @param[in,out] dataLen - Initially reqBuf length, set to replyBuf
 * length when done.
 * @return corresponding IPMI return code.
 */
ipmi_ret_t hashBlock(UpdateInterface* updater, const uint8_t* reqBuf,
                     uint8_t* replyBuf, size_t* dataLen);

/**
 * Indicate all hash data has been sent.
 *
 * @param[in] updater - Pointer to Updater object.
 * @param[in] reqBuf - the IPMI packet.
 * @param[in] replyBuf - Pointer to buffer for any response.
 * @param[in,out] dataLen - Initially reqBuf length, set to replyBuf
 * length when done.
 * @return corresponding IPMI return code.
 */
ipmi_ret_t hashFinish(UpdateInterface* updater, const uint8_t* reqBuf,
                      uint8_t* replyBuf, size_t* dataLen);

/**
 * Start the flash image verification process (whatever that is).
 *
 * @param[in] updater - Pointer to Updater object.
 * @param[in] reqBuf - the IPMI packet.
 * @param[in] replyBuf - Pointer to buffer for any response.
 * @param[in,out] dataLen - Initially reqBuf length, set to replyBuf
 * length when done.
 * @return corresponding IPMI return code.
 */
ipmi_ret_t dataVerify(UpdateInterface* updater, const uint8_t* reqBuf,
                      uint8_t* replyBuf, size_t* dataLen);

/**
 * Abort the image update process.
 *
 * @param[in] updater - Pointer to Updater object.
 * @param[in] reqBuf - the IPMI packet.
 * @param[in] replyBuf - Pointer to buffer for any response.
 * @param[in,out] dataLen - Initially reqBuf length, set to replyBuf
 * length when done.
 * @return corresponding IPMI return code.
 */
ipmi_ret_t abortUpdate(UpdateInterface* updater, const uint8_t* reqBuf,
                       uint8_t* replyBuf, size_t* dataLen);

/**
 * Check on the status of the verification process.
 *
 * @param[in] updater - Pointer to Updater object.
 * @param[in] reqBuf - the IPMI packet.
 * @param[in] replyBuf - Pointer to buffer for any response.
 * @param[in,out] dataLen - Initially reqBuf length, set to replyBuf
 * length when done.
 * @return corresponding IPMI return code.
 */
ipmi_ret_t checkVerify(UpdateInterface* updater, const uint8_t* reqBuf,
                       uint8_t* replyBuf, size_t* dataLen);
