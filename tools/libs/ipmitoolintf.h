#pragma once

/* The Aspeed AST2400 & AST2500 have 64 bytes of SRAM as the FIFO for each
 * direction, of which 2 bytes are reserved for len and for seq by upper layer
 * ipmi driver.
 */
#define MAX_PIPELINE_BANDWIDTH 62
#define IPMI_BUF_SIZE 1024

#include <stdint.h>

struct IpmiResponse
{
    uint8_t ccode;
    uint8_t data[IPMI_BUF_SIZE];
    int dataLen;
};

/**
 * Call into the ipmitool source to send the IPMI packet.
 *
 * @param[in] bytes - the IPMI packet contents.
 * @param[in] length - the number of bytes.
 * @param[in,out] resp - a pointer to write the response.
 * @return 0 on success.
 */
int ipmiSendCommand(const uint8_t* bytes, int length,
                    struct IpmiResponse* resp);
