#ifndef ZEST_ZAT_ENTRY_H
#define ZEST_ZAT_ENTRY_H

#include <string>
#include <cstdint>
#include <cstdint>

#include "ContentBlock.h"

namespace zest { namespace zat {

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