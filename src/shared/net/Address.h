#pragma once
#include <string>
#include <cstdint>

namespace shared::net
{
  struct Address
  {
    std::string host;
    uint16_t port;
  };
}