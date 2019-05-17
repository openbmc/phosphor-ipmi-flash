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

## Configuration

To use `phosphor-ipmi-flash` a platform must provide a configuration.  A
platform can configure multiple interfaces, such as both lpc and pci.  However,
a platform should only configure either static layout updates, or ubi.  If
enabling lpc, the platform must specify either aspeed or nuvoton.

The following are the two primary configuration options, which control how the
update is treated.

Option                   | Meaning
------------------------ | -------
`--enable-static-layout` | Enable treating the update as a static layout update.
`--enable-tarball-ubi`   | Enable treating the update as a tarball for UBI update.

The following are configuration options for how the host and BMC are meant to
transfer the data.  By default, the data-in-IPMI mechanism is enabled.

There are two configurable data transport mechanisms, either staging the bytes
via the LPC memory region, or the PCI-to-AHB memory region.  Because there is
only one `MAPPED_ADDRESS` variable at present, a platform should only configure
one.  The platform's device-tree may have the region locked to a specific
driver (lpc-aspeed-ctrl), preventing the region from other use.

***NOTE:*** It will likely be possible to configure both in the near future.

Variable              | Default | Meaning
--------------------- | ------- | -------
`MAPPED_ADDRESS`      | 0       | The address used for mapping P2A or LPC into the BMC's memory-space.

Option                | Meaning
--------------------- | -------
`--enable-pci-bridge` | Enable the PCI-to-AHB transport option.
`--enable-lpc-bridge` | Enable the LPC-to-AHB transport option.

If a platform enables p2a as the transport mechanism, a specific vendor must be
selected via the following configuration option.  Currently, only one is
supported.

Option                 | Meaning
-----------------------| -------
`--enable-aspeed-p2a`  | Use with ASPEED parts.

If a platform enables lpc as the transport mechanism, a specific vendor must be
selected via the following configuration option.  Currently, only two are
supported.

Option                 | Meaning
---------------------- | -------
`--enable-aspeed-lpc`  | Use with ASPEED parts.
`--enable-nuvoton-lpc` | Use with Nuvoton parts.

### Internal Configuration Details

The following variables can be set to whatever you wish, however they have
usable default values.

Variable                     | Default                    | Meaning
---------------------------- | -------------------------- | -------------------------------------------------------------------------
`STATIC_HANDLER_STAGED_NAME` | `/run/initramfs/bmc-image` | The filename where to write the staged firmware image for static updates.
`TARBALL_STAGED_NAME`        | `/tmp/image-update.tar`    | The filename where to write the UBI update tarball.
`HASH_FILENAME`              | `/tmp/bmc.sig`             | The file to use for the hash provided.
`VERIFY_STATUS_FILENAME`     | `/tmp/bmc.verify`          | The file checked for the verification status.
