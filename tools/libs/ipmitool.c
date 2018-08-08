#include "ipmitool.h"

#include <ipmitool/ipmi_intf.h>
#include <string.h>

extern struct ipmi_intf ipmi_open_intf;

int ipmiSendCommand(const uint8_t* bytes, int length, struct IpmiResponse* resp)
{
    struct ipmi_intf* intf = &ipmi_open_intf;

    /* The length needs to be at least two bytes [netfn][cmd] */
    if (length < 2)
    {
        return -1;
    }

    uint8_t data[MAX_PIPELINE_BANDWIDTH];
    struct ipmi_rq request;
    memset(&data[0], 0, sizeof(data));
    memset(&request, 0, sizeof(request));

    ipmi_intf_session_set_timeout(intf, 15);
    ipmi_intf_session_set_retry(intf, 1);

    request.msg.netfn = bytes[0];
    request.msg.lun = 0x00;
    request.msg.cmd = bytes[1];
    request.msg.data = &data[0];

    /* Can you fit the request in the buffer? */
    if ((length - 2) > sizeof(data))
    {
        return -1;
    }

    /* Skip beyond netfn and command. */
    memcpy(request.msg.data, &bytes[2], length - 2);
    request.msg.data_len = length - 2;

    /* Actually send the command and check for a response. */
    struct ipmi_rs* response = intf->sendrecv(intf, &request);
    if (!response)
    {
        return -1;
    }

    /* If the caller wanted the response back. */
    if (resp)
    {
        resp->ccode = response->ccode;
        if (response->data_len <= sizeof(resp->data))
        {
            memcpy(resp->data, response->data, response->data_len);
            resp->dataLen = response->data_len;
            return 0;
        }
        /* TODO: deal with truncation... */
    }

    return 0;
}

/* Nullify the methods we don't care about. */
void lprintf(int level, const char* format, ...)
{
    return;
}
void lperror(int level, const char* format, ...)
{
    return;
}
