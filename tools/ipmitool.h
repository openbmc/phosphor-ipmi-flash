#pragma once

struct IpmiAccess
{
    void* intf;
};

void initializeAccess(struct IpmiAccess* access);
