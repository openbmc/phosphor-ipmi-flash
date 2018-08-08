#pragma once

#include "interface.hpp"

class MockData : public DataInterface
{
  public:
    MOCK_METHOD2(SendData, void(std::FILE*, int));
    MOCK_METHOD0(External, bool());
};
