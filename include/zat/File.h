#ifndef ZEST_ZAT_ENTRY_H
#define ZEST_ZAT_ENTRY_H

#include <string>
#include <cstdint>

namespace zest { namespace zat {

struct ContentBlock {
  const std::uint32_t start_at;
  const std::uint32_t ends_at;

  bool operator==(const ContentBlock& rho) const {
    return start_at == rho.start_at && ends_at == rho.ends_at;
  };
};

struct FileInfo {
  const std::string name;
  ContentBlock content_block;
  const std::uint32_t entry_offset;


  static std::uint16_t size( std::string n) {
    return n.size() + sizeof(ContentBlock) + sizeof(std::uint32_t);
  }
};    

}}

#endif