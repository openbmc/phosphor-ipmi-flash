# Secure Flash Update Mechanism

This document describes the OpenBmc software implementing the secure flash
update mechanism.

The primary details are [here](https://github.com/openbmc/docs/blob/master/designs/firmware_update_via_blobs.md).

## Introduction

This supports two methods of providing the image to stage. You can send the
file over IPMI packets, which is a very slow process. A 32-MiB image can take
~3 hours to send via this method.  This can be done in <1 minutes via the PCI
bridge, or just a few minutes via LPC depending on the size of the mapped area.

This is implemented as a phosphor blob handler.

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

## Bridge Options

### P2A

The PCI-to-AHB bridge is only available on some systems, and provides a 64-KiB
region that can be pointed anywhere in BMC memory space.

It is controlled by two PCIe MMIO addresses that are based on BAR1. Further
specifics can be found in the ASPEED data sheet. However, the way it works in
this instance is that the BMC has configured a region of physical memory it
plans to use as this buffer region. The BMC returns the address to host program
so it can configure the PCIe MMIO registers properly for that address.

As an example, a 64-KiB region a platform can use for this
approach is the last 64-KiB of the VGA reserved region: `0x47ff0000`.

### LPC

Like the P2A mechanism, the BMC must have already allocated a memory region
with a platform-defined size. This region is mapped to LPC space at the request
of the host, which also specifies the LPC address and region size in the
mapping request subcommand. The host can then read the _actual_ LPC address at
which the BMC was able to map, checking that it's usable.

**Note: Not yet implemented for Stat metadata**
The command will return 1 byte in the response body with value:

*   0x00 on success (1 byte)
*   0x16 (EINVAL) if the BMC is not expecting an LPC bridge (1 byte)
*   0x1B (EFBIG) if the length argument is larger than the BMC can map at the
    requested address, followed by a 32-bit "length" payload representing the
    maximal buffer size that BMC can map. (5 bytes)

After a success map to LPC space, the host can use FlashRequestRegion to read
the LPC address of the chunk mapped, which should match that requested.

### USB

As future work, USB should be considered an option for staging the image to be
copied to the BMC.

