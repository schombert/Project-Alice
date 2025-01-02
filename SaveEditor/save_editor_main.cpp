#include <cstdio>
#include <cstring>
#include <string_view>
#include <algorithm>

// Forward declarations:
//   - open_file, simple_fs::view_contents, dcon::for_each_record, etc.
//   - We assume these exist as in your code.

namespace {

// Utility function to safely dump hex around a mismatch
void printHexContext(
  std::byte const* dataStart,
  std::byte const* dataEnd,
  std::byte const* mismatchPos)
{
  // We'll dump 8 bytes before and after the mismatch, within bounds
  constexpr int contextSize = 8;
  auto totalSize = dataEnd - dataStart;
  auto mismatchOffset = mismatchPos - dataStart;

  // Start offset
  auto startOffset = mismatchOffset > contextSize 
                    ? (mismatchOffset - contextSize) 
                    : 0;
  // End offset
  auto endOffset = std::min(mismatchOffset + contextSize, totalSize);

  std::printf("Mismatch at offset %zu. Hex context:\n<", size_t(mismatchOffset));
  for (auto i = startOffset; i < endOffset; i++) {
    std::printf("%02x ", static_cast<unsigned char>(dataStart[i]));
  }
  std::printf(">\n");
}

// Compares two records for data and size
void compareRecords(
  dcon::record_header const& header1,
  std::byte const* dataStart1, 
  std::byte const* dataEnd1,
  dcon::record_header const& header2,
  std::byte const* dataStart2, 
  std::byte const* dataEnd2,
  uint8_t const* fileStart1,
  uint8_t const* fileStart2)
{
  auto size1 = dataEnd1 - dataStart1;
  auto size2 = dataEnd2 - dataStart2;
  auto offset1 = dataStart1 - reinterpret_cast<const std::byte*>(fileStart1);
  auto offset2 = dataStart2 - reinterpret_cast<const std::byte*>(fileStart2);

  bool match = true;
  // Check size mismatch
  if (size1 != size2) {
    std::printf("%s.%s.%s: Size mismatch (%u/%u)\n", 
      header1.object_name_start, 
      header1.property_name_start, 
      header1.type_name_start,
      static_cast<unsigned int>(offset1), 
      static_cast<unsigned int>(offset2));
    match = false;
  }
  // Check data mismatch (only compare up to min(size1, size2))
  auto cmpSize = std::min(size1, size2);
  if (std::memcmp(dataStart1, dataStart2, cmpSize) != 0) {
    std::printf("%s.%s.%s: Data mismatch (%u/%u)\n", 
      header1.object_name_start, 
      header1.property_name_start, 
      header1.type_name_start,
      static_cast<unsigned int>(offset1), 
      static_cast<unsigned int>(offset2));

    // Find first mismatch
    auto p1 = dataStart1;
    auto p2 = dataStart2;
    while (*p1 == *p2 && (p1 < dataEnd1) && (p2 < dataEnd2)) {
      ++p1; 
      ++p2;
    }
    // Print hex context around mismatch
    printHexContext(dataStart1, dataEnd1, p1);
    printHexContext(dataStart2, dataEnd2, p2);
    match = false;
  }

  if (!match) {
    std::printf("*NOT MATCHING*\n");
  }
}

} // unnamed namespace

int main(int argc, char** argv) {
  auto dir = simple_fs::get_or_create_oos_directory();
  if (argc <= 1) {
    std::printf("Usage: %s [file1] [file2]\n", argv[0]);
    return EXIT_FAILURE;
  }

  auto oos_file_1 = open_file(dir, simple_fs::utf8_to_native(argv[1]));
  if(!bool(oos_file_1)) {
    return EXIT_FAILURE;
  }
  auto contents_1 = simple_fs::view_contents(*oos_file_1);
  auto const* start_1 = reinterpret_cast<uint8_t const*>(contents_1.data);
  auto end_1 = start_1 + contents_1.file_size;

  // If only one file: just report
  if(argc <= 2) {
    std::printf("Doing a report of %s\n", argv[1]);
    dcon::for_each_record(reinterpret_cast<const std::byte*>(start_1),
                          reinterpret_cast<const std::byte*>(end_1),
      [&](dcon::record_header const& header_1, 
          std::byte const* data_start_1, 
          std::byte const* data_end_1) 
      {
        auto size1 = data_end_1 - data_start_1;
        std::printf("%s.%s.%s, %zu\n", 
          header_1.object_name_start, 
          header_1.property_name_start, 
          header_1.type_name_start, 
          static_cast<size_t>(size1));
      }
    );
  } 
  // If two files: compare
  else {
    auto oos_file_2 = open_file(dir, simple_fs::utf8_to_native(argv[2]));
    if(!bool(oos_file_2)) {
      return EXIT_FAILURE;
    }
    auto contents_2 = simple_fs::view_contents(*oos_file_2);
    auto const* start_2 = reinterpret_cast<uint8_t const*>(contents_2.data);
    auto end_2 = start_2 + contents_2.file_size;

    std::printf("Comparing files %s and %s\n", argv[1], argv[2]);

    dcon::for_each_record(reinterpret_cast<const std::byte*>(start_1),
                          reinterpret_cast<const std::byte*>(end_1),
      [&](dcon::record_header const& header_1, 
          std::byte const* data_start_1, 
          std::byte const* data_end_1) 
      {
        dcon::for_each_record(reinterpret_cast<const std::byte*>(start_2),
                              reinterpret_cast<const std::byte*>(end_2),
          [&](dcon::record_header const& header_2, 
              std::byte const* data_start_2, 
              std::byte const* data_end_2)
          {
            auto obj1 = std::string_view{ header_1.object_name_start, header_1.object_name_end };
            auto obj2 = std::string_view{ header_2.object_name_start, header_2.object_name_end };
            if (obj1 == obj2) {
              auto prop1 = std::string_view{ header_1.property_name_start, header_1.property_name_end };
              auto prop2 = std::string_view{ header_2.property_name_start, header_2.property_name_end };
              if (prop1 == prop2) {
                // Compare
                compareRecords(header_1, data_start_1, data_end_1,
                               header_2, data_start_2, data_end_2,
                               start_1, start_2);
              }
            }
          }
        );
      }
    );
  }

  std::printf("Kosher! Finished! ^-^\n");
  return EXIT_SUCCESS;
}

