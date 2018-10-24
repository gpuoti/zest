#ifndef ZEST_ZAT_H
#define ZEST_ZAT_H

#include <cstdint>
#include <vector>
#include "zat/File.h"

namespace zest {
  struct ZAT {
    // empty_blocks
    std::vector<FileInfo> stored_files;
  };
}

#endif