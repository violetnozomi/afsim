// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "UtTable_RegularTable.hpp"

#include <climits>
#include <iostream>

#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "UtMemoryMappedFile.hpp"
#include "UtTable_Lookup.hpp"


#if defined(__BMI2__) || defined(_MSC_VER)
// msvc will always emit _pdep_u64, even if the target architecture doesn't support it
// (which would cause a crash), in which case we need to runtime check for pdep_u64 support.
// GCC/Clang on the otherhand require inline assembly for us to emit _pdep_u64 when
// the target host doesn't support it.
#include <immintrin.h> // _pdep_u64
#define BMI2_FOUND
#ifdef _MSC_VER
#define RUNTIME_CHECK_BMI2
#include <intrin.h> // __cpuid and __cpuidex
#endif
#endif

namespace // anonymous
{
struct TableLoader;

// meta-template programming class for creating a Table_ND with the correct template parameters
template<size_t Dimension>
struct CreateTable
{
   template<typename... IVHolders>
   static std::unique_ptr<UtTable::Table> op(TableLoader& aBuilder, IVHolders&&... aIVs);
};

template<class Base_Table>
class MemMapped_Table : public Base_Table
{
public:
   // pull in constructor
   using Base_Table::Base_Table;

   ~MemMapped_Table() override = default; // ensure parent uses virtual destructor

   void SetMemoryMappedFile(UtMemoryMappedFile&& aFile) { mMappedMemoryHandle = std::move(aFile); }

   /// write pre-calculated min/max value from the table without having
   /// to read the entire table to calculate them.
   void SetMinMax(double aMinValue, double aMaxValue)
   {
      this->mMinMaxValid = true;
      this->mMinValue    = static_cast<decltype(this->mMinValue)>(aMinValue);
      this->mMaxValue    = static_cast<decltype(this->mMaxValue)>(aMaxValue);
   }

private:
   UtMemoryMappedFile mMappedMemoryHandle{};
};

enum class InterpolationType
{
   eLinear,
   eNearest
};
} // end anonymous namespace

// Forward declarations for things defined in this file
namespace mortonnd
{
namespace // anonymous
{
// morton-nd - A header-only compile-time Morton encoding/decoding
//    library for N dimensions.
// https://github.com/kevinhartman/morton-nd
//
// Copyright (c) 2015-2018 Kevin Hartman
//
// Main Contributors:
//   Kevin Hartman @kevinhartman <kevin@hart.mn>
//
// Licensed under the MIT License.

#ifdef BMI2_FOUND
//! see also: https://en.wikipedia.org/wiki/Z-order_curve
template<size_t Dimensions>
class MortonIndexerBMI;
#endif

template<size_t Size, typename Default = void()>
using built_in_t = typename std::conditional<
   (Size <= std::numeric_limits<uint8_t>::digits),
   uint8_t,
   typename std::conditional<
      (Size <= std::numeric_limits<uint16_t>::digits),
      uint16_t,
      typename std::conditional<
         (Size <= std::numeric_limits<uint32_t>::digits),
         uint32_t,
         typename std::conditional<(Size <= std::numeric_limits<uint64_t>::digits), uint64_t, Default>::type>::type>::type>::type;

template<size_t Fields, size_t FieldBits, size_t LutBits, typename T = built_in_t<Fields * FieldBits>>
class MortonIndexerLUT;
} // end anonymous namespace
} // namespace mortonnd

namespace UtTable
{
namespace RegularTable
{
template<typename T>
UT_EXPORT void ReadDV_Array(UtInputBlock&         aInputBlock,
                            UtInput::ValueType    aUnitType,
                            const std::string&    aUnits,
                            const ValueCheck&     aValueCheck,
                            size_t                aExpectedSize,
                            std::unique_ptr<T[]>& aArrayPtr);

template<typename T>
UT_EXPORT void ReadIV_Array(UtInputBlock&         aInputBlock,
                            UtInput::ValueType    aUnitType,
                            const std::string&    aUnits,
                            const ValueCheck&     aValueCheck,
                            size_t&               aSize,
                            std::unique_ptr<T[]>& aArrayPtr);

UT_EXPORT std::unique_ptr<Table> ReadCurve(UtInputBlock&      aInputBlock,
                                           bool               aAllowExtrapolation,
                                           const std::string& aIV_Name,
                                           UtInput::ValueType aIV_UnitType,
                                           const std::string& aIV_Units,
                                           const ValueCheck&  aIV_ValueCheck,
                                           bool               aIV_Double,
                                           UtInput::ValueType aDV_UnitType,
                                           const std::string& aDV_Units,
                                           const ValueCheck&  aDV_ValueCheck,
                                           bool               aDV_Double);

template<typename IV, typename DV>
UT_EXPORT std::unique_ptr<Table> ReadCurve(UtInputBlock&      aInputBlock,
                                           bool               aAllowExtrapolation,
                                           const std::string& aIV_Name,
                                           UtInput::ValueType aIV_UnitType,
                                           const std::string& aIV_Units,
                                           const ValueCheck&  aIV_ValueCheck,
                                           UtInput::ValueType aDV_UnitType,
                                           const std::string& aDV_Units,
                                           const ValueCheck&  aDV_ValueCheck);
} // end namespace RegularTable
} // end namespace UtTable

using namespace UtTable;
using namespace UtTable::RegularTable;

// =======================================================================================

namespace mortonnd
{
namespace // anonymous
{
#ifdef BMI2_FOUND
//! see also: https://en.wikipedia.org/wiki/Z-order_curve
//! uses the _pdup_u64 instruction from the BMI2 instruction set
template<size_t Dimensions>
class MortonIndexerBMI
{
public:
   MortonIndexerBMI(const std::array<size_t, Dimensions>& aIvSizes) {}

   size_t Index(const std::array<size_t, Dimensions>& aIndexs) const
   {
      size_t index = 0;
      for (size_t i = 0; i < Dimensions; ++i)
      {
         index |= _pdep_u64(aIndexs[i], cSelector << i);
      }
      return index;
   }

   size_t MaxIndex(std::array<size_t, Dimensions> aIvSizes) const
   {
      for (auto& s : aIvSizes)
      {
         s -= 1;
      }
      return Index(aIvSizes);
   }

private:
   static constexpr size_t BuildSelector(size_t aBitsRemaining)
   {
      return aBitsRemaining == 1 ? 1 : (BuildSelector(aBitsRemaining - 1) << Dimensions) | 1;
   }

   static constexpr auto cSelector = BuildSelector(std::numeric_limits<size_t>::digits / Dimensions);
};
#endif

// =======================================================================================

/**
 * @tparam Fields the number of fields (components) to encode
 * @tparam FieldBits the number of bits in each input field, starting with the LSb. Higher bits are not cleared.
 * @tparam LutBits the number of bits for the LUT. Each field will be looked-up 'LutBits' bits at a time.
 * @tparam T the type of the components to encode/decode, as well as the type of the result
 */
template<size_t Fields, size_t FieldBits, size_t LutBits, typename T>
class MortonIndexerLUT
{
   static_assert(Fields > 0, "Parameter 'Fields' must be > 0.");
   static_assert(FieldBits > 0, "Parameter 'FieldBits' must be > 0. ");
   static_assert(LutBits > 0, "Parameter 'LutBits' must be > 0.");
   static_assert(LutBits <= FieldBits, "Parameter 'LutBits' must be <= 'FieldBits'.");

   // LUT entry size is always Fields * LutBits. If no suitable built-in type can hold the entry, the user-specified
   // field type will be used (if provided).
   using lut_entry_t = built_in_t<Fields * LutBits, T>;

public:
   using type = T;

   static const std::size_t ChunkCount = 1 + ((FieldBits - 1) / LutBits);
   static const T           InputMask  = (static_cast<T>(1) << FieldBits) - 1;

   constexpr MortonIndexerLUT() = default;
   constexpr MortonIndexerLUT(const std::array<size_t, Fields>& aIvSizes) {}

   size_t Index(const std::array<size_t, Fields>& aIndexs) const
   {
      T index = 0;
      for (T i = 0; i < Fields; ++i)
      {
         index |= LookupField(aIndexs[i], detail::make_index_sequence<ChunkCount>{}) << i;
      }
      return static_cast<size_t>(index);
   }

   size_t MaxIndex(std::array<size_t, Fields> aIvSizes) const
   {
      for (auto& s : aIvSizes)
      {
         s -= 1;
      }
      return Index(aIvSizes);
   }

private:
   template<size_t... I>
   T LookupField(T aField, detail::index_sequence<I...>) const
   {
      return LookupField(aField, I...);
   }

   template<typename... Args>
   constexpr T LookupField(T aField, size_t, Args... args) const
   {
      return (LookupField(aField >> LutBits, args...) << (Fields * LutBits)) |
             static_cast<T>(LookupTable[aField & ChunkMask]);
   }

   constexpr T LookupField(T aField, size_t) const { return LookupTable[aField & ChunkMask]; }

   static constexpr lut_entry_t SplitByN(lut_entry_t input, size_t bitsRemaining = LutBits)
   {
      return (bitsRemaining == 1) ?
                (input & static_cast<lut_entry_t>(1)) :
                (SplitByN(input >> 1, bitsRemaining - 1) << Fields) | (input & static_cast<lut_entry_t>(1));
   }

   template<size_t... i>
   static constexpr auto BuildLut(detail::index_sequence<i...>) -> std::array<lut_entry_t, sizeof...(i)>
   {
      return std::array<lut_entry_t, sizeof...(i)>{{SplitByN(i)...}};
   }

   static constexpr size_t                       LutSize   = 1ULL << LutBits;
   static constexpr T                            ChunkMask = (static_cast<T>(1) << LutBits) - 1;
   static const std::array<lut_entry_t, LutSize> LookupTable;
};

template<size_t F, size_t B, size_t L, typename T>
const std::array<built_in_t<F * L, T>, MortonIndexerLUT<F, B, L, T>::LutSize> MortonIndexerLUT<F, B, L, T>::LookupTable =
   MortonIndexerLUT<F, B, L, T>::BuildLut(detail::make_index_sequence<MortonIndexerLUT<F, B, L, T>::LutSize>{});
} // end anonymous namespace
} // namespace mortonnd


// =======================================================================================
#ifdef RUNTIME_CHECK_BMI2
/// Runtime check to see if the cpu being executed on
/// supports the BMI2 instruction set.
static bool IsBMI2_Supported()
{
   // https://docs.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex?view=vs-2019
   std::array<int, 4> cpui;
   // Calling __cpuid with 0x0 as the function_id argument
   // gets the number of the highest valid function ID.
   __cpuid(cpui.data(), 0);

   auto nIds = cpui[0];
   if (nIds < 7)
   {
      return false;
   }

   __cpuidex(cpui.data(), 7, 0);
   auto f_7_EBX = cpui[1];
   return (f_7_EBX >> 8) & 1;
}
#endif

// =======================================================================================

namespace // anonymous
{
struct TableLoader
{
   struct IV_Variant
   {
      std::unique_ptr<IV_Table<double>>    mDoubleIV;
      std::unique_ptr<IV_Table<float>>     mFloatIV;
      std::unique_ptr<IV_Table<LinearTag>> mLinearIV;
      bool                                 mAllowExtrapolation = false;

      bool IsValid() const { return mDoubleIV || mFloatIV || mLinearIV; }

      size_t Size() const
      {
         if (mDoubleIV)
         {
            return mDoubleIV->Size();
         }
         if (mFloatIV)
         {
            return mFloatIV->Size();
         }
         if (mLinearIV)
         {
            return mLinearIV->Size();
         }
         return 0;
      }
   };

   struct MemMapHolder
   {
      bool               IsValid() { return mMappedFile.IsValid(); }
      UtMemoryMappedFile mMappedFile;

      IndexScheme mIndexScheme{IndexScheme::eRectangular};
      bool        mMinMaxValid{false};
      double      mMinValue{};
      double      mMaxValue{};
   };

   struct DV_Array
   {
      bool IsValid() { return GetDoubleArray() != nullptr || GetFloatArray() != nullptr || mMappedFile.IsValid(); }

      const double*& GetDoubleArray()
      {
         if (mDoubleArray == nullptr)
         {
            mDoubleArray = mMutableDoubles.get();
         }
         return mDoubleArray;
      }

      const float*& GetFloatArray()
      {
         if (mFloatArray == nullptr)
         {
            mFloatArray = mMutableFloats.get();
         }
         return mFloatArray;
      }

      void SetNonOwningArray(ut::span<const double> aDoubleArray) { mDoubleArray = aDoubleArray.data(); }

      void SetNonOwningArray(ut::span<const float> aFloatArray) { mFloatArray = aFloatArray.data(); }

      std::unique_ptr<double[]> mMutableDoubles;
      std::unique_ptr<float[]>  mMutableFloats;

      MemMapHolder mMappedFile;

      InterpolationType mInterpolation = InterpolationType::eLinear;

   private:
      const double* mDoubleArray = nullptr; // non-owning
      const float*  mFloatArray  = nullptr; // non-owning
   };

   struct Units
   {
      UtInput::ValueType mType;
      std::string        mUnits;

      bool IsValid() { return mType >= UtInput::cFIRST_NON_UNITARY || !mUnits.empty(); }

      bool ProcessInput(UtInput& aInput)
      {
         if (aInput.GetCommand() != "units")
         {
            return false;
         }

         if (IsValid())
         {
            throw UtInput::OutOfContext(aInput, "units has already been specified, or the data type is non-dimensional");
         }

         ReadUnits(aInput, mType, mUnits);
         return true;
      }
   };

   static UtMemoryMappedFile OpenBinaryFile(UtInput& aInput)
   {
      std::string binary_path;
      aInput.ReadValueQuoted(binary_path);
      binary_path = aInput.LocateFile(binary_path);

      UtMemoryMappedFile binaryFile;
      bool               success = binaryFile.Open(binary_path, UtMemMap::MapWholeFile);
      if (!success || !binaryFile.IsValid())
      {
         throw UtInput::BadValue(aInput, "failed to map '" + binary_path + "' to memory");
      }

      return binaryFile;
   }

   //! assumes ivCount/mIVs has been populated
   size_t ExpectedNumElementsInMortonFile()
   {
      if (ivCount == 0)
      {
         return 0;
      }

      // build selector, where ivCount is the number of dimensions
      size_t bitsPerIndex = std::numeric_limits<size_t>::digits / ivCount;
      size_t selector     = 1;
      for (size_t b = 0; b < bitsPerIndex; ++b)
      {
         selector = (selector << ivCount) | 1;
      }

      // algorithmic implementation of _pdep_u64
      auto deposit_bits = [&](size_t val, size_t mask)
      {
         size_t res = 0;
         for (size_t bb = 1; mask != 0; bb += bb)
         {
#ifdef _MSC_VER
#pragma warning(disable : 4146) // unary minus operator applied to unsigned type, result still unsigned
#endif
            if (val & bb)
            {
               res |= mask & (-mask);
            }
            mask &= (mask - 1);
         }
         return res;
      };

      // compute morton code using the maximum index: {iv1size-1, iv2size-1,..., ivNsize-1}
      size_t maxMortonIndex = 0;
      for (size_t i = 0; i < ivCount; ++i)
      {
         size_t ivSize = mIVs[i].Size();
         maxMortonIndex |= deposit_bits(ivSize - 1, selector << i);
      }
      return maxMortonIndex + 1;
   }

   template<typename Real>
   const Real* InterpretMappedMemory(UtInput&          aInput,
                                     MemMapHolder&     aMemMap,
                                     const Units&      aUnits,
                                     const ValueCheck& aValueCheck,
                                     size_t            aExpectedNumPoints)
   {
      auto& mappedMem = aMemMap.mMappedFile;

      // if using a unit of measurement, the units command must be specified with the standard type of the unit.
      if (aUnits.mType < UtInput::cFIRST_NON_UNITARY)
      {
         auto* unitHandler = UtUnits::GetUnitType(static_cast<UtUnits::UnitType>(aUnits.mType));
         if (unitHandler->ReadUnit(aUnits.mUnits) != unitHandler->GetStandardUnitId())
         {
            throw UtInput::BadValue(aInput,
                                    "for clarity, you must specify 'units " +
                                       unitHandler->FindUnitName(unitHandler->GetStandardUnitId()) + "' " +
                                       "before 'binary_data_file', and it must be of that exact unit of measurement");
         }
      }

      if (aExpectedNumPoints == 0)
      {
         throw UtInput::BadValue(aInput, "'num_points' must be specified when using 'binary_data_file' command");
      }

      size_t header_bytes = 16;
      if (mappedMem.MappedSize() < header_bytes)
      {
         throw UtInput::BadValue(aInput, "binary file is smaller than the required 16-byte header to the binary file");
      }

      // ==========================================================
      // verify precision (first byte)
      enum Precision
      {
         eDOUBLE = 0,
         eFLOAT  = 1
      };
      // first byte: 0=double precision, 1=float precision
      auto precision  = mappedMem[0];
      bool useDoubles = std::is_same<Real, double>::value;
      if (precision != eDOUBLE && precision != eFLOAT)
      {
         throw UtInput::BadValue(aInput,
                                 "illegal precision byte in binary header, expect: 0=double; 1=float, found: " +
                                    std::to_string(precision));
      }

      if (precision == eDOUBLE && !useDoubles)
      {
         throw UtInput::BadValue(
            aInput,
            "binary file header indicates it uses double precision, but input 'precision' is not 'double'");
      }
      if (precision == eFLOAT && useDoubles)
      {
         throw UtInput::BadValue(aInput, "binary file header indicates it uses float precision, but input 'precision' is not 'float'");
      }
      if (precision == eDOUBLE)
      {
         header_bytes = 24;
         if (mappedMem.MappedSize() < header_bytes)
         {
            throw UtInput::BadValue(
               aInput,
               "binary file is smaller than the required 24-byte header for double precision table file.");
         }
      }

      // =========================================
      // min/max values
      auto minmaxValid = mappedMem[1];
      if (minmaxValid >= 2)
      {
         throw UtInput::BadValue(aInput,
                                 "illegal byte in binary header for min/max valid, expect 1=valid; 0=invalid, found: " +
                                    std::to_string(minmaxValid));
      }

      if (minmaxValid == 1)
      {
         // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
         auto* doubles = reinterpret_cast<const Real*>(mappedMem.GetData());
         Real  minVal  = doubles[useDoubles ? 1 : 2];
         Real  maxVal  = doubles[useDoubles ? 2 : 3];
         if (minVal > maxVal)
         {
            throw UtInput::BadValue(aInput,
                                    "binary table header violation, min value > max value; "
                                    "found min: " +
                                       std::to_string(minVal) + " max: " + std::to_string(maxVal));
         }

         if (!aValueCheck.IsValid(minVal))
         {
            throw UtInput::BadValue(aInput, "min value in binary table's header is an illegal value");
         }
         if (!aValueCheck.IsValid(maxVal))
         {
            throw UtInput::BadValue(aInput, "max value in binary table's header is an illegal value");
         }

         aMemMap.mMinMaxValid = true;
         aMemMap.mMinValue    = minVal;
         aMemMap.mMaxValue    = maxVal;
      }

      // ==========================================
      // verify indexing scheme
      aMemMap.mIndexScheme = static_cast<IndexScheme>(mappedMem[2]);
      if (aMemMap.mIndexScheme != IndexScheme::eRectangular && aMemMap.mIndexScheme != IndexScheme::eMorton)
      {
         throw UtInput::BadValue(
            aInput,
            "unexpected value in 2nd byte of binary header for indexing scheme, expect 0=normal;1=morton, found: " +
               std::to_string(static_cast<unsigned>(aMemMap.mIndexScheme)));
      }

      // ==================================================================

      // verify size of file contains exactly the expected number of points

      auto table_size = aExpectedNumPoints;
      if (aMemMap.mIndexScheme == IndexScheme::eMorton)
      {
         table_size = ExpectedNumElementsInMortonFile();
      }

      constexpr size_t bytes_per_element = sizeof(Real);
      auto             expectedBytes     = header_bytes + table_size * bytes_per_element;

      if (mappedMem.MappedSize() != expectedBytes)
      {
         constexpr bool usingDoubles = std::is_same<Real, double>::value;
         throw UtInput::BadValue(aInput,
                                 "binary file must contain exactly " + std::to_string(expectedBytes) +
                                    " bytes to represent "
                                    "a table of " +
                                    std::to_string(aExpectedNumPoints) + " " + (usingDoubles ? "doubles" : "floats") +
                                    ", found " + std::to_string(mappedMem.MappedSize()) + " bytes");
      }

      const unsigned char* rawDvArray = &mappedMem.GetData()[header_bytes];

      // reinterpret_cast used on std::byte*, char*, and unsigned char* is valid.
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      return reinterpret_cast<const Real*>(rawDvArray);
   }

   std::unique_ptr<UtTable::Table> LoadTable(UtInput&           aInput,
                                             UtInput::ValueType aDV_UnitType,
                                             const ValueCheck&  aDV_ValueCheck,
                                             const Parameters&  aParameters)
   {
      UtInputBlock inputBlock(aInput);

      std::string command;
      while (inputBlock.ReadCommand(command))
      {
         if (command == "independent_variable")
         {
            if (ivCount >= cMAX_DIMENSIONS)
            {
               throw UtInput::BadValue(aInput,
                                       "A maximum of " + std::to_string(cMAX_DIMENSIONS) +
                                          " independent_variables may be specified");
            }
            if (mDV.IsValid())
            {
               throw UtInput::OutOfContext(aInput, "independent_variable's must appear before dependent_variable");
            }

            ProcessIV(aInput, aParameters);
         }
         else if (command == "dependent_variable")
         {
            if (ivCount == 0)
            {
               throw UtInput::OutOfContext(aInput, "dependent_variable must follow independent_variable's");
            }
            if (mDV.IsValid())
            {
               throw UtInput::OutOfContext(aInput, "dependent_variable has already been defined");
            }

            ProcessDV(aInput, aDV_UnitType, aDV_ValueCheck);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }

      // Independent and dependent variable data should now be read in. Create the table object.
      if (ivCount == 0)
      {
         throw UtInput::BadValue(aInput, "At least one 'independent_variable' must be specified.");
      }

      if (!mDV.IsValid())
      {
         throw UtInput::BadValue(aInput, "'dependent_variable' was not specified.");
      }

      try
      {
         return CreateTable<0>::op(*this);
      }
      catch (UtException& e)
      {
         // rethrow with the additional information of the aInput parsing location.
         throw UtInput::BadValue(aInput, e.what());
      }
   }

   void ProcessIV(UtInput& aInput, const Parameters& aParameters)
   {
      UtInputBlock ivBlock(aInput);

      // Read the variable name and make sure it is a name known by the caller.
      std::string name;
      aInput.ReadValue(name);

      DataType           dataType;
      UtInput::ValueType unitType;
      ValueCheck         valueLimit;
      size_t             dataIndex;
      if (!aParameters.GetAttributes(name, dataType, unitType, valueLimit, dataIndex))
      {
         throw UtInput::BadValue(aInput, "'" + name + "' is not a known independent variable");
      }

      Units units{unitType};
      bool  useDoubles        = true;
      bool  usingEqualSpacing = false;
      ;
      double lineStartValue{};
      double lineEndValue{};

      std::string command;

      size_t ivSize = 0;

      std::unique_ptr<float[]>  floatArray;
      std::unique_ptr<double[]> doubleArray;

      MemMapHolder ivBinaryFile;

      auto& iv = mIVs[ivCount];

      auto isDataSpecified = [&]
      { return usingEqualSpacing || ivBinaryFile.IsValid() || floatArray || doubleArray || iv.IsValid(); };

      while (ivBlock.ReadCommand(command))
      {
         if (units.ProcessInput(aInput))
         {
         }
         else if (command == "precision")
         {
            ReadPrecision(aInput, useDoubles);
         }
         else if (command == "extrapolate")
         {
            iv.mAllowExtrapolation = true;
         }
         else if (command == "linear_equation")
         {
            if (isDataSpecified())
            {
               throw UtInput::OutOfContext(aInput, "command for specifing data points has already been used");
            }

            std::string cmd;
            aInput.ReadValue(cmd);
            aInput.StringEqual(cmd, "from");
            lineStartValue = ReadValue(aInput, units.mType, units.mUnits, valueLimit);

            aInput.ReadValue(cmd);
            aInput.StringEqual(cmd, "to");
            lineEndValue = ReadValue(aInput, units.mType, units.mUnits, valueLimit);

            if (lineStartValue == lineEndValue)
            {
               throw UtInput::BadValue(aInput, "linear_equation from/to can't be the same value");
            }

            usingEqualSpacing = true;
         }
         else if (command == "num_points")
         {
            unsigned int num_points;
            aInput.ReadValue(num_points);
            aInput.ValueGreaterOrEqual(num_points, 2U); // must have atleast 2 points
            ivSize = num_points;
         }
         else if (command == "binary_data_file")
         {
            if (isDataSpecified())
            {
               throw UtInput::OutOfContext(aInput, "cannot use multiple commands that specify the data points");
            }

            ivBinaryFile.mMappedFile = OpenBinaryFile(aInput);
         }
         else
         {
            aInput.PushBack(command);

            if (isDataSpecified())
            {
               throw UtInput::OutOfContext(
                  aInput,
                  "Cannot specify inline value list when using a command that pulls in/generates the value list");
            }

            auto specifiedSize = ivSize; // non-zero if 'num_points' command was used

            if (useDoubles)
            {
               ReadIV_Array(ivBlock, units.mType, units.mUnits, valueLimit, ivSize, doubleArray);
            }
            else
            {
               ReadIV_Array(ivBlock, units.mType, units.mUnits, valueLimit, ivSize, floatArray);
            }
            // if num_points command was used, perform a verification that it matched the number of inline data values.
            if (specifiedSize != 0 && specifiedSize != ivSize)
            {
               throw UtInput::BadValue(aInput,
                                       "specified 'num_points' did not match up with the number of data points listed");
            }

            // We should have read up to the end of the block, so just exit the while loop.
            break;
         }
      }

      // Create a table object of the specific type.
      if (doubleArray != nullptr)
      {
         iv.mDoubleIV = ut::make_unique<IV_Table<double>>(doubleArray.release(), ivSize, cOWNER_CLASS, name);
         iv.mDoubleIV->SelectLookupMethod(iv.mAllowExtrapolation);
         // if look up method is to use a linear_equation, then use the LinearTag specialization
         // of an IV_Table, so as not to waste unnecessary space storing the double array.
         if (iv.mDoubleIV->GetLookupMethod() == LookupMethod::cLM_LE ||
             iv.mDoubleIV->GetLookupMethod() == LookupMethod::cLM_LEX)
         {
            lineStartValue = iv.mDoubleIV->Get(0);
            lineEndValue   = iv.mDoubleIV->Get(iv.mDoubleIV->Size() - 1);
            iv.mLinearIV   = ut::make_unique<IV_Table<LinearTag>>(lineStartValue, lineEndValue, ivSize, name);
            iv.mLinearIV->SelectLookupMethod(iv.mAllowExtrapolation);
            iv.mDoubleIV = nullptr;
         }
      }
      else if (floatArray != nullptr)
      {
         iv.mFloatIV = ut::make_unique<IV_Table<float>>(floatArray.release(), ivSize, cOWNER_CLASS, name);
         iv.mFloatIV->SelectLookupMethod(iv.mAllowExtrapolation);
         // if look up method is to use a linear_equation, then use the LinearTag specialization
         // of an IV_Table, so as not to waste unnecessary space storing the float array.
         if (iv.mFloatIV->GetLookupMethod() == LookupMethod::cLM_LE ||
             iv.mFloatIV->GetLookupMethod() == LookupMethod::cLM_LEX)
         {
            lineStartValue = iv.mFloatIV->Get(0);
            lineEndValue   = iv.mFloatIV->Get(iv.mFloatIV->Size() - 1);
            iv.mLinearIV   = ut::make_unique<IV_Table<LinearTag>>(lineStartValue, lineEndValue, ivSize, name);
            iv.mLinearIV->SelectLookupMethod(iv.mAllowExtrapolation);
            iv.mFloatIV = nullptr;
         }
      }
      else if (usingEqualSpacing)
      {
         if (ivSize == 0)
         {
            throw UtInput::BadValue(aInput, "'num_points' must be specified when using 'linear_equation' command'");
         }
         iv.mLinearIV = ut::make_unique<IV_Table<LinearTag>>(lineStartValue, lineEndValue, ivSize, name);
         iv.mLinearIV->SelectLookupMethod(iv.mAllowExtrapolation);
      }
      else if (ivBinaryFile.IsValid())
      {
         if (useDoubles)
         {
            auto* doublesArrayMap = InterpretMappedMemory<double>(aInput, ivBinaryFile, units, valueLimit, ivSize);
            auto  memMapTable =
               ut::make_unique<MemMapped_Table<IV_Table<double>>>(doublesArrayMap, ivSize, cOWNER_CALLER, name);
            memMapTable->SetMemoryMappedFile(std::move(ivBinaryFile.mMappedFile));
            iv.mDoubleIV = std::move(memMapTable);
            iv.mDoubleIV->SelectLookupMethod(iv.mAllowExtrapolation);
         }
         else
         {
            auto* floatsArrayMap = InterpretMappedMemory<float>(aInput, ivBinaryFile, units, valueLimit, ivSize);
            auto  memMapTable =
               ut::make_unique<MemMapped_Table<IV_Table<float>>>(floatsArrayMap, ivSize, cOWNER_CALLER, name);
            memMapTable->SetMemoryMappedFile(std::move(ivBinaryFile.mMappedFile));
            iv.mFloatIV = std::move(memMapTable);
            iv.mFloatIV->SelectLookupMethod(iv.mAllowExtrapolation);
         }
      }
      else
      {
         throw UtInput::BadValue(aInput, "No independent_variable values were provided");
      }

      dvSize *= ivSize;
      ++ivCount;
   }

   void ProcessDV(UtInput& aInput, UtInput::ValueType aUnitType, const ValueCheck& aDV_ValueCheck)
   {
      Units units{aUnitType};
      bool  useDoubles = true;

      UtInputBlock dvBlock(aInput);
      std::string  command;

      UtMemoryMappedFile binaryFile;

      while (dvBlock.ReadCommand(command))
      {
         if (units.ProcessInput(aInput))
         {
         }
         else if (command == "precision")
         {
            ReadPrecision(aInput, useDoubles);
         }
         else if (command == "interpolation")
         {
            std::string interpolation;
            aInput.ReadValue(interpolation);
            if (interpolation == "linear")
            {
               mDV.mInterpolation = InterpolationType::eLinear;
            }
            else if (interpolation == "nearest")
            {
               mDV.mInterpolation = InterpolationType::eNearest;
               for (size_t i = 0; i < ivCount; ++i)
               {
                  if (mIVs[i].mAllowExtrapolation)
                  {
                     throw UtInput::BadValue(aInput, "can't use nearest interpolation when any of the indendent_varaibles are performing extrapolation");
                  }
               }
            }
            else
            {
               throw UtInput::BadValue(aInput, "supported values for interpolation are 'linear' and 'nearest'");
            }
         }
         else if (command == "binary_data_file")
         {
            if (mDV.mMappedFile.IsValid())
            {
               throw UtInput::BadValue(aInput, "cannot specify binary_data_file twice");
            }

            mDV.mMappedFile.mMappedFile = OpenBinaryFile(aInput);
         }
         else if (command == "save_as_rectangular_indexed_binary_file")
         {
            aInput.ReadValueQuoted(mSaveDvAsBinaryFile);
            mSaveDvAsBinaryFile = aInput.SubstitutePathVariables(mSaveDvAsBinaryFile);
         }
         else if (command == "save_as_morton_indexed_binary_file")
         {
            aInput.ReadValueQuoted(mSaveDvAsMortonFile);
            mSaveDvAsMortonFile = aInput.SubstitutePathVariables(mSaveDvAsMortonFile);
         }
         else
         {
            aInput.PushBack(command);

            if (mDV.mMappedFile.IsValid())
            {
               throw UtInput::BadValue(aInput, "Cannot specify inline value list when in combination with 'binary_data_file' command");
            }

            if (useDoubles)
            {
               ReadDV_Array(dvBlock, units.mType, units.mUnits, aDV_ValueCheck, dvSize, mDV.mMutableDoubles);
            }
            else
            {
               ReadDV_Array(dvBlock, units.mType, units.mUnits, aDV_ValueCheck, dvSize, mDV.mMutableFloats);
            }
            // We should have read up to the end of the block, so just exit the while loop.
            break;
         }
      }

      if (mDV.mMappedFile.IsValid())
      {
         if (useDoubles)
         {
            mDV.GetDoubleArray() = InterpretMappedMemory<double>(aInput, mDV.mMappedFile, units, aDV_ValueCheck, dvSize);
         }
         else
         {
            mDV.GetFloatArray() = InterpretMappedMemory<float>(aInput, mDV.mMappedFile, units, aDV_ValueCheck, dvSize);
         }
      }

      // Make sure the values were actually specified!
      if (!mDV.IsValid())
      {
         throw UtInput::OutOfContext(aInput, "No dependent_variable values were provided");
      }
   }

   IV_Variant  mIVs[cMAX_DIMENSIONS];
   DV_Array    mDV;
   std::string mSaveDvAsBinaryFile{}; // default empty means don't save, otherwise the path to file to be saved as
   std::string mSaveDvAsMortonFile{}; // default empty means don't save, otherwise the path to file to be saved as
   size_t      dvSize  = 1;
   size_t      ivCount = 0;
};

template<>
struct CreateTable<cMAX_DIMENSIONS>
{
   //! ensure the maximum representable morton index using the sizes fits into a size_t.
   static void VerifySizesFitIntoMortonCode(ut::span<const size_t> aSizes, size_t aMaxIndex)
   {
      auto popcount = [](size_t bb)
      {
         size_t count = 0;
         while (bb != 0)
         {
            bb &= bb - 1;
            ++count;
         }
         return count;
      };

      size_t expectedCount = popcount(aMaxIndex);
      size_t indicesCount  = 0;
      for (size_t ivSize : aSizes)
      {
         indicesCount += popcount(ivSize - 1);
      }

      if (expectedCount != indicesCount)
      {
         throw UtException("Cannot save using morton indexing with " + std::to_string(aSizes.size()) +
                           " dimensional data because "
                           "the maximum morton index would exceed the size of size_t. "
                           "to get the optimal smallest morton index for a dataset, sort the list of ivs "
                           "by reverse size, e.g. use 360x180 instead of 180x360.");
      }
   }

   template<typename Indexer, typename DV, size_t Dim>
   static void SaveDvTable(const std::string&             aPath,
                           const DV*                      aDvTable,
                           const std::array<size_t, Dim>& aIvSizes,
                           IndexScheme                    aIndexerType)
   {
      size_t num_elements = 1;
      for (auto size : aIvSizes)
      {
         num_elements *= size;
      }
      size_t saved_elements = num_elements;

      Indexer indexer{aIvSizes};
      auto    Get = [&](std::array<size_t, Dim>& aIndexs) { return aDvTable[indexer.Index(aIndexs)]; };

      // --------------------------------
      // setup morton indexer
      constexpr auto   bitsPerField = (sizeof(size_t) * CHAR_BIT / Dim) - (Dim == 1 ? 1 : 0);
      constexpr size_t lutBits      = bitsPerField > 8 ? 8 : bitsPerField;
      using mortonLUT               = mortonnd::MortonIndexerLUT<Dim, bitsPerField, lutBits>;
      mortonLUT mortonIndexer{aIvSizes};

      if (aIndexerType == IndexScheme::eMorton)
      {
         saved_elements = mortonIndexer.MaxIndex(aIvSizes) + 1;
         VerifySizesFitIntoMortonCode(aIvSizes, saved_elements - 1);
      }

      // -----------------------------
      // create file
      constexpr bool   useDoubles  = std::is_same<DV, double>::value;
      constexpr size_t header_size = useDoubles ? 24 : 16;
      size_t           total_bytes = header_size + saved_elements * sizeof(DV);

      UtMemMapWriter writer;
      bool           success = writer.CreateFileMap(aPath, total_bytes);
      if (!success)
      {
         throw UtException("Failed to create binary_file: '" + aPath + "'.");
      }

      // -------------------------
      // setup writing table
      std::array<size_t, Dim> first{};
      first.fill(0);
      DV minVal = Get(first);
      DV maxVal = Get(first);

      // ----------------
      // write the table

      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      auto* file_table = reinterpret_cast<DV*>(&writer.GetData()[header_size]);
      for (size_t i = 0; i < num_elements; ++i)
      {
         // get the index of each iv for element i
         std::array<size_t, Dim> ivIndexs{};
         auto                    id = i;
         for (std::ptrdiff_t d = Dim - 1; d >= 0; --d)
         {
            ivIndexs[d] = id % aIvSizes[d];
            id          = id / (aIvSizes[d]);
         }

         // get value of element i
         auto element = Get(ivIndexs);

         if (element < minVal)
         {
            minVal = element;
         }
         if (element > maxVal)
         {
            maxVal = element;
         }

         // encode the index and write to the table
         if (aIndexerType == IndexScheme::eMorton)
         {
            file_table[mortonIndexer.Index(ivIndexs)] = element;
         }
         else
         {
            file_table[i] = element;
         }
      }

      // -------------------------
      // write the header

      auto* data = writer.GetData();

      // zero-ize the header
      for (size_t i = 0; i < header_size; i++)
      {
         data[i] = 0;
      }

      data[0]        = useDoubles ? 0 : 1;              // first specifies precision: 0=double, 1=float
      data[1]        = 1;                               // second byte is validity flag for min/max value in header
      data[2]        = static_cast<char>(aIndexerType); // third byte is indexing scheme: 0=rectangle, 1=morton
      file_table[-1] = maxVal;
      file_table[-2] = minVal;

      writer.Close();
   }

   // ------------------------------------------------
   // save the dv table to file if commanded to do so
   template<typename Indexer, typename DV, size_t Dim>
   static void DoSaveAsConversions(TableLoader& aBuilder, const DV* aDV_ArrayPtr, const std::array<size_t, Dim>& ivSizes)
   {
      if (!aBuilder.mSaveDvAsBinaryFile.empty())
      {
         SaveDvTable<Indexer>(aBuilder.mSaveDvAsBinaryFile, aDV_ArrayPtr, ivSizes, IndexScheme::eRectangular);
      }
      if (!aBuilder.mSaveDvAsMortonFile.empty())
      {
         if /*constexpr*/ (Dim == 1)
         {
            throw UtException("It makes no sense to save a 1D table using morton indexing.");
         }
         else
         {
            SaveDvTable<Indexer>(aBuilder.mSaveDvAsMortonFile, aDV_ArrayPtr, ivSizes, IndexScheme::eMorton);
         }
      }
   }

   template<typename Indexer, typename Interpolator, typename DV, typename... IVHolders>
   static std::unique_ptr<UtTable::Table> MakeTable(TableLoader& aBuilder, const DV* aDV_ArrayPtr, IVHolders&&... aIVs)
   {
      constexpr size_t Dim = sizeof...(IVHolders); // number of dimensions of the table being created.

      using Table_XD = Table_ND<Dim, DV, Indexer, Interpolator>;
      using DV_Table = DV_Table_ND<Dim, DV, Indexer, Interpolator>;

      using IV_Array = typename Table_XD::IV_Array;
      // ------------------------------------------------
      // create the dependent_variable table
      std::unique_ptr<DV_Table> dv_table;
      std::array<size_t, Dim>   ivSizes{aIVs.GetIV_Table().Size()...};
      if (aBuilder.mDV.mMappedFile.IsValid())
      {
         auto dv_mapped = ut::make_unique<MemMapped_Table<DV_Table>>(aDV_ArrayPtr, ivSizes, cOWNER_CALLER);
         dv_mapped->SetMemoryMappedFile(std::move(aBuilder.mDV.mMappedFile.mMappedFile));
         if (aBuilder.mDV.mMappedFile.mMinMaxValid)
         {
            dv_mapped->SetMinMax(aBuilder.mDV.mMappedFile.mMinValue, aBuilder.mDV.mMappedFile.mMaxValue);
         }
         dv_table = std::move(dv_mapped);
      }
      else
      {
         // if the mutable array is non-null, then the dv table is taking ownership of the array
         Owner ownership = ((aBuilder.mDV.GetDoubleArray() != nullptr && aBuilder.mDV.mMutableDoubles == nullptr) ||
                            (aBuilder.mDV.GetFloatArray() != nullptr && aBuilder.mDV.mMutableFloats == nullptr)) ?
                              cOWNER_CALLER :
                              cOWNER_CLASS;

         dv_table = ut::make_unique<DV_Table>(aDV_ArrayPtr, ivSizes, ownership);
      }

      // ------------------------------------------------
      // save the dv table to file if commanded to do so
      DoSaveAsConversions<Indexer>(aBuilder, aDV_ArrayPtr, ivSizes);

      return ut::make_unique<Table_XD>(std::move(dv_table), IV_Array{std::move(aIVs)...});
   }
   // pass through helper for type switching between the underlying type used by the DV
   template<typename Indexer, typename Interpolator, typename... IVHolders>
   static std::unique_ptr<UtTable::Table> MakeOracle2(TableLoader& aBuilder, IVHolders&&... aIVs)
   {
      std::unique_ptr<UtTable::Table> tablePtr;
      if (aBuilder.mDV.GetDoubleArray() != nullptr)
      {
         tablePtr = MakeTable<Indexer, Interpolator>(aBuilder, aBuilder.mDV.GetDoubleArray(), std::move(aIVs)...);
         aBuilder.mDV.mMutableDoubles.release(); // ownership was passed to the table
      }
      else // (aBuilder.mDV.GetFloatArray() != nullptr)
      {
         tablePtr = MakeTable<Indexer, Interpolator>(aBuilder, aBuilder.mDV.GetFloatArray(), std::move(aIVs)...);
         aBuilder.mDV.mMutableFloats.release(); // ownership was passed to the table
      }
      return tablePtr;
   }

   template<typename Indexer, typename... IVHolders>
   static std::unique_ptr<UtTable::Table> MakeOracle(TableLoader& aBuilder, IVHolders&&... aIVs)
   {
      if (aBuilder.mDV.mInterpolation == InterpolationType::eNearest)
      {
         return MakeOracle2<Indexer, NearestInterpolator>(aBuilder, std::move(aIVs)...);
      }
      else // (aBuilder.mDV.mInterpolation == InterpolationType::eLinear)
      {
         return MakeOracle2<Indexer, LinearInterpolator>(aBuilder, std::move(aIVs)...);
      }
   }

   // pass through helper for type switching between the underlying indexer that the DV array uses.
   template<typename... IVHolders>
   static std::unique_ptr<UtTable::Table> op(TableLoader& aBuilder, IVHolders&&... aIVs)
   {
      constexpr size_t Dims = sizeof...(IVHolders);
      if (aBuilder.mDV.mMappedFile.mIndexScheme == IndexScheme::eRectangular)
      {
         return MakeOracle<RectangularIndexer<Dims>>(aBuilder, std::move(aIVs)...);
      }
      // else IndexScheme::eMorton
      constexpr size_t bitsPerField = sizeof(size_t) * CHAR_BIT / Dims;
      constexpr size_t lutBits      = bitsPerField < 8 ? bitsPerField : 8;
      using mortonLUT               = mortonnd::MortonIndexerLUT<Dims, bitsPerField, lutBits>;
      std::array<size_t, Dims> ivSizes{aIVs.GetIV_Table().Size()...};
      mortonLUT                portableMortan{ivSizes};
      auto                     maxMortanCode = portableMortan.MaxIndex(ivSizes);
      VerifySizesFitIntoMortonCode(ivSizes, maxMortanCode);

#ifdef BMI2_FOUND
#ifdef RUNTIME_CHECK_BMI2
      // ideally we would always use runtime checking for BMI2, since that gives the best
      // performance, but its very tedious to do with GCC, so this is only done with MSVC
      bool useBMI = IsBMI2_Supported();
      if (useBMI)
      {
         return MakeOracle<mortonnd::MortonIndexerBMI<Dims>>(aBuilder, std::move(aIVs)...);
      }
      return MakeOracle<mortonLUT>(aBuilder, std::move(aIVs)...);
#else
      // GCC/Clang targeting machine with BMI2 support
      return MakeOracle<mortonnd::MortonIndexerBMI<Dims>>(aBuilder, std::move(aIVs)...);
#endif
#else
      // GCC/Clang targeting machine missing BMI2 support
      return MakeOracle<mortonLUT>(aBuilder, std::move(aIVs)...);
#endif
   }

   // override for table of 1 dimension, as morton indexing is not applicable.
   static std::unique_ptr<UtTable::Table> op(TableLoader& aBuilder, UtTable::RegularTable::detail::IV_Holder&& aIV)
   {
      return MakeOracle<RectangularIndexer<1>>(aBuilder, std::move(aIV));
   }

   // override for table of 0 dimensions, this wouldn't be needed if the 'if constexpr' was used in the generic CreateTable.
   static std::unique_ptr<UtTable::Table> op(TableLoader& aBuilder) { return nullptr; }
};

// meta-template programming class for creating a Table_ND with the correct template parameters
template<size_t Dimension>
template<typename... IVHolders>
std::unique_ptr<UtTable::Table> CreateTable<Dimension>::op(TableLoader& aBuilder, IVHolders&&... aIVs)
{
   if /*constexpr*/ (sizeof...(IVHolders) > 0)
   {
      // check if reached the dimensional size of the table being built.
      if (sizeof...(IVHolders) == aBuilder.ivCount)
      {
         // pass through to CreateTable<cMAX_DIMENSIONS> which actually
         // handles the logic of building the table
         return CreateTable<cMAX_DIMENSIONS>::op(aBuilder, std::move(aIVs)...);
      }
   }

   using IV_Holder = UtTable::RegularTable::detail::IV_Holder;

   //-----------------------------------------------
   // template type switch logic for choosing which
   // IV table type to give to the Table_ND

   auto& iv = aBuilder.mIVs[Dimension];
   if (iv.mDoubleIV != nullptr)
   {
      return CreateTable<Dimension + 1>::op(aBuilder, std::move(aIVs)..., IV_Holder{std::move(iv.mDoubleIV)});
   }
   if (iv.mFloatIV != nullptr)
   {
      return CreateTable<Dimension + 1>::op(aBuilder, std::move(aIVs)..., IV_Holder{std::move(iv.mFloatIV)});
   }
   // else (iv.mLinearIV != nullptr)
   return CreateTable<Dimension + 1>::op(aBuilder, std::move(aIVs)..., IV_Holder{std::move(iv.mLinearIV)});
}

//! @param aPath file path to save the file to.
//! @param aSaveAsFormat format type to encode the binary file with
//! @param aDimensions the sizes of each iv array used to index the dv array
//! @param aDV_Values the array of dv values, rectangular indexed, to be written to a binary file.
template<class ValTypeSpan>
static void SaveDvTableToBinaryFileImpl(const std::string&         aPath,
                                        IndexScheme                aSaveAsFormat,
                                        const std::vector<size_t>& aDimensions,
                                        ValTypeSpan                aDV_Values)
{
   TableLoader builder{};

   // initialize ivs, only caring about their sizes.
   builder.ivCount = aDimensions.size();
   if (aDimensions.size() > cMAX_DIMENSIONS)
   {
      throw UtException("A maximum of " + std::to_string(cMAX_DIMENSIONS) + " dimensions may be used in a regular_table");
   }

   size_t totalSize = 1;
   for (size_t i = 0; i < aDimensions.size(); ++i)
   {
      size_t iv_size = aDimensions[i];
      if (iv_size == 0)
      {
         throw UtException("independent variable's can't have a dimensional size of zero");
      }
      totalSize *= iv_size;
      builder.mIVs[i].mLinearIV = ut::make_unique<IV_Table<LinearTag>>(0, 1, iv_size, std::to_string(i));
   }
   if (totalSize != aDV_Values.size())
   {
      throw UtException("size of dependent variable table must be the product of the size of each of its independent "
                        "variable dimensions");
   }
   builder.dvSize = aDV_Values.size();
   builder.mDV.SetNonOwningArray(aDV_Values);
   if (aSaveAsFormat == IndexScheme::eMorton)
   {
      builder.mSaveDvAsMortonFile = aPath;
   }
   else // IndexScheme::eRectangular
   {
      builder.mSaveDvAsBinaryFile = aPath;
   }

   CreateTable<0>::op(builder);
}

} // end anonymous namespace

// ================================================================================================
std::unique_ptr<UtTable::Table> UtTable::RegularTable::LoadInstance(UtInput&           aInput,
                                                                    UtInput::ValueType aDV_UnitType,
                                                                    const ValueCheck&  aDV_ValueCheck,
                                                                    const Parameters&  aParameters)
{
   return TableLoader{}.LoadTable(aInput, aDV_UnitType, aDV_ValueCheck, aParameters);
}

//! @param aPath file path to save the file to.
//! @param aSaveAsFormat format type to encode the binary file with
//! @param aDimensions the sizes of each iv array used to index the dv array
//! @param aDV_Values the array of dv values, rectangular indexed, to be written to a binary file.
void UtTable::RegularTable::SaveDvTableToBinaryFile(const std::string&         aPath,
                                                    IndexScheme                aSaveAsFormat,
                                                    const std::vector<size_t>& aDimensions,
                                                    ut::span<const double>     aDV_Values)
{
   SaveDvTableToBinaryFileImpl(aPath, aSaveAsFormat, aDimensions, aDV_Values);
}

//! @param aPath file path to save the file to.
//! @param aSaveAsFormat format type to encode the binary file with
//! @param aDimensions the sizes of each iv array used to index the dv array
//! @param aDV_Values the array of dv values, rectangular indexed, to be written to a binary file.
void UtTable::RegularTable::SaveDvTableToBinaryFile(const std::string&         aPath,
                                                    IndexScheme                aSaveAsFormat,
                                                    const std::vector<size_t>& aDimensions,
                                                    ut::span<const float>      aDV_Values)
{
   SaveDvTableToBinaryFileImpl(aPath, aSaveAsFormat, aDimensions, aDV_Values);
}

// ================================================================================================
std::unique_ptr<UtTable::Table> UtTable::RegularTable::LoadCurve(UtInput&           aInput,
                                                                 UtInput::ValueType aDV_UnitType,
                                                                 const ValueCheck&  aDV_ValueCheck,
                                                                 const Parameters&  aParameters)
{
   std::string        ivName;
   UtInput::ValueType ivUnitType = UtInput::cINVALID;
   std::string        ivUnits;
   ValueCheck         ivValueCheck;
   bool               ivDouble = true;

   std::string dvUnits;
   bool        dvDouble = true;

   bool allowExtrapolation = false;

   std::string  command;
   UtInputBlock inputBlock(aInput);
   while (inputBlock.ReadCommand(command))
   {
      if (command == "independent_variable")
      {
         // Read the variable name and make sure it is a name known by the caller.
         aInput.ReadValue(ivName);

         DataType dataType;
         size_t   dataIndex;
         if (!aParameters.GetAttributes(ivName, dataType, ivUnitType, ivValueCheck, dataIndex))
         {
            throw UtInput::BadValue(aInput, "'" + ivName + "' is not a known independent variable");
         }

         while (inputBlock.ReadCommand(command))
         {
            if (command == "units")
            {
               ReadUnits(aInput, ivUnitType, ivUnits);
            }
            else if (command == "precision")
            {
               ReadPrecision(aInput, ivDouble);
            }
            else if (command == "extrapolate")
            {
               allowExtrapolation = true;
            }
            else
            {
               aInput.PushBack(command);
               break;
            }
         }
         if (command == inputBlock.GetBlockTerminator())
         {
            throw UtInput::BadValue(aInput, "Unexpected end of table definition");
         }
      }
      else if (command == "dependent_variable")
      {
         while (inputBlock.ReadCommand(command))
         {
            if (command == "units")
            {
               ReadUnits(aInput, aDV_UnitType, dvUnits);
            }
            else if (command == "precision")
            {
               ReadPrecision(aInput, dvDouble);
            }
            else
            {
               aInput.PushBack(command);
               break;
            }
         }
         if (command == inputBlock.GetBlockTerminator())
         {
            throw UtInput::BadValue(aInput, "Unexpected end of table definition");
         }
      }
      else if (!ivName.empty())
      {
         aInput.PushBack(command);
         return ReadCurve(inputBlock,
                          allowExtrapolation,
                          ivName,
                          ivUnitType,
                          ivUnits,
                          ivValueCheck,
                          ivDouble,
                          aDV_UnitType,
                          dvUnits,
                          aDV_ValueCheck,
                          dvDouble);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   return nullptr;
}

// ================================================================================================
//! This form is used when the application is enforcing the variables that are the x and y values.
//! (The 'Parameters' block is not used).
std::unique_ptr<UtTable::Table> UtTable::RegularTable::LoadCurve(UtInput&           aInput,
                                                                 const std::string& aIV_Name,
                                                                 UtInput::ValueType aIV_UnitType,
                                                                 const std::string& aIV_Units,
                                                                 const ValueCheck&  aIV_ValueCheck,
                                                                 UtInput::ValueType aDV_UnitType,
                                                                 const std::string& aDV_Units,
                                                                 const ValueCheck&  aDV_ValueCheck)
{
   std::string ivUnits(aIV_Units);
   std::string dvUnits(aDV_Units);
   bool        ivDouble(true);
   bool        dvDouble(true);
   bool        allowExtrapolation(false);

   std::string  command;
   UtInputBlock inputBlock(aInput);
   while (inputBlock.ReadCommand(command))
   {
      if (command == "independent_variable")
      {
         while (inputBlock.ReadCommand(command))
         {
            if (command == "units")
            {
               ReadUnits(aInput, aIV_UnitType, ivUnits);
            }
            else if (command == "precision")
            {
               ReadPrecision(aInput, ivDouble);
            }
            else if (command == "extrapolate")
            {
               allowExtrapolation = true;
            }
            else
            {
               aInput.PushBack(command);
               break;
            }
         }
         if (command == inputBlock.GetBlockTerminator())
         {
            throw UtInput::BadValue(aInput, "Unexpected end of table definition");
         }
      }
      else if (command == "dependent_variable")
      {
         while (inputBlock.ReadCommand(command))
         {
            if (command == "units")
            {
               ReadUnits(aInput, aDV_UnitType, dvUnits);
            }
            else if (command == "precision")
            {
               ReadPrecision(aInput, dvDouble);
            }
            else
            {
               aInput.PushBack(command);
               break;
            }
         }
         if (command == inputBlock.GetBlockTerminator())
         {
            throw UtInput::BadValue(aInput, "Unexpected end of table definition");
         }
      }
      else
      {
         aInput.PushBack(command);
         return ReadCurve(inputBlock,
                          allowExtrapolation,
                          aIV_Name,
                          aIV_UnitType,
                          ivUnits,
                          aIV_ValueCheck,
                          ivDouble,
                          aDV_UnitType,
                          dvUnits,
                          aDV_ValueCheck,
                          dvDouble);
      }
   }
   return nullptr;
}

// ================================================================================================
template<typename T>
void UtTable::RegularTable::ReadDV_Array(UtInputBlock&         aInputBlock,
                                         UtInput::ValueType    aUnitType,
                                         const std::string&    aUnits,
                                         const ValueCheck&     aValueCheck,
                                         size_t                aExpectedSize,
                                         std::unique_ptr<T[]>& aArrayPtr)
{
   aArrayPtr               = ut::make_unique<T[]>(aExpectedSize);
   size_t      currentSize = 0;
   std::string command;
   UtInput&    input = aInputBlock.GetInput();
   while (aInputBlock.ReadCommand(command))
   {
      input.PushBack(command);
      double value = ReadValue(input, aUnitType, aUnits, aValueCheck);

      // Store the value if the expected array size hasn't been read
      if (currentSize < aExpectedSize)
      {
         aArrayPtr[currentSize] = static_cast<T>(value);
      }
      ++currentSize;
   }

   if (currentSize < aExpectedSize)
   {
      throw UtInput::BadValue(input, "Not enough dependent_variable values provided");
   }
   else if (currentSize > aExpectedSize)
   {
      throw UtInput::BadValue(input, "Too many dependent_variables values provided");
   }
}

// ================================================================================================
template<typename T>
void UtTable::RegularTable::ReadIV_Array(UtInputBlock&         aInputBlock,
                                         UtInput::ValueType    aUnitType,
                                         const std::string&    aUnits,
                                         const ValueCheck&     aValueCheck,
                                         size_t&               aSize,
                                         std::unique_ptr<T[]>& aArrayPtr)
{
   std::vector<T> values;
   std::string    command;
   UtInput&       input = aInputBlock.GetInput();
   while (aInputBlock.ReadCommand(command))
   {
      input.PushBack(command);
      double value = ReadValue(input, aUnitType, aUnits, aValueCheck);

      // Make sure values are monotonically ascending
      if ((!values.empty()) && (value <= values.back()))
      {
         throw UtInput::BadValue(input, "independent values must be monotonically ascending");
      }
      values.push_back(static_cast<T>(value));
   }

   // Make sure enough values were provided.
   if (values.size() < 2)
   {
      throw UtInput::BadValue(input, "At least two values must be specified");
   }

   // Copy the values from the vector<> to a dynamically allocated array.
   aSize     = values.size();
   aArrayPtr = ut::make_unique<T[]>(aSize);
   for (size_t i = 0; i < aSize; ++i)
   {
      aArrayPtr[i] = values[i];
   }
}

// ================================================================================================
//! Internal routine to read the actual curve x/y values.
//! This is called by the two forms of LoadCurve. It invokes the templated form using the precision
//! requested for each type.
std::unique_ptr<UtTable::Table> UtTable::RegularTable::ReadCurve(UtInputBlock&      aInputBlock,
                                                                 bool               aAllowExtrapolation,
                                                                 const std::string& aIV_Name,
                                                                 UtInput::ValueType aIV_UnitType,
                                                                 const std::string& aIV_Units,
                                                                 const ValueCheck&  aIV_ValueCheck,
                                                                 bool               aIV_Double,
                                                                 UtInput::ValueType aDV_UnitType,
                                                                 const std::string& aDV_Units,
                                                                 const ValueCheck&  aDV_ValueCheck,
                                                                 bool               aDV_Double)
{
   if (aIV_Double && aDV_Double)
   {
      return ReadCurve<double, double>(aInputBlock,
                                       aAllowExtrapolation,
                                       aIV_Name,
                                       aIV_UnitType,
                                       aIV_Units,
                                       aIV_ValueCheck,
                                       aDV_UnitType,
                                       aDV_Units,
                                       aDV_ValueCheck);
   }
   else if (aIV_Double && (!aDV_Double))
   {
      return ReadCurve<double, float>(aInputBlock,
                                      aAllowExtrapolation,
                                      aIV_Name,
                                      aIV_UnitType,
                                      aIV_Units,
                                      aIV_ValueCheck,
                                      aDV_UnitType,
                                      aDV_Units,
                                      aDV_ValueCheck);
   }
   else if ((!aIV_Double) && aDV_Double)
   {
      return ReadCurve<float, double>(aInputBlock,
                                      aAllowExtrapolation,
                                      aIV_Name,
                                      aIV_UnitType,
                                      aIV_Units,
                                      aIV_ValueCheck,
                                      aDV_UnitType,
                                      aDV_Units,
                                      aDV_ValueCheck);
   }
   else if ((!aIV_Double) && (!aDV_Double))
   {
      return ReadCurve<float, float>(aInputBlock,
                                     aAllowExtrapolation,
                                     aIV_Name,
                                     aIV_UnitType,
                                     aIV_Units,
                                     aIV_ValueCheck,
                                     aDV_UnitType,
                                     aDV_Units,
                                     aDV_ValueCheck);
   }
   return nullptr;
}

// ================================================================================================
template<typename IV, typename DV>
std::unique_ptr<UtTable::Table> UtTable::RegularTable::ReadCurve(UtInputBlock&      aInputBlock,
                                                                 bool               aAllowExtrapolation,
                                                                 const std::string& aIV_Name,
                                                                 UtInput::ValueType aIV_UnitType,
                                                                 const std::string& aIV_Units,
                                                                 const ValueCheck&  aIV_ValueCheck,
                                                                 UtInput::ValueType aDV_UnitType,
                                                                 const std::string& aDV_Units,
                                                                 const ValueCheck&  aDV_ValueCheck)
{
   std::vector<IV> ivValues;
   std::vector<DV> dvValues;

   std::string command;
   UtInput&    input = aInputBlock.GetInput();
   while (aInputBlock.ReadCommand(command))
   {
      input.PushBack(command);
      double ivValue = ReadValue(input, aIV_UnitType, aIV_Units, aIV_ValueCheck);

      // Make sure values are monotonically ascending
      if ((!ivValues.empty()) && (ivValue <= ivValues.back()))
      {
         throw UtInput::BadValue(input, "independent values must be monotonically ascending");
      }
      ivValues.push_back(static_cast<IV>(ivValue));

      double dvValue = ReadValue(input, aDV_UnitType, aDV_Units, aDV_ValueCheck);
      dvValues.push_back(static_cast<DV>(dvValue));
   }

   // Make sure enough values were provided.
   if (ivValues.size() < 2)
   {
      throw UtInput::BadValue(input, "At least two values must be specified");
   }

   // Copy the values from the vector<> to a dynamically allocated array.
   size_t size       = ivValues.size();
   IV*    ivArrayPtr = new IV[size];
   DV*    dvArrayPtr = new DV[size];
   for (size_t i = 0; i < size; ++i)
   {
      ivArrayPtr[i] = ivValues[i];
      dvArrayPtr[i] = dvValues[i];
   }

   auto ivTablePtr = ut::make_unique<IV_Table<IV>>(ivArrayPtr, size, cOWNER_CLASS, aIV_Name);
   ivTablePtr->SelectLookupMethod(aAllowExtrapolation);
   auto dvTablePtr = ut::make_unique<DV_Table_1D<DV>>(dvArrayPtr, size, cOWNER_CLASS);
   return ut::make_unique<Table_1D<DV, IV>>(std::move(dvTablePtr), std::move(ivTablePtr));
}
