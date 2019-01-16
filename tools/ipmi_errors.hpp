#pragma once

#include <exception>
#include <map>
#include <sstream>
#include <string>

namespace host_tool
{

class IpmiException : public std::exception
{
  public:
    const std::map<int, std::string> commonFailures = {
        {0xc0, "busy"},
        {0xc1, "invalid"},
        {0xc3, "timeout"},
    };

    explicit IpmiException(int cc)
    {
        std::ostringstream smessage;

        auto search = commonFailures.find(cc);
        if (search != commonFailures.end())
        {
            smessage << "Received IPMI_CC: " << search->second;
        }
        else
        {
            smessage << "Received IPMI_CC: " << cc;
        }

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
