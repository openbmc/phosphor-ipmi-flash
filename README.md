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
