#pragma once

#include "updater.hpp"

#include <cstdint>
#include <string>
#include <vector>

#include <gmock/gmock.h>

namespace host_tool
{

class UpdateHandlerMock : public UpdateHandlerInterface
{
  public:
    MOCK_METHOD(bool, checkAvailable, (const std::string&), (override));
    MOCK_METHOD(std::vector<uint8_t>, readVersion, (const std::string&),
                (override));
    MOCK_METHOD(void, sendFile, (const std::string&, const std::string&),
                (override));
    MOCK_METHOD(bool, verifyFile, (const std::string&, bool, (override));
    MOCK_METHOD(void, cleanArtifacts, (), (override));
};

} // namespace host_tool
