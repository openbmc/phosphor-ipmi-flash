#pragma once

#include "interface.hpp"

class LpcDataHandler : public DataInterface
{

  public:
    LpcDataHandler() = default;

    bool sendContents(const std::string& input, std::uint16_t session) override;
};
