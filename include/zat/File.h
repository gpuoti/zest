#ifndef ZEST_ZAT_ENTRY_H
#define ZEST_ZAT_ENTRY_H

#include <string>
#include <cstdint>
#include <cstdint>

namespace zest { namespace zat {

struct ContentBlock {
  std::uint32_t start_at;
  std::uint32_t ends_at;

  ContentBlock(
    const uint32_t block_start, 
    const uint32_t block_end )

  : start_at(block_start),
    ends_at(block_end)
  {}

  ContentBlock(const uint32_t block_start)
  : start_at(block_start),
    ends_at(block_start)
  {}

  ContentBlock (ContentBlock&& oth) 
  : start_at( std::move(oth.start_at)),
    ends_at(std::move(oth.ends_at))
  {}

  bool operator==(const ContentBlock& rho) const {
    return start_at == rho.start_at && ends_at == rho.ends_at;
  };

  bool operator !=(const ContentBlock& rho) const {
    return (start_at < rho.start_at && ends_at < rho.start_at)
            || (start_at > rho.start_at && ends_at > rho.start_at);
  }

  bool unbounded() const {
    return ends_at == std::numeric_limits<std::uint32_t>::max();
  }

  const uint32_t size() const {
    return ends_at - start_at;
  }
};

inline std::ostream& operator << (std::ostream& os, ContentBlock const& block) {
  os << "ContentBlock [" << block.start_at << ", " <<block.ends_at << "]";
  return os;
}

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

  bool operator== (const FileInfo& rho) const {
    return content_block.start_at == rho.content_block.start_at;
  }

  ContentBlock substitute_block (ContentBlock&& new_block) {
    ContentBlock free_block = content_block;
    std::swap(content_block, std::move(new_block) );
    
    return free_block;
  }

};    

}}

#endif