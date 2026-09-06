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

#ifndef UTDATALIB_HPP
#define UTDATALIB_HPP

#include "ut_export.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#include "UtCallback.hpp"
class UtInput;
class UtInputBlock;

/**
    A lightweight publish-subscribe for data in an object instance.
    This object is the place to publish availability of an independent variable
    that may be needed for table lookups, controls, etc.  It allows for only one
    entity to own and update the variable, but any number of clients/listeners
    to use the value.  The owner may change if another owner decides to re-publish
    the variable; the first publisher will unknowingly lose ability to set the
    published parameter.

    Simulation Controller call order:
       ProcessInput(), Initialize(), Initialize2(), <<many>> Update()s.

    The class allows for tailorable tabulated data-file creation, highlighted below:
    Logging only occurs upon a call to Update() command.  Options:  Logging Enables/
    /Disables upon command.  Values (Columns) Options = [ None, All, or Selected
    by Name ].  Logging (optionally) disabled for a pause time after Initialize(),
    and once it commences, may (optionally) be terminated after log duration expires.

    Enable hierarchy:  1) Stream is open.  2) Logging flag Enabled.
    3) mColumnsFound = true.  4.) IntervalEnable = true.  5.  FrameEnable = true.
*/

class UT_EXPORT UtDataLib
{
public:
   using Key    = std::pair<int, const double*>;
   using KeySet = std::vector<Key>;

   enum ColumnsMode
   {
      cNONE,
      cALL,
      cSELECTED
   };

   class Item
   {
   public:
      // Item();
      Item(void* aProducerPtr, const double* aValuePtr);

      bool operator<(const Item& aRhs) const { return mProducerPtr < aRhs.mProducerPtr; }

      void*         mProducerPtr; // The producer can be ANY entity pointer
      const double* mValuePtr;    // This is the value the client receives.
      bool          mSelected;    // A flag to indicate that the item is "selected" for output.
   };

   using ItemMap = std::map<int, Item>;

   UtDataLib();
   UtDataLib(const UtDataLib& aSrc);
   virtual ~UtDataLib();

   // ============ Simulation Controller Interface ==================

   void ProcessInputBlock(UtInputBlock& aInputBlock);
   bool ProcessInput(UtInput& aInput);

   // This function will open the stream for output.
   // The output file name will be the concatenation of aPrefix, mCaseName, aSuffix.
   bool Initialize(double aSimTime, const std::string& aOutputFilePrefix = "");

   bool Initialize2(double aSimeTime);

   void Update(double aSimTime);

   // Direct control and/or scriptable accessors.
   void SetDebug(bool aValue) { mDebug = aValue; }
   void SetFileEnable(bool aValue) { mFileEnable = aValue; }
   void SetLogEnable(bool aValue) { mLogIsEnabled = aValue; }
   void SetLogPause(double aValue) { mPauseDuration = aValue; }
   void SetLogDuration(double aValue) { mLoggingDuration = aValue; }
   void SetLogFrameTime(double aValue) { mLoggingFrame = aValue; }

   std::string IntToString(int aNameId);
   int         StringToInt(const std::string& aName);

   // ============= Publisher-Subscriber Interface ==================
   bool Publish(void* aProducerPtr, int aNameId, const double* aValuePtr);

   bool Publish(void* aProducerPtr, const KeySet& aKeySet);

   //! No named path variable substitution is performed here.
   void SetCaseName(const std::string& aCaseName);

   bool Subscribe(int aNameId, const double*& aValuePtrRef, bool aReport = true);

   // ============================================================

protected:
   void AddSelect(int aNameId);
   void WriteHeader();
   bool SelectColumns();
   bool ColumnsEnabled();
   bool FrameEnable(double aSimTime) const;
   bool IntervalEnable(double aSimTime) const;
   void WriteRecord(double aSimTime);
   bool ItemSelected(int aNameId) const;
   void SaveLogList(const std::string& aLogListName);

private:
   bool mDebug;
   bool mFileEnable;
   // This flag may allow for scripted turn-on, turn-off of data logging.
   bool mLogIsEnabled;
   bool mColumnsFound;
   bool mSaveLogList;

   ColumnsMode mColumnsMode;

   double mInitTime;
   double mPauseDuration;
   double mLoggingDuration;
   double mLoggingFrame;
   double mLastLogTime;

   std::string      mCaseName; // optional
   std::string      mOutputSuffix;
   std::ofstream    mOutStream;
   std::vector<int> mSelectedOutIds;

   ItemMap mItems;
};

#endif
