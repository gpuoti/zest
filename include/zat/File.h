#ifndef ZEST_ZAT_ENTRY_H
#define ZEST_ZAT_ENTRY_H

#include <string>
#include <cstdint>
#include <cstdint>

namespace zest { namespace zat {

struct ContentBlock {
  const std::uint32_t start_at;
  const std::uint32_t ends_at;

  ContentBlock(
    const uint32_t block_start, 
    const uint32_t block_end = std::numeric_limits<std::uint32_t>::max() )

  : start_at(block_start),
    ends_at(block_end)
  {}

  bool operator==(const ContentBlock& rho) const {
    return start_at == rho.start_at && ends_at == rho.ends_at;
  };

  bool operator !=(const ContentBlock& rho) const {
    return ! operator==(rho);
  }

  bool unbounded() const {
    return ends_at == std::numeric_limits<std::uint32_t>::max();
  }
};

struct FileInfo {
  const std::string name;
  ContentBlock content_block;
  const std::uint32_t entry_offset;

  static std::uint16_t size( std::string n) {
    return n.size() + sizeof(ContentBlock) + sizeof(std::uint32_t);
  }

  bool operator== (const std::string file_name) const {
    return name == file_name;
  }
};    

}}

#endif