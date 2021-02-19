#pragma once

#include <ipmiblob/blob_interface.hpp>

#include <cstdint>

namespace host_tool
{

/**
 * Poll an open verification session.
 *
 * @param[in] session - the open verification session
 * @param[in] blob - pointer to blob interface implementation object.
 * @return true if the verification was successul.
 */
bool pollStatus(std::uint16_t session, ipmiblob::BlobInterface* blob);

/**
 * Aligned memcpy
 * @param[out] destination - destination memory pointer
 * @param[in] source - source memory pointer
 * @param[in] size - bytes to copy
 * @return destination pointer
 */
void* memcpyAligned(void* destination, const void* source, size_t size);

} // namespace host_tool
