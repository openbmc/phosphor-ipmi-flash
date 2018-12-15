#pragma once

#include <exception>
#include <sstream>
#include <string>

namespace host_tool
{

class IpmiException : public std::exception
{
  public:
    explicit IpmiException(int cc)
    {
        std::ostringstream smessage;
        smessage << "Received IPMI_CC: " << cc;
        message = smessage.str();
    }
    explicit IpmiException(const std::string& message) : message(message){};

    virtual const char* what() const noexcept override
    {
        return message.c_str();
    }

  private:
    std::string message;
};

} // namespace host_tool
