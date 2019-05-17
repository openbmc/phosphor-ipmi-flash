#pragma once

#include "status.hpp"
#include "verify.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <vector>

namespace blobs
{

class VerificationMock : public VerificationInterface {
public:
  MOCK_METHOD0(triggerVerification, bool());
  MOCK_METHOD0(abortVerification, void());
  MOCK_METHOD0(checkVerificationState, VerifyCheckResponses());\
};

std::unique_ptr<VerificationInterface> CreateVerifyMock() {
    return std::make_unique<VerificationMock>();
}

}
