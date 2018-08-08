#pragma once

#include <vector>

#include "config.h"
#include "raw.hpp"

constexpr int OEM_FLASH_UPDATE_BT_COMMAND = 127;

constexpr int OEN_SIZE = 3;

// OEM Command Netfn for IPMI.
constexpr uint8_t NETFN_OEM = 0x2e;

// OEM Group identifier for OpenBMC.
constexpr uint8_t OEN_OPENBMC[OEN_SIZE] = {0xcf, 0xc2, 0x00};
constexpr uint8_t OEN_GOOGLE[OEN_SIZE] = {0x79, 0x2b, 0x00};

#ifdef ENABLE_GOOGLE

/* The header for the IPMI Raw command with a subcommand leading byte. */
struct CommandHdr
{
    uint8_t netfn;
    uint8_t command;
    uint8_t oen[OEN_SIZE];
    /* This is labeled subcommand but anything after padding is data. */
    uint8_t subcommand;
} __attribute__((packed));

#else

/* If not using the GOOGLE OEM, it uses the Firmware Netfn. */

struct CommandHdr
{
    uint8_t netfn;
    uint8_t command;
    uint8_t subcommand;
} __attribute__((packed));

#endif

constexpr int kFlashCommandHdrSizeBytes = sizeof(CommandHdr);

/* This interface defines an interface of helper calls that'll deal with the
 * details for sending the updater ipmi commands.  This also enables testing via
 * injection.
 */
class UpdateHelperInterface
{
  public:
    ~UpdateHelperInterface() = default;

    /**
     * Try to send an IPMI update firmware command that is only the command and
     * no payload.
     *
     * @param[in] command : the command byte to send.
     * @return the IPMI response.
     */
    virtual struct IpmiResponse SendEmptyCommand(int command) = 0;

    /**
     * Try to send an IPMI update firmware command, possibly with payload.
     *
     * @param[in] command : the command byte to send.
     * @param[in] payload : the payload bytes.
     * @return the IPMI response.
     */
    virtual struct IpmiResponse
        SendCommand(int command, const std::vector<uint8_t>& payload) = 0;
};

class IpmiUpdateHelper : public UpdateHelperInterface
{
  public:
    IpmiUpdateHelper(RawInterface* raw) : raw(raw)
    {
    }

    struct IpmiResponse SendEmptyCommand(int command) override;
    struct IpmiResponse
        SendCommand(int command, const std::vector<uint8_t>& payload) override;

  private:
    RawInterface* raw;
};
