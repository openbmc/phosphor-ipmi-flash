#include <ipmitool/ipmi_intf.h>

#define BUF2STR_MAXIMUM_OUTPUT_SIZE (3 * 1024 + 1)

void ipmi_intf_session_set_timeout(struct ipmi_intf *intf, uint32_t timeout)
{
    intf->ssn_params.timeout = timeout;
}

void ipmi_intf_session_set_retry(struct ipmi_intf *intf, int retry)
{
    intf->ssn_params.retry = retry;
}

/* Nullify the methods we don't care about. */
void lprintf(int level, const char *format, ...)
{
    return;
}
void lperror(int level, const char *format, ...)
{
    return;
}

int verbose = 0;

const char *buf2str_extended(const uint8_t *buf, int len, const char *sep)
{
    static char str[BUF2STR_MAXIMUM_OUTPUT_SIZE];
    char *cur;
    int i;
    int sz;
    int left;
    int sep_len;

    if (buf == NULL)
    {
        snprintf(str, sizeof(str), "<NULL>");
        return (const char *)str;
    }
    cur = str;
    left = sizeof(str);
    if (sep)
    {
        sep_len = strlen(sep);
    }
    else
    {
        sep_len = 0;
    }
    for (i = 0; i < len; i++)
    {
        /* may return more than 2, depending on locale */
        sz = snprintf(cur, left, "%2.2x", buf[i]);
        if (sz >= left)
        {
            /* buffer overflow, truncate */
            break;
        }
        cur += sz;
        left -= sz;
        /* do not write separator after last byte */
        if (sep && i != (len - 1))
        {
            if (sep_len >= left)
            {
                break;
            }
            strncpy(cur, sep, left - sz);
            cur += sep_len;
            left -= sep_len;
        }
    }
    *cur = '\0';

    return (const char *)str;
}

const char *buf2str(const uint8_t *buf, int len)
{
    return buf2str_extended(buf, len, NULL);
}

uint8_t ipmi_csum(uint8_t *d, int s)
{
    uint8_t c = 0;
    for (; s > 0; s--, d++)
        c += *d;
    return -c;
}

void printbuf(const uint8_t *buf, int len, const char *desc)
{
    int i;

    if (len <= 0)
        return;

    if (verbose < 1)
        return;

    fprintf(stderr, "%s (%d bytes)\n", desc, len);
    for (i = 0; i < len; i++)
    {
        if (((i % 16) == 0) && (i != 0))
            fprintf(stderr, "\n");
        fprintf(stderr, " %2.2x", buf[i]);
    }
    fprintf(stderr, "\n");
}

IPMI_OEM
ipmi_get_oem(struct ipmi_intf *intf)
{
    /* Execute a Get Device ID command to determine the OEM */
    struct ipmi_rs *rsp;
    struct ipmi_rq req;
    struct ipm_devid_rsp *devid;

    if (intf->fd == 0)
    {
        if (sel_iana != IPMI_OEM_UNKNOWN)
        {
            return sel_iana;
        }
        return IPMI_OEM_UNKNOWN;
    }

    /*
     * Return the cached manufacturer id if the device is open and
     * we got an identified OEM owner.   Otherwise just attempt to read
     * it.
     */
    if (intf->opened && intf->manufacturer_id != IPMI_OEM_UNKNOWN)
    {
        return intf->manufacturer_id;
    }

    memset(&req, 0, sizeof(req));
    req.msg.netfn = IPMI_NETFN_APP;
    req.msg.cmd = BMC_GET_DEVICE_ID;
    req.msg.data_len = 0;

    rsp = intf->sendrecv(intf, &req);
    if (rsp == NULL)
    {
        lprintf(LOG_ERR, "Get Device ID command failed");
        return IPMI_OEM_UNKNOWN;
    }
    if (rsp->ccode > 0)
    {
        lprintf(LOG_ERR, "Get Device ID command failed: %#x %s", rsp->ccode,
                val2str(rsp->ccode, completion_code_vals));
        return IPMI_OEM_UNKNOWN;
    }

    devid = (struct ipm_devid_rsp *)rsp->data;

    lprintf(LOG_DEBUG, "Iana: %u",
            IPM_DEV_MANUFACTURER_ID(devid->manufacturer_id));

    return IPM_DEV_MANUFACTURER_ID(devid->manufacturer_id);
}
