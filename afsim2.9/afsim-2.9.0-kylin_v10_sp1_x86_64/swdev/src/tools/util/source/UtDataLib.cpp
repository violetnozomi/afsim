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
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "UtDataLib.hpp"

#include <cfloat>

#include "UtDictionary.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"

// ============================================================================
UtDataLib::UtDataLib()
   : mDebug(false)
   , mFileEnable(false)
   , mLogIsEnabled(true)
   , mColumnsFound(false)
   , mSaveLogList(false)
   , mColumnsMode(cALL)
   , mInitTime(0.0)
   , mPauseDuration(0.0)
   , mLoggingDuration(DBL_MAX / 2.0)
   , mLoggingFrame(0.00000001)
   , mLastLogTime(-DBL_MAX)
   , mCaseName("")
   , mOutputSuffix(".csv")
   , mOutStream()
   , mSelectedOutIds()
   , mItems()
{
}

// ============================================================================
UtDataLib::UtDataLib(const UtDataLib& aSrc)
   : mDebug(aSrc.mDebug)
   , mFileEnable(aSrc.mFileEnable)
   , mLogIsEnabled(aSrc.mLogIsEnabled)
   , mColumnsFound(aSrc.mColumnsFound)
   , mSaveLogList(aSrc.mSaveLogList)
   , mColumnsMode(aSrc.mColumnsMode)
   , mInitTime(aSrc.mInitTime)
   , mPauseDuration(aSrc.mPauseDuration)
   , mLoggingDuration(aSrc.mLoggingDuration)
   , mLoggingFrame(aSrc.mLoggingFrame)
   , mLastLogTime(aSrc.mLastLogTime)
   , mCaseName(aSrc.mCaseName)
   , mOutputSuffix(aSrc.mOutputSuffix)
   , mOutStream()
   , mSelectedOutIds(aSrc.mSelectedOutIds)
   , mItems()
{
}

// ============================================================================
UtDataLib::~UtDataLib()
{
   mOutStream.close();
}

// ============================================================================
void UtDataLib::SetCaseName(const std::string& aCaseName)
{
   mCaseName = aCaseName;
}

// ============================================================================
void UtDataLib::ProcessInputBlock(UtInputBlock& aInputBlock)
{
   while (aInputBlock.ReadCommand())
   {
      UtInput& input = aInputBlock.GetInput();
      if (!ProcessInput(input))
      {
         throw UtInput::UnknownCommand(input);
      }
   }
}

// ============================================================================
bool UtDataLib::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   if (command == "debug")
   {
      SetDebug(true);
   }
   else if (command == "file_enable")
   {
      aInput.ReadValue(mFileEnable);
   }
   else if (command == "create_log_list")
   {
      aInput.ReadValue(mSaveLogList);
   }
   else if (command == "file_disable")
   {
      mFileEnable = false;
   }
   else if (command == "log_enable")
   {
      mLogIsEnabled = true;
   }
   else if (command == "log_disable")
   {
      mLogIsEnabled = false;
   }
   else if (command == "log_all")
   {
      mColumnsMode = cALL;
   }
   else if (command == "log_none")
   {
      mColumnsMode = cNONE;
   }
   else if (command == "log_selected")
   {
      mColumnsMode = cSELECTED;
   }
   else if (command == "log")
   {
      std::string name;
      aInput.ReadValue(name);
      int id = StringToInt(name);
      AddSelect(id);
   }
   else if (command == "pause_duration")
   {
      aInput.ReadValueOfType(mPauseDuration, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mPauseDuration, 0.0);
   }
   else if (command == "log_duration")
   {
      aInput.ReadValueOfType(mLoggingDuration, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mLoggingDuration, 0.0);
   }
   else if (command == "log_frame")
   {
      aInput.ReadValueOfType(mLoggingFrame, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mLoggingFrame, 0.0);
   }
   else if (command == "case_name")
   {
      std::string rawName;
      aInput.ReadValue(rawName);
      mCaseName = aInput.SubstitutePathVariables(rawName);
   }
   else if (command == "output_file_suffix")
   {
      aInput.ReadValue(mOutputSuffix);
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// ============================================================================
bool UtDataLib::Publish(void* aProducerPtr, int aNameId, const double* aValuePtr)
{
   auto iter(mItems.find(aNameId));
   if (iter != mItems.end())
   {
      // We do have one already , but it is replaceable... change its source.
      if (mDebug)
      {
         auto out = ut::log::debug() << "Library modified existing data item to change the producer.";
         out.AddNote() << "Item: " << IntToString(aNameId);
      }

      iter->second.mProducerPtr = aProducerPtr;
      iter->second.mValuePtr    = aValuePtr;
      return true;
   }

   ItemMap::value_type                newVal(aNameId, Item(aProducerPtr, aValuePtr));
   std::pair<ItemMap::iterator, bool> result = mItems.insert(newVal);
   if (mDebug && result.second)
   {
      const double* ptr = nullptr;
      Subscribe(aNameId, ptr, false);
      auto out = ut::log::debug() << "DataLib added:";
      out.AddNote() << "Name ID#: " << aNameId;
      out.AddNote() << "Name: " << IntToString(aNameId);
      out.AddNote() << "Value: " << *aValuePtr;
      out.AddNote() << "Producer: " << aProducerPtr;
      out.AddNote() << "Address: " << ptr;
   }
   else if (mDebug)
   {
      auto out = ut::log::debug() << "DataLib did not add:";
      out.AddNote() << "Name: " << IntToString(aNameId);
      out.AddNote() << "Value: " << *aValuePtr;
      out.AddNote() << "Producer: " << aProducerPtr;
   }

   return result.second;
}

// ============================================================================
bool UtDataLib::Publish(void* aProducerPtr, const KeySet& aKeySet)
{
   bool success = true;
   for (size_t i = 0; i != aKeySet.size(); ++i)
   {
      if (!Publish(aProducerPtr, aKeySet[i].first, aKeySet[i].second))
      {
         success = false;
      }
   }
   return success;
}

// ============================================================================
bool UtDataLib::Subscribe(int aNameId, const double*& aPtrToValue, bool aReport)
{
   auto iter = mItems.find(aNameId);
   if (iter == mItems.end())
   {
      return false;
   }
   else
   {
      aPtrToValue = iter->second.mValuePtr;
      if (mDebug && aReport)
      {
         auto out = ut::log::debug() << "DataLib gave ptr:";
         out.AddNote() << "Ptr:" << iter->first;
         out.AddNote() << "Value: " << *(iter->second.mValuePtr);
         out.AddNote() << "Producer: " << iter->second.mProducerPtr;
         out.AddNote() << "Address: " << aPtrToValue;
      }
      return true;
   }
}

// ============================================================================
// This function will open the stream for output.
bool UtDataLib::Initialize(double aSimTime, const std::string& aOutputFilePrefix)
{
   if (mFileEnable)
   {
      std::string separator = "";
      if ((!aOutputFilePrefix.empty()) && (!mCaseName.empty()))
      {
         separator = "_";
      }

      std::string actualName = aOutputFilePrefix + separator + mCaseName + mOutputSuffix;
      if (!actualName.empty())
      {
         mOutStream.open(actualName.c_str());
      }
   }

   mInitTime = aSimTime;

   return true;
}

// ============================================================================
// This function will open the stream for output.
void UtDataLib::SaveLogList(const std::string& aLogListName)
{
   std::ofstream theStream;
   theStream.open(aLogListName.c_str());
   if (theStream)
   {
      UtDictionary& dict = UtDictionary::GetDictionary();
      for (const auto& item : mItems)
      {
         theStream << "   log " << dict.GetString(item.first) << std::endl;
      }
      theStream.close();
   }
}

// ============================================================================
// This function will open the stream for output.
bool UtDataLib::Initialize2(double aSimTime)
{
   if (mOutStream)
   {
      if (mColumnsMode == cSELECTED)
      {
         mColumnsFound = SelectColumns();
      }
      else
      {
         mColumnsFound = !mItems.empty();
      }

      WriteHeader();
   }

   if (mSaveLogList)
   {
      std::string logListName = "data_log_parameter_list.log";
      SaveLogList(logListName);
   }

   Update(aSimTime);

   return true;
}

// ============================================================================
bool UtDataLib::ColumnsEnabled()
{
   return mColumnsFound || mColumnsMode == cALL;
}

// ============================================================================
void UtDataLib::Update(double aSimTime)
{
   // Logging controls heirarchy:  1) Stream is open.  2) Logging flag Enabled.
   // 3) mColumnsFound = true.  4.) IntervalEnable = true.  5.  FrameEnable = true.

   // For debug use only...
   // bool outst = mOutStream.is_open();
   // bool logEn = mLogIsEnabled;
   // bool cols = ColumnsEnabled();
   // bool interval = IntervalEnable(aSimTime);
   // bool frame = FrameEnable(aSimTime);

   if (mOutStream.is_open() && mLogIsEnabled && mColumnsFound && IntervalEnable(aSimTime) && FrameEnable(aSimTime))
   {
      WriteRecord(aSimTime);
   }
}

// ============================================================================
bool UtDataLib::FrameEnable(double aSimTime) const
{
   return aSimTime >= (mLastLogTime + mLoggingFrame);
}

// ============================================================================
bool UtDataLib::IntervalEnable(double aSimTime) const
{
   return ((aSimTime >= mInitTime + mPauseDuration) && (aSimTime <= mInitTime + mPauseDuration + mLoggingDuration));
}

// ============================================================================
void UtDataLib::WriteHeader()
{
   UtDictionary& dict = UtDictionary::GetDictionary();

   // Just for information to the user, print out all the available columns:
   mOutStream << "# Listing of all published items:  ";
   for (const auto& item : mItems)
   {
      mOutStream << " " << dict.GetString(item.first);
   }
   mOutStream << std::endl << std::endl;

   // Now print the comma-delimited header row:
   mOutStream << "sim_time";
   bool writeAll = (mColumnsMode == cALL);
   for (const auto& item : mItems)
   {
      if (writeAll || item.second.mSelected)
      {
         mOutStream << ", " << dict.GetString(item.first);
      }
   }
   mOutStream << std::endl;
}

// ============================================================================
bool UtDataLib::SelectColumns()
{
   bool gotOne = false;
   // If an "item" is in the vector of "log" name items, set its "selected flag.
   for (auto& item : mItems)
   {
      if (ItemSelected(item.first))
      {
         item.second.mSelected = true;
         gotOne                = true;
      }
   }
   return gotOne;
}

// ============================================================================
void UtDataLib::WriteRecord(double aSimTime)
{
   mOutStream << aSimTime;
   bool writeAll = (mColumnsMode == cALL);
   for (const auto& item : mItems)
   {
      if (writeAll || item.second.mSelected)
      {
         mOutStream << ", " << *(item.second.mValuePtr);
      }
   }
   mOutStream << std::endl;
   mLastLogTime = aSimTime;
}

// ============================================================================
void UtDataLib::AddSelect(int aNameId)
{
   for (int id : mSelectedOutIds)
   {
      if (id == aNameId)
         return;
   }
   mSelectedOutIds.push_back(aNameId);
}

// ============================================================================
bool UtDataLib::ItemSelected(int aNameId) const
{
   for (int id : mSelectedOutIds)
   {
      if (id == aNameId)
      {
         return true;
      }
   }
   return false;
}

// ============================================================================
// ====================================== sub-class Item ======================
/*UtDataLib::Item::Item()
  : mProducerPtr(0),
    mValuePtr(0),
    mSelected(false)
{
}*/

// ============================================================================
UtDataLib::Item::Item(void* aProducerPtr, const double* aValuePtr)
   : mProducerPtr(aProducerPtr)
   , mValuePtr(aValuePtr)
   , mSelected(false)
{
}

// ============================================================================
std::string UtDataLib::IntToString(int aNameId)
{
   return UtDictionary::GetDictionary().GetString(aNameId);
}

// ============================================================================
int UtDataLib::StringToInt(const std::string& aName)
{
   return UtDictionary::GetDictionary().GetNumber(aName);
}
