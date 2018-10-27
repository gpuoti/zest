#ifndef ZEST_ZAT_H
#define ZEST_ZAT_H

#include <cstdint>
#include <vector>
#include <list>
#include <algorithm>
#include "zat/File.h"
#include "zat/Fragment.h"


namespace zest {
  struct ZAT {
    // empty_blocks

    std::vector<zat::FileInfo> files;
    std::list<zat::Fragment> fragments;
    std::uint32_t fs_size = 0;

    zat::FileInfo create (std::string path ){
      zat::Fragment& zat_fragment = find_space_on_fragments_for(path);
      files.push_back( zat_fragment.make_file_info(path, zat::ContentBlock {fs_size}) ) ;

      return files.back();
    }

    bool exists(std::string file_path) const  {
      return std::find( std::begin(files), std::end(files), file_path) 
              != std::end(files);
    }

  private:
    zat::Fragment& find_space_on_fragments_for( std::string file_path){
      
      auto fragment_it = std::find_if ( 
        std::begin(fragments), std::end(fragments), 
        [file_path](const zat::Fragment& f){ 
          return f.has_space_for(file_path);
        } );

      if(fragment_it == fragments.end() ){
        fragments.push_front( zat::Fragment(fs_size) );
        fs_size += zat::Fragment::size;
        fragment_it = fragments.begin();  
      }

      return *fragment_it;
    }
  };
}

#endif