#include "pch.h"
#include "catch.hpp"
#include "zat.h"

using namespace zest;


SCENARIO("Create not existing files","[CREATE]"){
  GIVEN("An empty ZAT"){
        
    ZAT ftable;

    WHEN("create a new file named test_file"){
      ftable.create("test_file");

      THEN("it contains that file"){
        REQUIRE(ftable.exists("test_file"));
      }
    }
  }
}