#ifndef ZEST_ZAT_H
#define ZEST_ZAT_H

#include <cstdint>
#include <vector>
#include <list>
#include <algorithm>
#include "zat/File.h"
#include "zat/Fragment.h"


namespace zest {

struct Device {
  virtual void write_at(const char* buf, const std::uint32_t sz, const uint32_t at) = 0;
  virtual void read_into_from(const char* buf, const uint32_t at, const std::uint32_t sz) = 0;
  
  void copy(const zat::ContentBlock& src, const zat::ContentBlock& dest) {
    char buf[1024*2];
    const std::uint32_t sz = src.size();

    read_into_from( buf, src.start_at, sz );
    write_at(buf, sz, dest.start_at);
  }
};

  struct ZAT {
    // empty_blocks

    std::vector<zat::FileInfo> files;
    std::list<zat::Fragment> fragments;
    std::uint32_t fs_size = 0;
    Device& m_device;

    ZAT (Device& device) : m_device(device) {}

    zat::FileInfo create (std::string path ){
      if (exists(path)) {
        throw std::logic_error("file already exists");
      }
      zat::Fragment& zat_fragment = find_space_on_fragments_for(path);
      files.push_back( zat_fragment.make_file_info(path, zat::ContentBlock {fs_size}) ) ;

      return files.back();
    }

    bool exists(const std::string file_path) const  {
      return std::find( std::begin(files), std::end(files), file_path) 
              != std::end(files);
    }

    const zat::FileInfo& info(std::string file_path) const {
      auto finfo = std::find(std::begin(files), std::end(files), file_path);
      return *finfo;
    }

    struct FileWriter {
      ZAT& m_fs;

      FileWriter(ZAT& fs, zat::FileInfo& file_info)
      : m_fs(fs),
        m_file_info(file_info)
      {}

      FileWriter& write(const char* buf, std::uint32_t sz) {

        m_fs.append_to( buf, sz, m_file_info);
        return *this;
      }

    private:
      const zat::FileInfo& m_file_info;
    };

    struct FileReader {
      FileReader(const ZAT& fs, const zat::FileInfo& file_info)
      : m_fs(fs), 
        m_file_info(file_info)
      {}

      void read_into(char* buf, std::uint32_t sz) {
        m_fs.m_device.read_into_from( buf, m_file_info.content_block.start_at + m_current_pos, sz);
        m_current_pos += sz;
      }

    private:
      const zat::FileInfo& m_file_info;
      const ZAT& m_fs;
      std::uint32_t m_current_pos = 0;
    };

    FileWriter writer( const std::string fname ) { 
      return FileWriter(*this, info( fname) ); 
    }

    FileReader reader(const std::string fname) const {
      return FileReader(*this, info(fname));
    }

    void append_to(const char* buf, std::uint32_t sz, const zat::FileInfo file_info) {
      auto& finfo = info(file_info);
      std::uint32_t orig_sz = finfo.content_block.size();

      auto extended_block = allocate_content_block( orig_sz + sz );
      m_device.copy(finfo.content_block, extended_block);
      finfo.substitute_block( std::move( extended_block ) );
      m_device.write_at( buf, sz, finfo.content_block.start_at + orig_sz );
    }

  private:

    zat::FileInfo& info(const std::string file_path) {
      auto finfo = std::find(std::begin(files), std::end(files), file_path);
      return *finfo;
    }

    zat::FileInfo& info(const zat::FileInfo file_info) {
      auto finfo = std::find(std::begin(files), std::end(files), file_info);
      return *finfo;
    }

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

    zat::ContentBlock allocate_content_block( const std::uint32_t sz) {
      fs_size += sz;
      return zat::ContentBlock( fs_size-sz, fs_size);
    }
  };
}

#endif