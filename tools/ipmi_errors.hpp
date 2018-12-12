#pragma once

#include <exception>
#include <sstream>
#include <string>

class IpmiException : public std::exception
{
  public:
    explicit IpmiException(int cc)
    {
        std::ostringstream smessage;
        smessage << "Received IPMI_CC: " << cc;
        message = smessage.str();
    }

    virtual const char* what() const noexcept override
    {
        return message.c_str();
    }

  private:
    std::string message;
};
