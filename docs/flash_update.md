# Secure Flash Update Mechanism

This document describes the OpenBmc software implementing the secure flash
update mechanism.

## Introduction

This supports two methods of providing the image to stage. You can send the
file over IPMI packets, which is a very slow process. A 32-MiB image can tak
~3 hours to send via this method.  This can be done in <1 minutes via the PCI
bridge, or just a few minutes via LPC depending on the size of the mapped area.

This is implemented in the Google OEM number space: **11129**.

It is command: **127**

The OpenBmc tool, not yet upstreamed, supports this approach.

The image must be signed via the production or development keys, the former
being required for production builds. The image itself and the image signature
are separately sent to the BMC for verification. The verification package
source is beyond the scope of this design.

Basically the IPMI OEM handler receives the image in one fashion or another and
then triggers the `verify_image` service. Then, the user polls until the result
is reported. This is because the image verification process can exceed 10
seconds.

### Using Legacy Images

The image flashing mechanism itself is the initramfs stage during reboot. It
will check for files named "`image-*`" and flash them appropriately for each
name to section. The IPMI command creates a file `/run/initramfs/bmc-image` and
writes the contents there. It was found that writing it in /tmp could cause OOM
errors moving it on low memory systems, whereas renaming a file within the same
folder seems to only update the directory inode's contents.

### Using UBI

The staging file path can be controlled via software configuration.  The image
is assumed to be the tarball contents and is written into `/tmp/{tarball_name}.gz`

TODO: Flesh out the UBI approach.

## Primary Implementation Approaches

To determine if the bridge sequence is supported you can send the
`FlashRequestRegion` subcommand and see if it returns success with an address.
However, for the sake of sanity, there is also a `FlashVersion` subcommand
that'll return the version of the protocol.

### Data over IPMI Sequence

If you're updating the image entirely over IPMI, then you should expect to send
the following sequence of commands:

1.  `FlashStartTransfer`
1.  `FlashDataBlock` (for each piece of the image)
1.  `FlashDataFinish`
1.  `FlashStartHash`
1.  `FlashHashData` (for each piece of the hash)
1.  `FlashHashFinish`
1.  `FlashDataVerify`
1.  `FlashVerifyCheck` (repeatedly until it results with 3-5s sleeps)

### Data over Bridge Sequence

If you're using some region to send the image and hash, you should expect to
send the following sequences of commands:

1.  `FlashStartTransfer`
1.  `FlashMapRegionLpc`(if necessary)
1.  `FlashRequestRegion`
1.  `FlashDataExtBlock` (for each piece of the image)
1.  `FlashDataFinish`
1.  `FlashStartHash`
1.  `FlashHashExtData` (for each piece of the hash)
1.  `FlashHashFinish`
1.  `FlashDataVerify`
1.  `FlashVerifyCheck` (repeatedly until it results with 3-5s sleeps)

## Bridge Options

### P2A

The PCI-to-AHB bridge is only available on some systems, possibly on all
non-Zaius, and provides a 64-KiB region that can be pointed anywhere in BMC
memory space.

It is controlled by two PCIe MMIO addresses that are based on BAR1. Further
specifics can be found in the ASPEED data sheet. However, the way it works in
this instance is that the BMC has configured a region of physical memory it
plans to use as this buffer region. The BMC returns the address to host program
so it can configure the PCIe MMIO registers properly for that address.

As an example, the 64-KiB region the Iceblink OpenBmc image will use for this
approach is the last 64-KiB of the VGA reserved region: `0x47ff0000`.

### LPC

Like the P2A mechanism, the BMC must have already allocated a memory region
with a platform-defined size. This region is mapped to LPC space at the request
of the host, which also specifies the LPC address and region size in the
mapping request subcommand. The host can then read the _actual_ LPC address at
which the BMC was able to map, checking that it's usable.

### USB

As future work, USB should be considered an option for staging the image to be
copied to the BMC.

## Commands

The following details each subcommand with which you'll lead the body of the
command. Unlike some designs, the responses don't necessarily include the
corresponding sub-command, partially because this protocol is meant to be used
by one user at a time and trying to use otherwise can have negative effects and
is not supported.

In the following, any reference to the command body starts after the 3 bytes of
OEM header, and the 1-byte subcommand.

### FlashStartTransfer (0)

The FlashStartTransfer command expects to receive a body of:

```
struct StartTx
{
    uint32_t length; /* Maximum image length is 4GiB */
};
```

However, this data is presently ignored.

This command will first close out and abort any previous attempt at updating
the flash.

On success it will return 1 byte of `0x00`, in the body, after the 3-byte OEM
portion.

### FlashDataBlock (1)

This command expects to receive a body of:

```
struct ChunkHdr
{
    uint32_t offset; /* The byte sequence start, (0 based). */
};
```

However, this data is presently ignored.

Immediately following this structure are the bytes to write. The length of the
entire packet is variable and handled at a higher level, therefore the number
of bytes to write is the size of the command body less the sub-command (1 byte)
and less the structure size (4 bytes).

On success it will return 1 byte of `0x00`, in the body, after the 3-byte OEM
portion.

### FlashDataFinish (2)

This command expects the body to be empty.

On success it will return 1 byte of `0x00`, in the body, after the 3-byte OEM
portion.

### FlashStartHash (3)

The FlashStartHash command expects to receive a body of:

```
struct StartTx {
    uint32_t length; /* Maximum image length is 4GiB */
};
```

This is used! But it's only checked to see that it's non-zero.

On success it will return 1 byte of `0x00`, in the body, after the 3-byte OEM
portion.

### FlashHashData (4)

This command expects to receive a body of:

```
struct ChunkHdr {
    uint32_t offset; /* The byte sequence start, (0 based). */
};
```

However, this data is presently ignored.

Immediately following this structure are the bytes to write. The length of the
entire packet is variable and handled at a higher level, therefore the number
of bytes to write is the size of the command body less the sub-command (1 byte)
and less the structure size (4 bytes).

On success it will return 1 byte of `0x00`, in the body, after the 3-byte OEM
portion.

### FlashHashFinish (5)

This command expects the body to be empty.

On success it will return 1 byte of `0x00`, in the body, after the 3-byte OEM
portion.

### FlashDataVerify (6)

This command expects the body to be empty.

This will start the `verify_image` systemd service.

On success it will return 1 byte of `0x00`, in the body, after the 3-byte OEM
portion.

### FlashAbort (7)

This command expects the body to be empty.

This command deletes any temporary files or flash image, hashes, etc.

On success it will return 1 byte of `0x00`, in the body, after the 3-byte OEM
portion.

### FlashVerifyCheck (8)

This command expects the body to be empty.

This command opens the verification result file and checks to see if it
contains: "`running`", "`success`" or "`failed`". This then packs that as a 1
byte of result of:

```
enum VerifyCheckResponses {
    VerifyRunning = 0x00,
    VerifySuccess = 0x01,
    VerifyFailed  = 0x02,
    VerifyOther   = 0x03,
};
```

### FlashVersion (9)

This command expects the body to be empty.

Returns 16-bit version sequence (little endian), in the body, after the 3-byte
OEM portion.

### FlashRequestRegion (10)

Should **_return type string "LPC" or "P2A"_** but does not yet.  We need to
add this into another command, such as **_FlashRegionSupportedType_**

returns 32-bit address (little endian) of either the BMC's memory buffer in BAR
space (P2A bridge) or in LPC FW space (LPC bridge).

### FlashDataExtBlock (11)

This command expects to receive:

```
struct ExtChunkHdr {
    uint32_t length; /* Length of the data queued (little endian). */
};
```

A design limitation of this is that it expects to be able to copy and write the
data before the IPMI command times out, which should be completely possible.

On success it will return 1 byte of `0x00`, in the body, after the 3-byte OEM
portion.

### FlashHashExtData (12)

This command expects to receive:

```
struct ExtChunkHdr {
    uint32_t length; /* Length of the data queued (little endian). */
};
```

A design limitation of this is that it expects to be able to copy and write the
data before the IPMI command times out, which should be completely possible.

On success it will return 1 byte of `0x00`, in the body, after the 3-byte OEM
portion.

### FlashMapRegionLpc (13)

This command maps a chunk (size specified by host) of the BMC memory into LPC
space at a specified address. It expects to receive:

```
struct LpcRegion {
    uint32_t address; /* Host LPC address where the chunk is to be mapped. */
    uint32_t length; /* Size of the chunk to be mapped. */
};
```

The command will return 1 byte in the response body with value:

*   0x00 on success (1 byte)
*   0x16 (EINVAL) if the BMC is not expecting an LPC bridge (1 byte)
*   0x1B (EFBIG) if the length argument is larger than the BMC can map at the
    requested address, followed by a 32-bit "length" payload representing the
    maximal buffer size that BMC can map. (5 bytes)

After a success map to LPC space, the host can use FlashRequestRegion to read
the LPC address of the chunk mapped, which should match that requested.
