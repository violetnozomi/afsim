// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <fstream>
#include <iostream>
#include <random>

#include <gtest/gtest.h>

#include "UtMemoryMappedFile.hpp"

namespace memory_mapped_tests
{

// RAII deleter of output files for when the test is done.
struct RAII_FileDeleter
{
   std::string mPath;
   const char* Path() { return mPath.c_str(); }
   ~RAII_FileDeleter() { remove(Path()); }
};

static void PathAppend(std::string& aPath, const std::string& aAppendment)
{
#ifdef _WIN32
   aPath += '\\';
#else
   aPath += '/';
#endif
   aPath += aAppendment;
}
// return path to file containing simply "abcd" as it's contents
static std::string MakeABCDFile()
{
   std::string path = testing::UnitTest::GetInstance()->original_working_dir();
   PathAppend(path, "test_utmemmappedfile_abcd_input.txt");

   std::ofstream out(path, std::ios::out | std::ios::binary);
   EXPECT_TRUE(out); // check failed to open file
   out << 'a' << 'b' << 'c' << 'd';
   out.close();

   return path;
}
TEST(UtMemoryMappedFile, MapFileToMemory)
{
   auto               abcdFilePath = MakeABCDFile();
   RAII_FileDeleter   remover{abcdFilePath};
   UtMemoryMappedFile mappedFile(abcdFilePath);
   ASSERT_TRUE(mappedFile.IsValid());

   EXPECT_EQ(mappedFile[0], 'a');
   EXPECT_EQ(mappedFile[1], 'b');
   EXPECT_EQ(mappedFile[2], 'c');
   EXPECT_EQ(mappedFile[3], 'd');

   EXPECT_THROW(mappedFile.At(4), std::out_of_range);

   EXPECT_EQ(mappedFile.FileSize(), 4);
   EXPECT_EQ(mappedFile.MappedSize(), 4);

   EXPECT_NO_THROW(mappedFile.Close());
}

TEST(UtMemoryMappedFile, MultipleMappingsOfSameFile)
{
   auto               abcdFilePath = MakeABCDFile();
   RAII_FileDeleter   remover{abcdFilePath};
   UtMemoryMappedFile firstMapping(abcdFilePath);
   ASSERT_TRUE(firstMapping.IsValid());

   {
      UtMemoryMappedFile secondMapping(abcdFilePath);
      ASSERT_TRUE(secondMapping.IsValid());

      // verify values are equal between both maps
      ASSERT_EQ(firstMapping.MappedSize(), secondMapping.MappedSize());

      for (size_t i = 0; i < firstMapping.MappedSize(); ++i)
      {
         EXPECT_EQ(firstMapping[i], secondMapping[i]);
      }
   }

   // ensure closing of second mapping didn't affect firstMapping
   ASSERT_TRUE(firstMapping.IsValid());
   EXPECT_EQ(firstMapping[0], 'a');
}

TEST(UtMemoryMappedFile, MoveConstructAndMoveAssign)
{
   auto             abcdFilePath = MakeABCDFile();
   RAII_FileDeleter remover{abcdFilePath};

   UtMemoryMappedFile moveAssignedMapping{};    // default constructed
   EXPECT_FALSE(moveAssignedMapping.IsValid()); // nothing has been moved in yet

   // scope to cause the original and move constructed mapping to destroy themselves,
   // allowing us to check to make sure they don't unmap the file when they've been
   // invalidated by a move.
   {
      UtMemoryMappedFile originalMapping(abcdFilePath);

      ASSERT_TRUE(originalMapping.IsValid());
      EXPECT_EQ(originalMapping[0], 'a');

      UtMemoryMappedFile moveConstructedMapping(std::move(originalMapping));
      EXPECT_FALSE(originalMapping.IsValid());
      ASSERT_TRUE(moveConstructedMapping.IsValid());
      EXPECT_EQ(moveConstructedMapping[0], 'a');

      moveAssignedMapping = std::move(moveConstructedMapping);
      EXPECT_FALSE(moveConstructedMapping.IsValid());
      ASSERT_TRUE(moveAssignedMapping.IsValid());
      EXPECT_EQ(moveAssignedMapping[0], 'a');
   }

   // check to make sure moveAssignedMapping didn't get invalidate by destruction
   // of the other two mappings.
   ASSERT_TRUE(moveAssignedMapping.IsValid());
   EXPECT_EQ(moveAssignedMapping[0], 'a');
}

TEST(UtMemoryMappedFile, UtMemMapWriter)
{
   std::string path = testing::UnitTest::GetInstance()->original_working_dir();
   PathAppend(path, "utmemmappedfile_output.txt");
   RAII_FileDeleter remover{path};

   UtMemMapWriter writer(path, 6);

   ASSERT_TRUE(writer.IsValid());

   const char* data = "abcdef";
   // write abcdef to the file in a nonlinear order
   writer.Write(0, &data[0], 1);
   writer.Write(3, &data[3], 3);
   writer.Write(1, &data[1], 2);

   // close the file, then re-open with UtMemoryMappedFile so that we can inspect the contents
   writer.Close();

   EXPECT_FALSE(writer.IsValid());

   UtMemoryMappedFile arr(path);

   ASSERT_TRUE(arr.IsValid());

   EXPECT_EQ(arr.FileSize(), 6);
   EXPECT_EQ(arr[0], 'a');
   EXPECT_EQ(arr[1], 'b');
   EXPECT_EQ(arr[2], 'c');
   EXPECT_EQ(arr[3], 'd');
   EXPECT_EQ(arr[4], 'e');
   EXPECT_EQ(arr[5], 'f');
}

// Disabled due to this being an exceedingly long-running test.
// not to mention regularly writing then wiping such a large file can
// quickly wear down a physical drive, making it more prone to hardware failure.
TEST(UtMemoryMappedFile, DISABLED_HandleBigFile)
{
   std::string path = testing::UnitTest::GetInstance()->original_working_dir();
   PathAppend(path, "utmemmappedfile_output.txt");
   RAII_FileDeleter remover{path.c_str()};

   constexpr size_t gigaByte    = 1073741824;
   constexpr size_t fileSize    = 35 * gigaByte; // 35 GB
   constexpr size_t elementSize = sizeof(std::mt19937::result_type);
   constexpr size_t numElements = fileSize / elementSize;

   UtMemMapWriter writer(path, fileSize);

   ASSERT_TRUE(writer.IsValid());

   auto         seed = testing::UnitTest::GetInstance()->random_seed();
   std::mt19937 gen(seed);

   auto print_progress = [&](size_t x)
   {
      constexpr size_t num_prints = 1024;
      if ((x + 1) % (numElements / num_prints) == 0)
      {
         std::cout << "\r" << static_cast<size_t>(x * 1000.0 / numElements) / 10.0 << "%" << std::flush;
      }
   };
   std::cout << "writing big file\n";
   auto* elements = reinterpret_cast<std::mt19937::result_type*>(writer.GetData());
   for (size_t x = 0; x < numElements; ++x)
   {
      elements[x] = gen();
      print_progress(x);
   }
   std::cout << "\nflushing/closing\n " << std::flush;
   // close the file, then re-open with UtMemoryMappedFile so that we can inspect the contents
   writer.Close();

   EXPECT_FALSE(writer.IsValid());


   UtMemoryMappedFile reader(path);

   ASSERT_TRUE(reader.IsValid());

   std::mt19937 regen(seed);

   auto* readBack          = reinterpret_cast<const std::mt19937::result_type*>(reader.GetData());
   bool  isReadBackCorrect = true;
   std::cout << "reading back big file\n";
   for (size_t x = 0; x < numElements; ++x)
   {
      isReadBackCorrect = isReadBackCorrect && (readBack[x] == regen());
      print_progress(x);
   }

   EXPECT_TRUE(isReadBackCorrect);
}

} // namespace memory_mapped_tests
