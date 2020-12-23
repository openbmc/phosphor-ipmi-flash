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

/** @brief Lightweight class wrapper that removes move operations from a class
 *         in order to guarantee the contents stay pinned to a specific location
 *         in memory.
 */
template <typename T>
struct Pinned : public T
{
    template <typename... Args>
    Pinned(Args&&... args) : T(std::forward<Args>(args)...)
    {}
    template <typename Arg>
    Pinned& operator=(const Arg& o)
    {
        *static_cast<T*>(this) = o;
        return *this;
    }

    Pinned(Pinned&&) = delete;
    Pinned& operator=(Pinned&&) = delete;
};

} // namespace ipmi_flash
