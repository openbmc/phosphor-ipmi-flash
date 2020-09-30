#pragma once

namespace ipmi_flash
{

inline constexpr char biosBlobId[] = "/flash/bios";
inline constexpr char updateBlobId[] = "/flash/update";
inline constexpr char verifyBlobId[] = "/flash/verify";
inline constexpr char hashBlobId[] = "/flash/hash";
inline constexpr char activeImageBlobId[] = "/flash/active/image";
inline constexpr char activeHashBlobId[] = "/flash/active/hash";
inline constexpr char staticLayoutBlobId[] = "/flash/image";
inline constexpr char ubiTarballBlobId[] = "/flash/tarball";
inline constexpr char cleanupBlobId[] = "/flash/cleanup";

} // namespace ipmi_flash
