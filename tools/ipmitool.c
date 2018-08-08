#include "ipmitool.h"
#include "ipmitool/ipmi_intf.h"

extern struct ipmi_intf ipmi_open_intf;

void initializeAccess(struct IpmiAccess* access)
{
    access->intf = &ipmi_open_intf;
    return;
}

void lprintf(int level, const char* format, ...)
{
    return;
}
void lperror(int level, const char* format, ...)
{
    return;
}
