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
      auto finfo = ftable.create("test_file");

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
      THEN("the device has the file name and its content block reference stored in finfo.entryoffset") {
        std::uint16_t finfo_sz;
        test_device.read_into_from((char*)&finfo_sz, finfo.entry_offset, sizeof(std::uint16_t));

        REQUIRE(finfo_sz == (9 + 8) );

        std::uint16_t fname_sz = finfo_sz = 2 * sizeof(std::uint32_t);
        char fname[255];
        test_device.read_into_from(fname, finfo.entry_offset + 1, fname_sz);
        fname[fname_sz] = 0; 
        REQUIRE( std::string(fname) == "test_file");

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

  GIVEN("A ZAT binded to a zest debug device with one file already stored ") {
    zest::test::RamDsk test_device;
    ZAT ftable(test_device);
    const ZAT& query_ftable = ftable;

    std::string existing_content = "some stuff in the existing file";
    ftable.create("existing_file");
    {
      auto w = ftable.writer("existing_file");
      w.write(existing_content.c_str(), existing_content.size());
    }

    WHEN("I create another file and put some stuff into it") {
      ftable.create("test_file");
      auto w = ftable.writer("test_file");

      w.write("some test stuff", std::strlen("some test stuff"));

      THEN("their content block are different") {
        REQUIRE(query_ftable.info("test_file").content_block != query_ftable.info("existing_file").content_block);
      }

      THEN("the  previously exising file still hold its content") {
        auto existing_file = query_ftable.info("existing_file");
        REQUIRE(test_device.read(existing_file.content_block) == existing_content);
      } 
    }
  }
}

SCENARIO("Read a file content using a stream like interface ") {
  GIVEN("A ZAT created on a test_device with a simple test file on it") {
    test::RamDsk test_device;
    ZAT ftable(test_device);
    const ZAT& query_ftable = ftable;

    ftable.create( "test_file");

    {
      auto w = ftable.writer("test_file");
      w.write("Insert some data into the test_file.\n", std::strlen("Insert some data into the test_file.\n") );
      w.write("And some more.", std::strlen("And some more."));
    }
    auto finfo = query_ftable.info("test_file");

    GIVEN("a file reader for the test_file") {
      auto r = query_ftable.reader("test_file");
      WHEN("ask for 6 byte of data") {
        char fdata[6];
        r.read_into( fdata, 6 );

        THEN("I get the string: Insert") {
          std::string read_string (fdata, fdata +6);
          REQUIRE( read_string == "Insert");
        }

        AND_WHEN("I read one more byte, discard it, and read 4 more") {
          char discarded[1];
          r.read_into(discarded, 1);
          char more_data[4];
          r.read_into(more_data, 4);

          THEN("I get the string: some") {
            std::string read_string (more_data, more_data + 4);
            REQUIRE(read_string == "some");
          }
        }
      }
      WHEN("I read the complete file size") {
        char mmap_file[4096];
        r.read_into( mmap_file, finfo.content_block.size());
        THEN("I get the complete file content into the buffer") {
          std::string file_content (mmap_file, mmap_file + finfo.content_block.size());
          REQUIRE(file_content == "Insert some data into the test_file.\nAnd some more.");
        }
      }
    }
  }
}