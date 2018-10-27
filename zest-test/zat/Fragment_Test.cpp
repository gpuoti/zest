#include "pch.h"
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "zat/Fragment.h"

using namespace zest::zat;
SCENARIO (
"Use ZAT Fragment to store existing file pointer",
"[FRAGMENT]" )
{
  GIVEN ( "an empty ZAT fragment at position 0" ) {
    Fragment f(0);
    
    THEN( "it has enough space to store an entry for a file named: this/is/a/test"){
      CHECK(f.has_space_for("this/is/a/test"));

      AND_THEN( "it can store a reference to that file provided that is already "
                "designed to be stored into the segment starting from 2048 with "
                "a size of 123 byte")
      {
        auto file_info = f.make_file_info( "this/is/a/test", ContentBlock { 2048, 123 } );
        CHECK(file_info.name == "this/is/a/test" );
        CHECK(file_info.content_block == ContentBlock (2048, 123) );
        
        AND_THEN("the file_info is stored at the very beginning of the file"){
          CHECK( file_info.entry_offset == 0);
        }

        AND_WHEN("an additional file named this/is/a/second/test whose size is " 
                 "211 byte is created and stored in the ZAT")
        {
          CHECK(f.has_space_for("this/is/a/second/test"));
          auto second_file_info = f.make_file_info("this/is/a/second/test", { 2048+123, 211} );
          THEN("the fragment has still enought space to store the entry"){
            CHECK(second_file_info.name == "this/is/a/second/test" );
            CHECK(second_file_info.content_block == ContentBlock (2048+123, 211 ));
          }
        }
      }
    }
  }
}

SCENARIO (
"Use Zat fragment with not enough space to try to store a new file descriptor",
"[FRAGMENT]" )
{
  GIVEN("a completelly full ZAT fragment at the very beginning of the file" )
  {
    Fragment f(0);
    f.force_fill_level(4096);

    THEN("it cannot store any more FileInfo object (even if it has no name at all)")
    {
      CHECK(!f.has_space_for(""));
      REQUIRE_THROWS_AS(f.make_file_info ("", ContentBlock (2048, 123) ), std::logic_error );
    }
  }

  GIVEN("a almost full (4 byte left) ZAT fragment at the very beginning of the file" )
  {
    Fragment f(0);
    f.force_fill_level(-4);

    THEN("it cannot store any more FileInfo object (even if it has no name at all)")
    {
      CHECK(!f.has_space_for(""));
      REQUIRE_THROWS_AS(f.make_file_info ("", ContentBlock (2048, 123) ), std::logic_error );
    }
  }

  GIVEN("a almost full (12 byte left) ZAT fragment at the very beginning of the file" )
  {
    Fragment f(0);
    f.force_fill_level(-12);

    THEN("it cannot store any more FileInfo object with a name as short as 1 char")
    {
      CHECK(!f.has_space_for("x"));
      REQUIRE_THROWS_AS(f.make_file_info ("x", ContentBlock (2048, 123) ), std::logic_error );
    }
  }
}