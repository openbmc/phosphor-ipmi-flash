#pragma once

#include "status.hpp"
#include "verify.hpp"

#include <memory>
#include <vector>

#include <gtest/gtest.h>

namespace ipmi_flash
{

class VerificationMock : public VerificationInterface
{
  public:
    MOCK_METHOD0(triggerVerification, bool());
    MOCK_METHOD0(abortVerification, void());
    MOCK_METHOD0(status, VerifyCheckResponses());
};

std::unique_ptr<VerificationInterface> CreateVerifyMock()
{
    return std::make_unique<VerificationMock>();
}

} // namespace ipmi_flash
