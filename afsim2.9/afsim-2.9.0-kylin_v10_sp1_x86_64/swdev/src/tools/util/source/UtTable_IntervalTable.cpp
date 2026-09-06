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

#include "UtTable_IntervalTable.hpp"

#include <iostream>
#include <list>
#include <memory>
#include <stack>
#include <vector>

#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"

// Forward declarations for things defined in this file
namespace UtTable
{
namespace IntervalTable
{
template<typename IV, typename DV>
class IntTable;

template<typename IV, typename DV>
class SharedData;

enum LookupMethod
{
   cLM_LU,
   cLM_EXACT
};

struct IV_Type
{
   std::string        mName;
   UtInput::ValueType mUnitType{UtInput::cNON_DIMENSIONAL};
   std::string        mUnits;
   ValueCheck         mValueCheck;
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
} // namespace IntervalTable
} // namespace UtTable

// ================================================================================================
template<typename IV, typename DV>
class UT_EXPORT UtTable::IntervalTable::SharedData
{
public:
   size_t FindInterval(double aValue, const IV aTable[], size_t aSize) const
   {
      for (size_t i = 0; (i + 1) < aSize; ++i)
      {
         if (aValue < aTable[i + 1])
         {
            return i;
         }
      }
      return aSize - 1; // Value is bigger than the last interval endpoint.
   }

   double Lookup(double aIV_Values[]) const
   {
      size_t level  = 0;
      size_t offset = mRootOffset;
      size_t size   = mRootSize;
      double value  = 0.0;
      while (level < mLevels)
      {
         if (size == 0)
         {
            value = mLeafDV_Values[offset];
            break;
         }
         else if ((level + 1) < mLevels)
         {
            size_t nextOffset = offset + FindInterval(aIV_Values[level], mIndexValues.data() + offset, size);
            offset            = mIndexOffset[nextOffset];
            size              = mIndexSize[nextOffset];
         }
         else
         {
            size_t leafOffset = offset + FindInterval(aIV_Values[level], mLeafIV_Values.data() + offset, size);
            value             = mLeafDV_Values[leafOffset];
            break;
         }
         ++level;
      }
      return value;
   }

   std::vector<std::string>  mIV_Names;
   std::vector<LookupMethod> mLU_Methods;

   size_t mLevels{0};

   size_t mRootOffset{0};
   size_t mRootSize{0};

   //! @name Index structure for the non-leaf levels
   //@{
   std::vector<IV>     mIndexValues;
   std::vector<size_t> mIndexOffset;
   std::vector<size_t> mIndexSize;
   //@}

   //! @name values for the leaf level
   //@{
   std::vector<IV> mLeafIV_Values;
   std::vector<DV> mLeafDV_Values;
   //@}
};

// ================================================================================================
template<typename IV, typename DV>
class UT_EXPORT UtTable::IntervalTable::IntTable : public UtTable::Table
{
public:
   using SharedDataPtr = typename UtTable::IntervalTable::SharedData<IV, DV>;

   IntTable(std::shared_ptr<SharedDataPtr> aSharedDataPtr)
      : mSharedDataPtr(std::move(aSharedDataPtr))
      , mIV_DataIndex(aSharedDataPtr->mLevels)
   {
   }

   Table* Clone() const override { return new IntTable(*this); }

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
      auto IV_Values = static_cast<double*>(alloca(mSharedDataPtr->mLevels * sizeof(double)));

      for (size_t i = 0; i < mSharedDataPtr->mLevels; ++i)
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
std::unique_ptr<UtTable::Table> UtTable::IntervalTable::LoadInstance(UtInput&           aInput,
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
std::unique_ptr<UtTable::Table> UtTable::IntervalTable::LoadTableValues(UtInputBlock&      aInputBlock,
                                                                        UtInput::ValueType aDV_UnitType,
                                                                        const std::string& aDV_Units,
                                                                        const ValueCheck&  aDV_ValueCheck,
                                                                        const IV_TypeList& aIV_TypeList)
{
   using IndexList = typename std::list<UtTable::IntervalTable::IndexEntry<IV>>;

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

         const IV_Type& ivType = aIV_TypeList[curLevel];
         auto ivValue          = static_cast<IV>(ReadValue(input, ivType.mUnitType, ivType.mUnits, ivType.mValueCheck));
         if ((!indexStack.top().empty()) && (ivValue <= indexStack.top().back().mValue))
         {
            throw UtInput::BadValue(input, "independent variable values must be monotonically ascending");
         }
         indexStack.top().emplace_back(ivValue);

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
         // We could be backing up several levels, so see which level we are popping to and
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
   for (size_t i = 0; i < aIV_TypeList.size(); ++i)
   {
      dataPtr->mIV_Names[i]   = aIV_TypeList[i].mName;
      dataPtr->mLU_Methods[i] = cLM_LU;
   }

   dataPtr->mRootOffset = rootOffset;
   dataPtr->mRootSize   = rootSize;

   size_t indexSize = index.size();
   dataPtr->mIndexValues.resize(indexSize);
   dataPtr->mIndexOffset.resize(indexSize);
   dataPtr->mIndexSize.resize(indexSize);
   size_t ai = 0;
   for (const auto& indexEntry : index)
   {
      (dataPtr->mIndexValues)[ai] = indexEntry.mValue;
      (dataPtr->mIndexOffset)[ai] = indexEntry.mOffset;
      (dataPtr->mIndexSize)[ai]   = indexEntry.mSize;
      ++ai;
   }

   // Copy the leaf values from the local vector to the resulting array.

   dataPtr->mLeafIV_Values = leafIV_Values;
   dataPtr->mLeafIV_Values.shrink_to_fit();
   dataPtr->mLeafDV_Values = leafDV_Values;
   dataPtr->mLeafDV_Values.shrink_to_fit();

   return ut::make_unique<IntTable<IV, DV>>(std::move(dataPtr));
}
