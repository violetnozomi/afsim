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

#ifndef WSFEVENTPIPEOPTIONS_HPP
#define WSFEVENTPIPEOPTIONS_HPP

#include <map>
#include <string>
#include <vector>

class UtInput;

// Stores an enable bit for each event category
class WsfEventPipeOptions
{
public:
   WsfEventPipeOptions() = default;

   void SetMaxEventId(int aMaxEventId) { mBoolOptions.resize(aMaxEventId + 1); }

   bool ProcessInput(UtInput& aInput, const std::vector<std::string>& aEventNames);

   void Add(const WsfEventPipeOptions& aOtherOptions)
   {
      if (mBoolOptions.size() < aOtherOptions.mBoolOptions.size())
      {
         mBoolOptions.resize(aOtherOptions.mBoolOptions.size());
      }
      for (size_t i = 0; i < mBoolOptions.size(); ++i)
      {
         mBoolOptions[i] = mBoolOptions[i] || aOtherOptions.mBoolOptions[i];
      }
   }

   void Subtract(const WsfEventPipeOptions& aOtherOptions)
   {
      if (mBoolOptions.size() < aOtherOptions.mBoolOptions.size())
      {
         mBoolOptions.resize(aOtherOptions.mBoolOptions.size());
      }
      for (size_t i = 0; i < mBoolOptions.size(); ++i)
      {
         mBoolOptions[i] = mBoolOptions[i] && aOtherOptions.mBoolOptions[i];
      }
   }

   void EnableAll();
   void DisableAll();

   // for options registered through RegisterBoolOption()
   bool IsOptionEnabled(int aOptionIndex) const { return mBoolOptions[aOptionIndex]; }

   void EnableOption(int aOptionIndex, bool aEnable = true) { mBoolOptions[aOptionIndex] = aEnable; }

   // returns the update interval that should be applied to visual parts.
   // 0 indicates parts' articulations should not be published except initially
   double GetVisualPartUpdateInterval() const { return mVisualPartUpdateInterval; }
   void   SetVisualPartUpdateInterval(double aValue) { mVisualPartUpdateInterval = aValue; }

   bool operator!=(const WsfEventPipeOptions& aRhs) const { return mBoolOptions != aRhs.mBoolOptions; }

   std::map<std::string, bool> GetOptionValues(const std::vector<std::string>& aEventNames) const;

private:
   std::vector<bool> mBoolOptions;

   // the interval on which the sim puts out articulation updates for visual parts
   // defaults to 0.0, which will not publish visual-part articulations.
   double mVisualPartUpdateInterval{0.0};
};

#endif
