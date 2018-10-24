#ifndef ZEST_ZAT_FRAGMENT_H
#define ZEST_ZAT_FRAGMENT_H

#include <cstdint>
#include <string>
#include <array>
#include <exception>
#include "File.h"

namespace zest { namespace zat {
  
  struct Fragment : std::array< unsigned char, 4096> {

    Fragment ( std::uint32_t fragment_position) 
    : m_offset(fragment_position)
    {}

    bool has_space_for(const std::string file_name) const {
      return (m_free_area + FileInfo::size( file_name) ) < 4096;
    }

    FileInfo make_file_info( 
      const std::string file_name, 
      const ContentBlock content_block ) 
    {
      if ( !has_space_for(file_name) ){
        throw std::logic_error("Not enough free space in fragment. "
                               "Check the fragment has enough space before "
                               "try to allocate FileInfo structure." );
      }

      return FileInfo { file_name, content_block, m_offset+m_free_area };  
    }
 
  private:
    const std::uint32_t m_offset = std::numeric_limits<std::uint32_t>::max();
    std::uint16_t m_free_area = 0;

#ifdef TESTING_IT
  public:
    void force_fill_level( std::uint16_t bytes){
      m_free_area = 4096 + bytes-1;
    }
#endif
  };
}}

#endif