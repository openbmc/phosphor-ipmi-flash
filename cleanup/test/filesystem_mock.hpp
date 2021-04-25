#pragma once

#include "fs.hpp"

#include <string>

#include <gmock/gmock.h>

namespace ipmi_flash
{

class FileSystemMock : public FileSystemInterface
{
  public:
    MOCK_METHOD(void, remove, (const std::string&), (const, override));
};
} // namespace ipmi_flash
