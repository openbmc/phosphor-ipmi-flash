#pragma once

#include "host-ipmid/ipmid-api.h"

#include "flash-ipmi.hpp"

/**
 * Prepare to receive a BMC image and then a signature.
 *
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
 * @param[in] reqBuf - the IPMI packet.
 * @param[in] replyBuf - Pointer to buffer for any response.
 * @param[in,out] dataLen - Initially reqBuf length, set to replyBuf
 * length when done.
 * @return corresponding IPMI return code.
 */
ipmi_ret_t dataBlock(UpdateInterface* updater, const uint8_t* reqBuf,
                     uint8_t* replyBuf, size_t* dataLen);

