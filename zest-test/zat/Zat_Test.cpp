#include "pch.h"
#include "catch.hpp"
#include "zat.h"

using namespace zest;

namespace zest {
namespace test {



struct RamDsk : std::array<char, 1024 * 16>, zest::Device {
  using mem_type = std::array<char, 1024 * 16>;

  void write_at(const char* buf, const std::uint32_t sz, const uint32_t where) override {
    std::copy( buf, buf+sz, &at(where) );
  }
  void read_into_from(const char* buf, const uint32_t where, const std::uint32_t sz) override{
    memcpy((void*)buf, &at(where), sz);
  }

  std::string read(const zat::ContentBlock block) {
    std::string data;

    std::copy( begin() + block.start_at, begin() + block.ends_at, std::back_inserter(data) );
    return data;
  }
};

}}

SCENARIO("Create not existing files","[CREATE]"){
  GIVEN("An empty ZAT, binded to a zest debug device"){
        
    zest::test::RamDsk test_device;
    ZAT ftable(test_device);
    const ZAT& query_ftable = ftable;

    WHEN("create a new file named test_file"){
      ftable.create("test_file");

      THEN("it contains that file"){
        REQUIRE(ftable.exists("test_file"));
      }
      THEN("the zat table can provide a FileInfo with the information needed to "
           "find both the file content and the descriptor position in the ZAT")
      {
        
        auto& finfo = query_ftable.info("test_file");
        REQUIRE(finfo.name == "test_file");
        REQUIRE(finfo.content_block.start_at > 0);
      }

      AND_WHEN("create another new file with name second_test") {
        ftable.create("second_test");

        THEN("they both exist") {
          REQUIRE(ftable.exists("test_file"));
          REQUIRE(ftable.exists("second_test"));
        }
      }

      AND_WHEN("try to create another file with the same name") {
        THEN("it raise a logical error"){
          REQUIRE_THROWS_AS( ftable.create("test_file"), std::logic_error);
        }
      }

     
    }
  }
}

SCENARIO("Write a file on a mini-ramdisk", "[CREATE]") {
  GIVEN("An empty ZAT, binded to a zest debug device") {

    zest::test::RamDsk test_device;
    ZAT ftable(test_device);
    const ZAT& query_ftable = ftable;

    WHEN("I create a file 'test_file' and ask for a writer on it") {
      ftable.create("test_file");
      auto fwriter = ftable.writer("test_file");

      THEN("I can request a writer for it ") {
        REQUIRE_NOTHROW(ftable.writer("test_file"));
      }
      // move this in proper test case for file writing
      AND_WHEN("I request a writer for the created test_file and write 'some stuff' ") {
        auto writer = ftable.writer("test_file");
        writer.write("some stuff", std::strlen("some stuff"));

        THEN("The file content is not empty as for the descriptor") {
          auto finfo = query_ftable.info("test_file");
          REQUIRE(finfo.content_block.size() == std::strlen("some stuff"));

          AND_THEN("the device is holding the correct data in the position " 
                   "reported in the info structure") 
          {
            std::string expected_string ("some stuff");
            std::string actual_string = test_device.read( finfo.content_block );
            
            REQUIRE(actual_string == expected_string);
          }

          AND_WHEN("I write ' and some more stuff'") {
            writer.write(" and some more stuff", std::strlen(" and some more stuff"));

            THEN("the expected file includes both the string contatenated") {
              std::string expected_text = "some stuff and some more stuff";
              auto finfo = query_ftable.info("test_file");
              REQUIRE(finfo.content_block.size() == expected_text.size());
              std::string actual_text = test_device.read( finfo.content_block);
              REQUIRE( actual_text == expected_text);
            }
          }
        }


      }

    }
  }
}