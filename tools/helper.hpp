#pragma once

#include <chrono>
#include <cstdint>
#include <ipmiblob/blob_interface.hpp>

namespace host_tool
{

/**
 * Poll an open verification session.
 *
 * @param[in] session - the open verification session
 * @param[in] blob - pointer to blob interface implementation object.
 * @param[in] timeout - maximum time in seconds to wait for status.
 * @return true if the verification was successul.
 */
bool pollStatus(std::uint16_t session, ipmiblob::BlobInterface* blob,
                std::chrono::seconds timeout);

} // namespace host_tool
