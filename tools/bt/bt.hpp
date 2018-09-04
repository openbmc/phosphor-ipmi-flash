#pragma once

#include "interface.hpp"
#include "updatehelper.hpp"

class BtDataHandler : public DataInterface
{
  public:
    explicit BtDataHandler(UpdateHelperInterface* helper) : helper(helper)
    {
    }

    void SendData(std::ifstream input, int command) override;

    bool External() override
    {
        return false;
    }

  private:
    UpdateHelperInterface* helper;
};
