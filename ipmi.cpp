#include "flash-ipmi.hpp"
#include "ipmi.hpp"

ipmi_ret_t startTransfer(UpdateInterface* updater, const uint8_t* reqBuf,
                         uint8_t* replyBuf, size_t* dataLen)
{
    /* Validate the request buffer. */
    if ((sizeof(struct StartTx) + SUBCMD_SZ) > (*dataLen))
    {
        return IPMI_CC_INVALID;
    }

    auto request = reinterpret_cast<const struct StartTx*>(reqBuf);

    if (!updater->start(request))
    {
        return IPMI_CC_INVALID;
    }

    /* We were successful and set the response byte to 0. */
    replyBuf[0] = 0x00;
    (*dataLen) = 1;
    return IPMI_CC_OK;
}
