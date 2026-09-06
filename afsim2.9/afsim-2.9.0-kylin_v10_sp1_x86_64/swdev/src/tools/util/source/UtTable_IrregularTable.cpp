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

#include "UtTable_IrregularTable.hpp"

#include <cassert>
#include <iostream>
#include <list>
#include <memory>
#include <stack>
#include <vector>

#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtTable_Lookup.hpp"

// Forward declarations for things defined in this file
namespace UtTable
{
namespace IrregularTable
{
template<typename IV, typename DV>
class IR_Table;

template<typename IV, typename DV>
class SharedData;

enum LookupMethod
{
   cLM_LU,
   cLM_LUX,
   cLM_EXACT
};

struct IV_Type
{
   std::string        mName;
   UtInput::ValueType mUnitType{UtInput::cNON_DIMENSIONAL};
   std::string        mUnits;
   ValueCheck         mValueCheck;
   bool               mAllowExtrapolation{false};
};

using IV_TypeList = std::vector<IV_Type>;

template<typename IV>
struct IndexEntry
{
   IndexEntry(IV aValue)
      : mValue(aValue)
      , mOffset(0)
      , mSize(0)
   {
   }

   IV     mValue;
   size_t mOffset;
   size_t mSize;
};

template<typename IV, typename DV>
std::unique_ptr<Table> LoadTableValues(UtInputBlock&      aInputBlock,
                                       UtInput::ValueType aDV_UnitType,
                                       const std::string& aDV_Units,
                                       const ValueCheck&  aDV_ValueCheck,
                                       const IV_TypeList& aIV_TypeList);
} // namespace IrregularTable
} // namespace UtTable

// ================================================================================================
template<typename IV, typename DV>
class UT_EXPORT UtTable::IrregularTable::SharedData
{
public:
   double InterpolateLeaf(size_t aOffset, size_t aSize, double aValue) const
   {
      // A size of zero is a special indication of a constant
      if (aSize == 0)
      {
         return mLeafDV_Values[aOffset];
      }

      LookupState lookUpState;

      // use the lookup index from the previous lookup as a hint
      // for where to start searching for this lookup's index.
      lookUpState.mIndex = std::min(aSize - 1U, mPreviousLookupIndex.back());

      switch (mLU_Methods.back())
      {
      case cLM_LU:
         LookupLU(static_cast<IV>(aValue), mLeafIV_Values.data() + aOffset, aSize, lookUpState);
         break;
      case cLM_LUX:
         LookupLUX(static_cast<IV>(aValue), mLeafIV_Values.data() + aOffset, aSize, lookUpState);
         break;
      default:
         // Should not occur at leaf level
         break;
      }

      // cache the discovered lookup index, to act as a
      // hint the next time a lookup is performed.
      mPreviousLookupIndex.back() = lookUpState.GetIndex();

      size_t i0  = lookUpState.GetIndex();
      double dv0 = mLeafDV_Values[aOffset + i0];
      double dv1 = mLeafDV_Values[aOffset + i0 + 1];
      double r   = lookUpState.GetRatio();
      double dv  = dv0 + (r * (dv1 - dv0));
      return dv;
   }

   double Interpolate(size_t aLevel, size_t aOffset, size_t aSize, const double aIV_Values[]) const
   {
      static constexpr double ALMOST_ZERO = 0.0000000001;
      static constexpr double ALMOST_ONE  = 0.9999999999;

      // A size of zero is a special indication of a constant
      if (aSize == 0)
      {
         return mLeafDV_Values[aOffset];
      }

      LookupState lookUpState;
      // use the lookup index from the previous lookup as a hint
      // for where to start searching for this lookup's index.
      lookUpState.mIndex = std::min(aSize - 1U, mPreviousLookupIndex[aLevel]);

      switch (mLU_Methods[aLevel])
      {
      case cLM_LU:
         LookupLU(static_cast<IV>(aIV_Values[aLevel]), mIndexValues.data() + aOffset, aSize, lookUpState);
         break;
      case cLM_LUX:
         LookupLUX(static_cast<IV>(aIV_Values[aLevel]), mIndexValues.data() + aOffset, aSize, lookUpState);
         break;
      default:
         assert(mLU_Methods[aLevel] == cLM_LU); // Not supported
         ut::log::error() << "UtTable_IrregularTable: Undefined lookup method.";
         break;
      }
      // cache the discovered lookup index, to act as a
      // hint the next time a lookup is performed.
      mPreviousLookupIndex[aLevel] = lookUpState.GetIndex();

      size_t i0  = aOffset + lookUpState.GetIndex();
      double dv0 = 0.0;
      double dv1 = 0.0;
      double r   = lookUpState.GetRatio();
      if ((aLevel + 2) == mLevels)
      {
         // Next level is the leaf level.
         double iv = aIV_Values[aLevel + 1];
         if (r < ALMOST_ONE)
         {
            dv0 = InterpolateLeaf(mIndexOffset[i0], mIndexSize[i0], iv);
         }
         if (r > ALMOST_ZERO)
         {
            dv1 = InterpolateLeaf(mIndexOffset[i0 + 1], mIndexSize[i0 + 1], iv);
         }
      }
      else
      {
         // Next level is not a leaf
         if (r < ALMOST_ONE)
         {
            dv0 = Interpolate(aLevel + 1, mIndexOffset[i0], mIndexSize[i0], aIV_Values);
         }
         if (r > ALMOST_ZERO)
         {
            dv1 = Interpolate(aLevel + 1, mIndexOffset[i0 + 1], mIndexSize[i0 + 1], aIV_Values);
         }
      }
      double dv = dv0 + (r * (dv1 - dv0));
      return dv;
   }

   double Lookup(const double aIV_Values[]) const { return Interpolate(0, mRootOffset, mRootSize, aIV_Values); }

   std::vector<std::string>  mIV_Names;
   std::vector<LookupMethod> mLU_Methods;

   /// cache of the last found lookup indexs, as benchmarks
   /// showed that performing linear lookups starting at
   /// the previous found index helps performance.
   /// Since the cached values are only used as hint, it's safe to have
   /// read/write thread races on them, which is why this is mutable.
   mutable std::vector<size_t> mPreviousLookupIndex;

   size_t mLevels{0};
   size_t mRootOffset{0};
   size_t mRootSize{0};

   //! @name Index structure for the non-leaf levels
   ///@{
   std::vector<IV>     mIndexValues;
   std::vector<size_t> mIndexOffset;
   std::vector<size_t> mIndexSize;
   ///@}

   //! @name values for the leaf level
   ///@{
   std::vector<IV> mLeafIV_Values;
   std::vector<DV> mLeafDV_Values;
   ///@}
};

// ================================================================================================
template<typename IV, typename DV>
class UT_EXPORT UtTable::IrregularTable::IR_Table : public UtTable::Table
{
public:
   using SharedDataPtr = typename UtTable::IrregularTable::SharedData<IV, DV>;

   IR_Table(std::shared_ptr<SharedDataPtr> aSharedDataPtr)
      : mSharedDataPtr(std::move(aSharedDataPtr))
      , mIV_DataIndex(mSharedDataPtr->mLevels)
   {
   }

   Table* Clone() const override { return new IR_Table(*this); }

   bool ResolveReferences(const Parameters& aParameters) override
   {
      bool     ok = true;
      DataType dataType;
      for (size_t i = 0; i < mSharedDataPtr->mLevels; ++i)
      {
         if (!aParameters.GetTypeAndIndex(mSharedDataPtr->mIV_Names[i], dataType, mIV_DataIndex[i]))
         {
            // This *SHOULDN'T* happen because this only published IV names are accepted during input.
            std::string ivName = mSharedDataPtr->mIV_Names[i];
            auto        out    = ut::log::error() << "Unable to resolve independent variable.";
            out.AddNote() << "Variable: " << ivName;
            ok = false;
         }
      }
      return ok;
   }

   double Lookup(const double* aArguments) const override
   {
      //! Current independent variable values, indexed by level number.
      //! uses alloca to declare variable length array on stack instead of using std::vector
      auto IV_Values = static_cast<double*>(alloca(mIV_DataIndex.size() * sizeof(double)));

      for (size_t i = 0; i < mIV_DataIndex.size(); ++i)
      {
         IV_Values[i] = aArguments[mIV_DataIndex[i]];
      }
      return mSharedDataPtr->Lookup(IV_Values);
   }

   bool VariableExists(const std::string& aName) const override
   {
      bool exists = false;
      for (size_t i = 0; (i < mSharedDataPtr->mLevels) && (!exists); ++i)
      {
         exists = (aName == mSharedDataPtr->mIV_Names[i]);
      }
      return exists;
   }

   void GetVariableNames(std::vector<std::string>& aNames) const override
   {
      aNames.clear();
      aNames.resize(mSharedDataPtr->mLevels);
      for (size_t i = 0; i < mSharedDataPtr->mLevels; ++i)
      {
         aNames[i] = mSharedDataPtr->mIV_Names[i];
      }
   }

   bool GetVariableLimits(const std::string& /*aName*/, double& /*aMinValue*/, double& /*aMaxValue*/) const override
   {
      return false; // This function is not currently supported;
   }

   bool GetVariable(const std::string& /*aName*/, std::vector<double>& /*aValues*/) const override
   {
      return false; // This function is not currently supported;
   }

   std::shared_ptr<SharedDataPtr> mSharedDataPtr;

   //! The real argument index, indexed by level number.
   std::vector<size_t> mIV_DataIndex;
};

// ================================================================================================
std::unique_ptr<UtTable::Table> UtTable::IrregularTable::LoadInstance(UtInput&           aInput,
                                                                      UtInput::ValueType aDV_UnitType,
                                                                      const ValueCheck&  aDV_ValueCheck,
                                                                      const Parameters&  aParameters)
{
   UtInputBlock inputBlock(aInput);

   std::unique_ptr<Table> tablePtr = nullptr;
   std::vector<IV_Type>   ivTypeList;
   std::string            dvUnits;
   bool                   ivDouble = true;
   bool                   dvDouble = true;

   std::string command;
   while (inputBlock.ReadCommand(command))
   {
      if (command == "independent_variable")
      {
         if (tablePtr != nullptr)
         {
            throw UtInput::OutOfContext(aInput, "independent_variable's must appear before table definition");
         }

         IV_Type ivType;

         // Read the variable name and make sure it is a name known by the caller.
         aInput.ReadValue(ivType.mName);

         DataType dataType;
         size_t   dataIndex;
         if (!aParameters.GetAttributes(ivType.mName, dataType, ivType.mUnitType, ivType.mValueCheck, dataIndex))
         {
            throw UtInput::BadValue(aInput, "'" + ivType.mName + "' is not a known independent variable");
         }

         while (inputBlock.ReadCommand(command))
         {
            if (command == "units")
            {
               ReadUnits(aInput, ivType.mUnitType, ivType.mUnits);
            }
            else if (command == "precision")
            {
               ReadPrecision(aInput, ivDouble);
            }
            else if (command == "extrapolate")
            {
               ivType.mAllowExtrapolation = true;
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
         if ((ivType.mUnitType != UtInput::cNON_DIMENSIONAL) && ivType.mUnits.empty())
         {
            throw UtInput::BadValue(aInput, "independent_variable " + ivType.mName + " requires units definition");
         }
         ivTypeList.push_back(ivType);
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
         if ((aDV_UnitType != UtInput::cNON_DIMENSIONAL) && dvUnits.empty())
         {
            throw UtInput::BadValue(aInput, "dependent_variable requires units definition");
         }
      }
      else if ((!ivTypeList.empty()) && (command == ivTypeList[0].mName))
      {
         // Start of table definition.
         if (ivTypeList.size() < 2)
         {
            throw UtInput::BadValue(aInput, "At least two independent_variables must be defined for an irregular_table");
         }

         aInput.PushBack(command);
         if (ivDouble && dvDouble) // IV's and DV's are both double
         {
            tablePtr = LoadTableValues<double, double>(inputBlock, aDV_UnitType, dvUnits, aDV_ValueCheck, ivTypeList);
         }
         else if (ivDouble && (!dvDouble)) // IV's are double, DV's are float
         {
            tablePtr = LoadTableValues<double, float>(inputBlock, aDV_UnitType, dvUnits, aDV_ValueCheck, ivTypeList);
         }
         else if ((!ivDouble) && dvDouble) // IV's are float, DV's are double
         {
            tablePtr = LoadTableValues<float, double>(inputBlock, aDV_UnitType, dvUnits, aDV_ValueCheck, ivTypeList);
         }
         else // IV's and DV's are both float
         {
            tablePtr = LoadTableValues<float, float>(inputBlock, aDV_UnitType, dvUnits, aDV_ValueCheck, ivTypeList);
         }
         // This should have read to the end of the block...
         break;
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }

   if (tablePtr == nullptr)
   {
      throw UtInput::BadValue(aInput, "No table definition was entered");
   }
   return tablePtr;
}

// ================================================================================================
//! Load the table values.
template<typename IV, typename DV>
std::unique_ptr<UtTable::Table> UtTable::IrregularTable::LoadTableValues(UtInputBlock&      aInputBlock,
                                                                         UtInput::ValueType aDV_UnitType,
                                                                         const std::string& aDV_Units,
                                                                         const ValueCheck&  aDV_ValueCheck,
                                                                         const IV_TypeList& aIV_TypeList)
{
   using IndexList = typename std::list<UtTable::IrregularTable::IndexEntry<IV>>;

   // The stack of index levels being built. These are moved to the composite as they are completed.
   std::stack<IndexList> indexStack;

   // The composite index.
   std::list<IndexEntry<IV>> index;

   // The independent and dependent values for the final dimension.
   std::vector<IV> leafIV_Values;
   std::vector<DV> leafDV_Values;

   size_t maxLevel = aIV_TypeList.size() - 1;

   // Start at the root level
   size_t curLevel = 0;
   indexStack.push(IndexList());

   std::string ivName;
   size_t      dvCount    = 0;
   bool        readingIVs = false;
   bool        readingDVs = false;

   UtInput& input = aInputBlock.GetInput();
   while (aInputBlock.ReadCommand(ivName))
   {
      if (readingIVs)
      {
         // Reading the independent values for the last dimension
         if (ivName == "values")
         {
            // End of independent variable values for this dimension
            // Start reading dependent variables.
            readingIVs = false;
            readingDVs = true;
            if (dvCount < 2)
            {
               throw UtInput::BadValue(input, "At least two values must be specified");
            }
            indexStack.top().back().mSize = dvCount;
         }
         else
         {
            // Add another independent variable value.
            input.PushBack(ivName);
            const IV_Type& ivType  = aIV_TypeList[curLevel];
            double         ivValue = ReadValue(input, ivType.mUnitType, ivType.mUnits, ivType.mValueCheck);
            if ((dvCount != 0) && (static_cast<IV>(ivValue) <= leafIV_Values.back()))
            {
               throw UtInput::BadValue(input, "independent variable values must be monotonically ascending");
            }
            leafIV_Values.push_back(static_cast<IV>(ivValue));
            ++dvCount;
         }
      }
      else if (readingDVs)
      {
         // Reading the dependent values for the last dimension.
         input.PushBack(ivName);
         double dvValue = ReadValue(input, aDV_UnitType, aDV_Units, aDV_ValueCheck);
         leafDV_Values.push_back(static_cast<DV>(dvValue));

         // If all of the values have been read, pop up.
         --dvCount;
         if (dvCount == 0)
         {
            readingDVs = false;
            --curLevel;
         }
      }
      else if (ivName == "constant")
      {
         // A 'constant' value can be specified for any level except the first level.
         // The constant must be of the type indicated by the dependent variable.

         if (curLevel == 0)
         {
            throw UtInput::UnknownCommand(input);
         }

         size_t leafOffset = leafIV_Values.size();
         // Push a dummy IV value so there are an equal number of IV/DV entries.
         leafIV_Values.push_back(0.0);
         double dvValue = ReadValue(input, aDV_UnitType, aDV_Units, aDV_ValueCheck);
         leafDV_Values.push_back(static_cast<DV>(dvValue));

         if (curLevel != maxLevel)
         {
            indexStack.pop();
         }

         --curLevel;
         indexStack.top().back().mOffset = leafOffset;
         indexStack.top().back().mSize   = 0; // A special indicator indicating a constant
      }
      else if ((curLevel < maxLevel) && (ivName == aIV_TypeList[curLevel].mName))
      {
         // Found the expected introduction of the next level that is not the final level

         const IV_Type& ivType  = aIV_TypeList[curLevel];
         double         ivValue = ReadValue(input, ivType.mUnitType, ivType.mUnits, ivType.mValueCheck);
         if ((!indexStack.top().empty()) && (ivValue <= indexStack.top().back().mValue))
         {
            throw UtInput::BadValue(input, "independent variable values must be monotonically ascending");
         }
         indexStack.top().push_back(IndexEntry<IV>(static_cast<IV>(ivValue)));

         // Now expecting the next level. If the next level is not the final level then start a new
         // index level. (We don't need an index for the final level).
         ++curLevel;
         if (curLevel != maxLevel)
         {
            indexStack.push(IndexList());
         }
      }
      else if ((curLevel == maxLevel) && (ivName == aIV_TypeList[curLevel].mName))
      {
         // Found the start of the final level
         readingIVs = true;
         // Record the index of the first entry.
         indexStack.top().back().mOffset = leafIV_Values.size();
         dvCount                         = 0;
      }
      else
      {
         // The current value should be the name of a higher level variable.
         //
         // We could be backing up several levels, so see which level we are poping to and
         // complete any intermediate levels.
         while (ivName != aIV_TypeList[curLevel].mName)
         {
            if (curLevel == 0) // Cannot backup from the root level
            {
               throw UtInput::UnknownCommand(input);
            }

            // Move the complete current level to the final index.
            IndexList& curIndex = indexStack.top();
            size_t     offset   = index.size();
            size_t     size     = indexStack.top().size();
            if (size < 2)
            {
               throw UtInput::BadValue(input, "There must be at least two values for a level");
            }
            index.splice(index.end(), curIndex, curIndex.begin(), curIndex.end());
            indexStack.pop();

            // Update the index level in this level to point to the completed index.
            indexStack.top().back().mOffset = offset;
            indexStack.top().back().mSize   = size;

            --curLevel;
         }
         input.PushBack(ivName);
      }
   }

   if (readingIVs || readingDVs)
   {
      throw UtInput::BadValue(input, "Unexpected end-of-table");
   }

   // Finish any open levels
   while (curLevel != 0)
   {
      // Move the complete current level to the final index.
      IndexList& curIndex = indexStack.top();
      size_t     offset   = index.size();
      size_t     size     = indexStack.top().size();
      if (size < 2)
      {
         throw UtInput::BadValue(input, "There must be at least two values for a level");
      }
      index.splice(index.end(), curIndex, curIndex.begin(), curIndex.end());
      indexStack.pop();

      // Update the index level in this level to point to the completed index.
      indexStack.top().back().mOffset = offset;
      indexStack.top().back().mSize   = size;

      --curLevel;
   }

   // And finally move the root index onto the final index
   IndexList& rootIndex  = indexStack.top();
   size_t     rootOffset = index.size();
   size_t     rootSize   = rootIndex.size();
   index.splice(index.end(), rootIndex, rootIndex.begin(), rootIndex.end());

   // Build the class that contains the sharable data....

   auto dataPtr = ut::make_unique<SharedData<IV, DV>>();

   dataPtr->mLevels = aIV_TypeList.size();
   dataPtr->mIV_Names.resize(aIV_TypeList.size());
   dataPtr->mLU_Methods.resize(aIV_TypeList.size());
   dataPtr->mPreviousLookupIndex.resize(aIV_TypeList.size(), 0U);
   for (size_t i = 0; i < aIV_TypeList.size(); ++i)
   {
      dataPtr->mIV_Names[i]   = aIV_TypeList[i].mName;
      dataPtr->mLU_Methods[i] = cLM_LU;
      if (aIV_TypeList[i].mAllowExtrapolation)
      {
         dataPtr->mLU_Methods[i] = cLM_LUX;
      }
   }

   dataPtr->mRootOffset = rootOffset;
   dataPtr->mRootSize   = rootSize;
   dataPtr->mIndexValues.resize(index.size());
   dataPtr->mIndexOffset.resize(index.size());
   dataPtr->mIndexSize.resize(index.size());
   size_t ai = 0;
   for (const auto& indexEntry : index)
   {
      (dataPtr->mIndexValues)[ai] = indexEntry.mValue;
      (dataPtr->mIndexOffset)[ai] = indexEntry.mOffset;
      (dataPtr->mIndexSize)[ai]   = indexEntry.mSize;
      ++ai;
   }

   // Copy the leaf values from the local vector to the resulting array.

   size_t leafLevelSize = leafDV_Values.size();
   dataPtr->mLeafIV_Values.resize(leafLevelSize);
   dataPtr->mLeafDV_Values.resize(leafLevelSize);
   for (size_t i = 0; i < leafLevelSize; ++i)
   {
      (dataPtr->mLeafIV_Values)[i] = leafIV_Values[i];
      (dataPtr->mLeafDV_Values)[i] = leafDV_Values[i];
   }

   return ut::make_unique<IR_Table<IV, DV>>(std::move(dataPtr));
}
