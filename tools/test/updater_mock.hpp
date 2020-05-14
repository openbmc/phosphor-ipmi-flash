#pragma once

#include "updater.hpp"

#include <chrono>
#include <string>

#include <gmock/gmock.h>

namespace host_tool
{

class UpdateHandlerMock : public UpdateHandlerInterface
{
  public:
    MOCK_METHOD1(checkAvailable, bool(const std::string&));
    MOCK_METHOD2(sendFile, void(const std::string&, const std::string&));
    MOCK_METHOD3(verifyFile,
                 bool(const std::string&, bool, std::chrono::seconds));
    MOCK_METHOD0(cleanArtifacts, void());
};

} // namespace host_tool
