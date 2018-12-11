#pragma once

#include "interface.hpp"

class BtDataHandler : public DataInterface
{
  public:
    BtDataHandler() = default;

    bool sendContents(const std::string& input, std::uint16_t session) override;
};
