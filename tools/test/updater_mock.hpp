#pragma once

#include "updater.hpp"

#include <string>

#include <gmock/gmock.h>

namespace host_tool
{

class UpdateHandlerMock : public UpdateHandler
{
  public:
    MOCK_METHOD1(checkAvailable, bool(const std::string&));
    MOCK_METHOD2(sendFile, void(const std::string&, const std::string&));
    MOCK_METHOD1(verifyFile, bool(const std::string&));
};

} // namespace host_tool
