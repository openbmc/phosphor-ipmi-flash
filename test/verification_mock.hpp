#pragma once

#include "status.hpp"
#include "verify.hpp"

#include <gtest/gtest.h>

#include <vector>

namespace blobs
{

class VerificationMock : public VerificationInterface {
public:
  MOCK_METHOD0(triggerVerification, bool());
  MOCK_METHOD0(abortVerification, void());
  MOCK_METHOD0(checkVerificationState, VerifyCheckResponses());\
};
}
