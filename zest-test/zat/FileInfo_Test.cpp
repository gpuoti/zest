#include "pch.h"
#include "catch.hpp"
#include "zat/File.h"

using namespace zest;

SCENARIO(""){
  GIVEN("The ZAT descriptor for an empty and still unbounded file") {
    zat::FileInfo finfo {"new_unbounded", zat::ContentBlock { 1024 }, 0};

  }
}
