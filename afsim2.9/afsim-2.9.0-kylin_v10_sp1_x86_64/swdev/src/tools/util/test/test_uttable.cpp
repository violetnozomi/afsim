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

#include <chrono>
#include <fstream>
#include <iostream>
#include <random>

#include <gtest/gtest.h>

#include "UtInput.hpp"
#include "UtTable.hpp"

namespace uttable_tests
{

struct RAII_FileDeleter
{
   std::string mPath;
   explicit RAII_FileDeleter(const std::string& aPath)
      : mPath(aPath)
   {
   }
   RAII_FileDeleter(RAII_FileDeleter&& aMov) noexcept
      : mPath(aMov.mPath)
   {
      aMov.mPath.clear();
   }
   const char* Path() { return mPath.c_str(); }

   ~RAII_FileDeleter()
   {
      if (!mPath.empty())
      {
         remove(Path());
      }
   }
};
static std::string PathAppend(const std::string& aDir, const std::string& aAppendment)
{
   std::string path = aDir;
#ifdef _WIN32
   path += '\\';
#else
   path += '/';
#endif
   path += aAppendment;
   return path;
}

RAII_FileDeleter CreateBinaryFile()
{
   auto dir  = testing::UnitTest::GetInstance()->original_working_dir();
   auto path = PathAppend(dir, "uttable_binary_input.dat");

   std::ofstream out(path, std::ios::out | std::ios::binary);
   EXPECT_TRUE(out); // check failed to open file

   // write header:
   char precision = 0; // 0 = double, 1 = float
   out.write(&precision, 1);
   char minMaxValid = 1; // 1 = valid, 0 = invalid
   out.write(&minMaxValid, 1);
   char scheme = 0; // 0 = rectangular, 1 = morton/zorder curve
   out.write(&scheme, 1);
   char numDimensions = 2;
   out.write(&numDimensions, 1);
   char reserved[4]{0, 0, 0, 0};
   out.write(reserved, 4);

   auto   write = [&](double d) { out.write(reinterpret_cast<const char*>(&d), sizeof(d)); };
   double min   = 1.234;
   write(min);
   double max = 5e6;
   write(max);

   // write array contents:
   write(1.234);
   write(3.14159);
   write(9001.0);
   write(5e6);

   out.close();
   return RAII_FileDeleter{path};
}

// creat big binary table with sequintally incrementing values
template<typename... IvSizes>
void CreateBigBinaryFile(const std::string& aPath, IvSizes... aSizes)
{
   std::ofstream  out(aPath, std::ios::out | std::ios::binary);
   constexpr char header_size = 24;
   for (char i = 0; i < header_size; ++i)
   {
      char val = 0;
      out.write(&val, 1);
   }
   auto write = [&](double d) { out.write(reinterpret_cast<const char*>(&d), sizeof(d)); };

   size_t num_elements = 1;
   for (size_t s : {aSizes...})
   {
      num_elements *= s;
   }
   std::cerr << "creating " << aPath << " with " << num_elements << " elements\n";
   for (size_t i = 0; i < num_elements; ++i)
   {
      write(static_cast<double>(i));
   }
}

std::unique_ptr<UtTable::Table> MakeTableAllowThrow(const std::string& aInputStr,
                                                    UtInput::ValueType aDvUnitType = UtInput::cNON_DIMENSIONAL)
{
   UtInput input;
   input.PushInputString(aInputStr);
   UtTable::Parameters params{};
   for (int iv = 0; iv < 4; ++iv)
   {
      params.AddRealParameter("iv" + std::to_string(iv), UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), iv);
   }

   std::unique_ptr<UtTable::Table> tablePtr{UtTable::LoadInstance(input, aDvUnitType, UtTable::NoCheck(), params)};
   return tablePtr;
}
std::unique_ptr<UtTable::Table> MakeTable(const std::string& aInputStr,
                                          UtInput::ValueType aDvUnitType = UtInput::cNON_DIMENSIONAL)
{
   std::unique_ptr<UtTable::Table> tablePtr;
   try
   {
      tablePtr = MakeTableAllowThrow(aInputStr, aDvUnitType);
   }
   catch (const UtException& e)
   {
      GTEST_NONFATAL_FAILURE_(e.what());
      throw e;
   }
   return tablePtr;
}

// even though its bad practice to pass unique_ptr around when not passing ownership, this makes the calls to it cleaner.
template<typename... Reals>
double Lookup(const std::unique_ptr<UtTable::Table>& aTablePtr, Reals... aIVs)
{
   double ivs[sizeof...(Reals)]{static_cast<double>(aIVs)...};
   return aTablePtr->Lookup(ivs);
}


TEST(UtTable, RegularTable_dv_binary_file)
{
   auto binary_file      = CreateBinaryFile();
   auto binary_file_path = binary_file.mPath;

   auto table = MakeTable("regular_table               "
                          "   independent_variable iv0 "
                          "      0 1 2 3               "
                          "   end_independent_variable "
                          "   dependent_variable       "
                          "      binary_data_file \"" +
                          binary_file_path +
                          "\""
                          "   end_dependent_variable   "
                          "end_regular_table           ");

   ASSERT_NE(table, nullptr);

   EXPECT_EQ(Lookup(table, 0), 1.234);
   EXPECT_EQ(Lookup(table, 1), 3.14159);
   EXPECT_EQ(Lookup(table, 2), 9001.0);
   EXPECT_EQ(Lookup(table, 3), 5e6);

   // missing units command when using binary_data_file should throw when using a unit of measurement.
   EXPECT_THROW(MakeTableAllowThrow("regular_table               "
                                    "   independent_variable iv0 "
                                    "      0 1 2 3               "
                                    "   end_independent_variable "
                                    "   dependent_variable       "
                                    "      binary_data_file \"" +
                                       binary_file_path +
                                       "\""
                                       "   end_dependent_variable   "
                                       "end_regular_table           ",
                                    UtInput::cLENGTH),
                UtInput::BadValue);

   // units command must not of the standard type, not some kind of variation of the unit of measurement
   EXPECT_THROW(MakeTableAllowThrow("regular_table               "
                                    "   independent_variable iv0 "
                                    "      0 1 2 3               "
                                    "   end_independent_variable "
                                    "   dependent_variable       "
                                    "      units feet          "
                                    "      binary_data_file \"" +
                                       binary_file_path +
                                       "\""
                                       "   end_dependent_variable   "
                                       "end_regular_table           ",
                                    UtInput::cLENGTH),
                UtInput::BadValue);

   // should successfully create the table when specifing the standard type as the units.
   EXPECT_NO_THROW(MakeTable("regular_table               "
                             "   independent_variable iv0 "
                             "      0 1 2 3               "
                             "   end_independent_variable "
                             "   dependent_variable       "
                             "      units meters          "
                             "      binary_data_file \"" +
                                binary_file_path +
                                "\""
                                "   end_dependent_variable   "
                                "end_regular_table           ",
                             UtInput::cLENGTH));

   // should be able to detect that the wrong precision was specified based on the size of the binary_file
   EXPECT_THROW(MakeTableAllowThrow("regular_table               "
                                    "   independent_variable iv0 "
                                    "      0 1 2 3               "
                                    "   end_independent_variable "
                                    "   dependent_variable       "
                                    "      precision float       "
                                    "      binary_data_file \"" +
                                    binary_file_path +
                                    "\""
                                    "   end_dependent_variable   "
                                    "end_regular_table           "),
                UtInput::BadValue);
}

TEST(UtTable, RegularTable_iv_binary_file)
{
   auto binary_file      = CreateBinaryFile();
   auto binary_file_path = binary_file.mPath;

   auto table = MakeTable("regular_table               "
                          "   independent_variable iv0 "
                          "      num_points 4          "
                          "      binary_data_file \"" +
                          binary_file_path +
                          "\""
                          "   end_independent_variable "
                          "   dependent_variable       "
                          "      0 1 2 3               "
                          "   end_dependent_variable   "
                          "end_regular_table           ");

   ASSERT_NE(table, nullptr);

   EXPECT_EQ(Lookup(table, 1.234), 0);
   EXPECT_EQ(Lookup(table, 3.14159), 1);
   EXPECT_EQ(Lookup(table, 9001.0), 2);
   EXPECT_EQ(Lookup(table, 5e6), 3);

   EXPECT_THROW(MakeTableAllowThrow("regular_table               "
                                    "   independent_variable iv0 "
                                    "      precision float       " // wrong precision!
                                    "      num_points 4          "
                                    "      binary_data_file \"" +
                                    binary_file_path +
                                    "\""
                                    "   end_independent_variable "
                                    "   dependent_variable       "
                                    "      0 1 2 3               "
                                    "   end_dependent_variable   "
                                    "end_regular_table           "),
                UtInput::BadValue);

   EXPECT_THROW(MakeTableAllowThrow("regular_table               "
                                    "   independent_variable iv0 "
                                    "      num_points 3          " // wrong number of points!
                                    "      binary_data_file \"" +
                                    binary_file_path +
                                    "\""
                                    "   end_independent_variable "
                                    "   dependent_variable       "
                                    "      0 1 2                 "
                                    "   end_dependent_variable   "
                                    "end_regular_table           "),
                UtInput::BadValue);
}


TEST(UtTable, RegularTable_iv_linear_equation)
{
   auto table = MakeTable("regular_table               "
                          "   independent_variable iv0 "
                          "      linear_equation from 2 to 8"
                          "      num_points 4          "
                          "   end_independent_variable "
                          "   dependent_variable       "
                          "      4 8 12 16             "
                          "   end_dependent_variable   "
                          "end_regular_table           ");

   EXPECT_EQ(Lookup(table, 2), 4);
   EXPECT_EQ(Lookup(table, 4), 8);
   EXPECT_EQ(Lookup(table, 5.5), 11); // interpolated
   EXPECT_EQ(Lookup(table, 20), 16);  // clamped (no extrapolation)

   std::vector<double> knownValues{2, 4, 6, 8};
   std::vector<double> values;
   table->GetVariable("iv0", values);
   EXPECT_EQ(values, knownValues);

   double min, max;
   table->GetVariableLimits("iv0", min, max);
   EXPECT_EQ(min, 2);
   EXPECT_EQ(max, 8);
}

TEST(UtTable, RegularTable_1D)
{
   auto table1D = MakeTable("regular_table               "
                            "   independent_variable iv0 "
                            "      0 1                   "
                            "   end_independent_variable "
                            "   dependent_variable       "
                            "      1 2                   "
                            "   end_dependent_variable   "
                            "end_regular_table           ");

   EXPECT_EQ(Lookup(table1D, 0), 1);
   EXPECT_EQ(Lookup(table1D, 1), 2);

   EXPECT_EQ(Lookup(table1D, 0.5), 1.5); // center interpolated
   EXPECT_EQ(Lookup(table1D, 0.1), 1.1); // lopsided interpolation
   EXPECT_EQ(Lookup(table1D, 0.9), 1.9); // lopsided interpolation
}

TEST(UtTable, RegularTable_2D)
{
   auto table2D =
      MakeTable("regular_table               "
                "   independent_variable iv0 "
                "      0 1                   "
                "   end_independent_variable "
                "   independent_variable iv1 "
                "      0 1                   "
                "   end_independent_variable "
                "   dependent_variable       "
                "      1 3 2 4               " // listed {iv0_0, iv1_0}, {iv0_0, iv1_1}, {iv0_1, iv1_0}, {iv0_1,iv1_1},
                "   end_dependent_variable   "
                "end_regular_table           ");

   EXPECT_EQ(Lookup(table2D, 0, 0), 1);
   EXPECT_EQ(Lookup(table2D, 1, 0), 2);
   EXPECT_EQ(Lookup(table2D, 0, 1), 3);
   EXPECT_EQ(Lookup(table2D, 1, 1), 4);

   EXPECT_EQ(Lookup(table2D, 0.5, 0.5), 2.5);    // center interpolated
   EXPECT_EQ(Lookup(table2D, 0.5, 0.0), 1.5);    // single side interpolate
   EXPECT_EQ(Lookup(table2D, 0.75, 0.25), 2.25); // lopsided interpolation
}

TEST(UtTable, RegularTable_3D)
{
   auto table3D = MakeTable("regular_table               "
                            "   independent_variable iv0 "
                            "      0 1                   "
                            "   end_independent_variable "
                            "   independent_variable iv1 "
                            "      0 1                   "
                            "   end_independent_variable "
                            "   independent_variable iv2 "
                            "      0 1                   "
                            "   end_independent_variable "
                            "   dependent_variable       "
                            "      1 2 3 4 5 6 7 8       "
                            "   end_dependent_variable   "
                            "end_regular_table           ");

   int counter = 1;
   for (double iv0 : {0, 1})
   {
      for (double iv1 : {0, 1})
      {
         for (double iv2 : {0, 1})
         {
            EXPECT_EQ(Lookup(table3D, iv0, iv1, iv2), counter);
            ++counter;
         }
      }
   }
   EXPECT_EQ(Lookup(table3D, 0.5, 0.5, 0.5), 4.5); // center interpolated
   EXPECT_EQ(Lookup(table3D, 0.0, 0.5, 0.5), 2.5); // interpolated along iv0=0
   EXPECT_EQ(Lookup(table3D, 0.6, 0.1, 0.3), 3.9); // lopsided interpolation
}

TEST(UtTable, RegularTable_4D)
{
   auto table4D = MakeTable("regular_table               "
                            "   independent_variable iv0 "
                            "      0 1                   "
                            "   end_independent_variable "
                            "   independent_variable iv1 "
                            "      0 1                   "
                            "   end_independent_variable "
                            "   independent_variable iv2 "
                            "      0 1                   "
                            "   end_independent_variable "
                            "   independent_variable iv3 "
                            "      0 1                   "
                            "   end_independent_variable "
                            "   dependent_variable       "
                            "      1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16"
                            "   end_dependent_variable   "
                            "end_regular_table           ");

   int counter = 1;
   for (double iv0 : {0, 1})
   {
      for (double iv1 : {0, 1})
      {
         for (double iv2 : {0, 1})
         {
            for (double iv3 : {0, 1})
            {
               EXPECT_EQ(Lookup(table4D, iv0, iv1, iv2, iv3), counter);
               ++counter;
            }
         }
      }
   }

   EXPECT_EQ(Lookup(table4D, 0.5, 0.5, 0.5, 0.5), 8.5); // center interpolated
   EXPECT_EQ(Lookup(table4D, 0.0, 0.5, 0.5, 0.5), 4.5); // interpolated along iv0=0
   EXPECT_EQ(Lookup(table4D, 0.0, 0.0, 0.5, 0.5), 2.5); // interpolated along iv0=0, iv1=0
   EXPECT_EQ(Lookup(table4D, 0.1, 0.2, 0.3, 0.4), 3.6); // lopsided interpolation
}

TEST(UtTable, RegularTable_nearest_interpolation)
{
   auto table2D =
      MakeTable("regular_table               "
                "   independent_variable iv0 "
                "      0 1                   "
                "   end_independent_variable "
                "   independent_variable iv1 "
                "      0 1                   "
                "   end_independent_variable "
                "   dependent_variable       "
                "      interpolation nearest "
                "      1 3 2 4               " // listed {iv0_0, iv1_0}, {iv0_0, iv1_1}, {iv0_1, iv1_0}, {iv0_1,iv1_1},
                "   end_dependent_variable   "
                "end_regular_table           ");

   EXPECT_EQ(Lookup(table2D, 0, 0), 1);
   EXPECT_EQ(Lookup(table2D, 1, 0), 2);
   EXPECT_EQ(Lookup(table2D, 0, 1), 3);
   EXPECT_EQ(Lookup(table2D, 1, 1), 4);

   // test nearest interpolation
   EXPECT_EQ(Lookup(table2D, 0.4, 0.4), 1);
   EXPECT_EQ(Lookup(table2D, 0.6, 0.4), 2);
   EXPECT_EQ(Lookup(table2D, 0.4, 0.6), 3);
   EXPECT_EQ(Lookup(table2D, 0.6, 0.6), 4);
}

TEST(UtTable, RegularTable_unit_conversion)
{
   auto table1D = MakeTable("regular_table               "
                            "   independent_variable iv0 "
                            "      0 1                   "
                            "   end_independent_variable "
                            "   dependent_variable       "
                            "      units kilometers      "
                            "      1 2                   "
                            "   end_dependent_variable   "
                            "end_regular_table           ",
                            UtInput::cLENGTH);

   // 0 km / 1 km values should be converted to meters during lookup
   EXPECT_EQ(Lookup(table1D, 0), 1000);
   EXPECT_EQ(Lookup(table1D, 1), 2000);

   EXPECT_EQ(Lookup(table1D, 0.5), 1500); // interpolated

   // fail on illegal value for units
   EXPECT_THROW(MakeTableAllowThrow("regular_table               "
                                    "   independent_variable iv0 "
                                    "      0 1                   "
                                    "   end_independent_variable "
                                    "   dependent_variable       "
                                    "      units pascals         " // pascals is not a unit of length
                                    "      1 2                   "
                                    "   end_dependent_variable   "
                                    "end_regular_table           ",
                                    UtInput::cLENGTH),
                UtInput::BadValue);
}

TEST(UtTable, RegularTable_save_as_binary_file)
{
   auto             dir              = testing::UnitTest::GetInstance()->original_working_dir();
   std::string      binary_file_path = PathAppend(dir, "uttable_binary_table.dat");
   std::string      zcurve_file_path = PathAppend(dir, "uttable_zcurve_table.dat");
   RAII_FileDeleter fileRemover{binary_file_path};
   RAII_FileDeleter fileRemover2{zcurve_file_path};

   auto tableWrite = MakeTable("regular_table               "
                               "   independent_variable iv0 "
                               "      1 2 3                 "
                               "   end_independent_variable "
                               "   independent_variable iv1 "
                               "      4 5 6 7               "
                               "   end_independent_variable "
                               "   independent_variable iv2 "
                               "      8 9 10 11 12          "
                               "   end_independent_variable "
                               "   dependent_variable       "
                               "     save_as_rectangular_indexed_binary_file \"" +
                               binary_file_path +
                               "\""
                               "     save_as_morton_indexed_binary_file \"" +
                               zcurve_file_path +
                               "\""
                               "      0  1  2  3  4  5  6  7  8  9 "
                               "     10 11 12 13 14 15 16 17 18 19 "
                               "     20 21 22 23 24 25 26 27 28 29 "
                               "     30 31 32 33 34 35 36 37 38 39 "
                               "     40 41 42 43 44 45 46 47 48 49 "
                               "     50 51 52 53 54 55 56 57 58 59 "
                               "   end_dependent_variable   "
                               "end_regular_table           ");

   int i = 0;
   for (int x : {1, 2, 3})
   {
      for (int y : {4, 5, 6, 7})
      {
         for (int z : {8, 9, 10, 11, 12})
         {
            EXPECT_EQ(Lookup(tableWrite, x, y, z), i);
            ++i;
         }
      }
   }
   double dv_min;
   double dv_max;
   tableWrite->GetVariableLimits("", dv_min, dv_max);
   EXPECT_EQ(dv_min, 0);
   EXPECT_EQ(dv_max, 59);

   auto tableBin    = MakeTable("regular_table               "
                                "   independent_variable iv0 "
                                "      1 2 3                 "
                                "   end_independent_variable "
                                "   independent_variable iv1 "
                                "      4 5 6 7               "
                                "   end_independent_variable "
                                "   independent_variable iv2 "
                                "      8 9 10 11 12          "
                                "   end_independent_variable "
                                "   dependent_variable       "
                                "     binary_data_file \"" +
                             binary_file_path +
                             "\""
                                "   end_dependent_variable   "
                                "end_regular_table           ");
   auto tableZCurve = MakeTable("regular_table               "
                                "   independent_variable iv0 "
                                "      1 2 3                 "
                                "   end_independent_variable "
                                "   independent_variable iv1 "
                                "      4 5 6 7               "
                                "   end_independent_variable "
                                "   independent_variable iv2 "
                                "      8 9 10 11 12          "
                                "   end_independent_variable "
                                "   dependent_variable       "
                                "     binary_data_file \"" +
                                zcurve_file_path +
                                "\""
                                "   end_dependent_variable   "
                                "end_regular_table           ");

   i = 0;
   for (int x : {1, 2, 3})
   {
      for (int y : {4, 5, 6, 7})
      {
         for (int z : {8, 9, 10, 11, 12})
         {
            EXPECT_EQ(Lookup(tableBin, x, y, z), i);
            EXPECT_EQ(Lookup(tableZCurve, x, y, z), i);
            ++i;
         }
      }
   }


   tableBin->GetVariableLimits("", dv_min, dv_max);
   EXPECT_EQ(dv_min, 0);
   EXPECT_EQ(dv_max, 59);

   tableZCurve->GetVariableLimits("", dv_min, dv_max);
   EXPECT_EQ(dv_min, 0);
   EXPECT_EQ(dv_max, 59);

   std::vector<double> knownValues = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                                      20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
                                      40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59};

   std::vector<double> rectValues;
   tableBin->GetVariable("", rectValues);
   EXPECT_EQ(rectValues, knownValues);

   std::vector<double> mortonValues;
   tableZCurve->GetVariable("", mortonValues);
   EXPECT_EQ(mortonValues, knownValues);
}

void BenchmarkBigTable(std::unique_ptr<UtTable::Table>& aTable)
{
   auto   start = std::chrono::high_resolution_clock::now();
   double x     = 300;
   double y     = 200;
   double z     = 100;

   // sample points along a linear line, which is intended to simulate common use cases
   // of table lookups, wherin subsequent lookup points tend to be nearby previous lookup points.
   for (int i = 0; i < 2000; ++i)
   {
      x += 0.25;
      y += 0.3;
      z += 0.4;
      Lookup(aTable, x, y, z);
   }
   auto end = std::chrono::high_resolution_clock::now();
   std::cerr << "linear points: time taken: " << std::chrono::duration<size_t, std::nano>(end - start).count() << " ns\n";

   // ==================================================

   start = std::chrono::high_resolution_clock::now();

   // sample 200 random points
   std::uniform_real_distribution<> dist{0, 1023};
   auto                             seed = testing::UnitTest::GetInstance()->random_seed();
   std::mt19937                     gen(seed);
   for (int i = 0; i < 200; ++i)
   {
      Lookup(aTable, dist(gen), dist(gen), dist(gen));
   }

   end = std::chrono::high_resolution_clock::now();
   std::cerr << "random points: time taken: " << std::chrono::duration<size_t, std::nano>(end - start).count() << " ns\n";
}

TEST(UtTable, DISABLED_RegularTable_benchmark_big_binary_file)
{
   auto        dir              = testing::UnitTest::GetInstance()->original_working_dir();
   std::string binary_file_path = PathAppend(dir, "uttable_big_binary_table.dat");
   // std::string zcurve_file_path = PathAppend(dir,"uttable_big_zcurve_table.dat");

   // uncomment to create the initial big binary file, then comment back out
   // CreateBigBinaryFile(binary_file_path, 1024, 1024, 1024);

   auto table = MakeTable("regular_table               "
                          "   independent_variable iv0 "
                          "      num_points 1024"
                          "      linear_equation from 0 to 1023"
                          "   end_independent_variable "
                          "   independent_variable iv1 "
                          "      num_points 1024"
                          "      linear_equation from 0 to 1023"
                          "   end_independent_variable "
                          "   independent_variable iv2 "
                          "      num_points 1024"
                          "      linear_equation from 0 to 1023"
                          "   end_independent_variable "
                          "   dependent_variable       "
                          "      binary_data_file \"" +
                          binary_file_path +
                          "\""
                          // uncomment to create the initial big morton binary file, then comment back out
                          //"      save_as_morton_indexed_binary_file \"" + zcurve_file_path + "\""
                          "   end_dependent_variable   "
                          "end_regular_table           ");

   EXPECT_EQ(Lookup(table, 0, 0, 0), 0);

   BenchmarkBigTable(table);
}

TEST(UtTable, DISABLED_RegularTable_benchmark_big_mortan_file)
{
   auto        dir              = testing::UnitTest::GetInstance()->original_working_dir();
   std::string zcurve_file_path = PathAppend(dir, "uttable_big_zcurve_table.dat");

   auto table = MakeTable("regular_table               "
                          "   independent_variable iv0 "
                          "      num_points 1024"
                          "      linear_equation from 0 to 1023"
                          "   end_independent_variable "
                          "   independent_variable iv1 "
                          "      num_points 1024"
                          "      linear_equation from 0 to 1023"
                          "   end_independent_variable "
                          "   independent_variable iv2 "
                          "      num_points 1024"
                          "      linear_equation from 0 to 1023"
                          "   end_independent_variable "
                          "   dependent_variable       "
                          "      binary_data_file \"" +
                          zcurve_file_path +
                          "\""
                          "   end_dependent_variable   "
                          "end_regular_table           ");

   EXPECT_EQ(Lookup(table, 0, 0, 0), 0);

   BenchmarkBigTable(table);
}
} // namespace uttable_tests
