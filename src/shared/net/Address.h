#pragma once
#include <string>
#include <cstdint>

namespace Net
{
  struct Address
  {
    std::string host;
    uint16_t port;
  };
}