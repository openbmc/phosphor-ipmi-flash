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

int ipmiSendCommand(const uint8_t* bytes, int length,
                    struct IpmiResponse* resp);
