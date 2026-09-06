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

#include "WsfIFF_Manager.hpp"

#include <iostream>
#include <vector>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "WsfStringId.hpp"

// ****************************************************************************
// Public

// ============================================================================
// Virtual
WsfIFF_Manager::~WsfIFF_Manager() {}

// ============================================================================
//! Process the IFF block.
//!
//! @param aInput    [input]  The input stream.
//!
//! @return 'true' if the command was recognized (and processed) or 'false' if the command was not
//! one recognized by this class.
bool WsfIFF_Manager::ProcessInput(UtInput& aInput)
{
   // iff_mapping
   //    side [default] ... end_side
   // end_iff_mapping

   bool ok(false);
   if (aInput.GetCommand() == "iff_mapping")
   {
      ok = true;
      UtInputBlock block(aInput, "end_iff_mapping");
      while (block.ReadCommand())
      {
         std::string command(aInput.GetCommand());
         if (command == "side")
         {
            ProcessSide(aInput);
         }
         else if (command == "debug")
         {
            mDebugEnabled = true;
         }
         else
         {
            return false;
         }
      }
   }
   return ok;
}

// ============================================================================
WsfTrack::IFF_Status WsfIFF_Manager::GetIFF_Status(WsfStringId                          aPlaformSideId,
                                                   WsfStringId                          aTgtPlatformSideId,
                                                   const WsfCategoryList::CategoryList& aTgtCategories) const
{
   // Look for side IFF status
   WsfTrack::IFF_Status iffStatus(WsfTrack::cIFF_UNKNOWN);
   if (GetIFF_StatusP(aPlaformSideId, aTgtPlatformSideId, aTgtCategories, iffStatus))
   {
      // found
      return iffStatus;
   }

   // Look for default IFF status
   if (GetIFF_StatusP(mDefaultId, aTgtPlatformSideId, aTgtCategories, iffStatus))
   {
      // found
      return iffStatus;
   }

   // Pure side check
   if (aTgtPlatformSideId == aPlaformSideId)
   {
      return WsfTrack::cIFF_FRIEND;
   }
   else
   {
      return WsfTrack::cIFF_FOE;
   }
}

// ****************************************************************************
// Protected

// Constructor
WsfIFF_Manager::WsfIFF_Manager()
   : mIFF_SideData()
   , mIFF_CategoryData()
   , mIFF_DefaultData()
   , mDebugEnabled(false)
   , mDefaultId("__default")
{
}

// ****************************************************************************
// Private

// ============================================================================
//! Convert a string to IFF enumeration defined in WsfTrack.
//!
//! @param aIFF_Str    [input]  A string representing IFF status.
//!
//! @return 'true' if the conversion was successful
bool WsfIFF_Manager::ConvertIFF_String(const std::string& aIFF_Str, WsfTrack::IFF_Status& aIFF_Status)
{
   bool success(true);
   aIFF_Status = WsfTrack::cIFF_UNKNOWN;
   if (aIFF_Str == "foe")
   {
      aIFF_Status = WsfTrack::cIFF_FOE;
   }
   else if (aIFF_Str == "friend")
   {
      aIFF_Status = WsfTrack::cIFF_FRIEND;
   }
   else if (aIFF_Str == "neutral")
   {
      aIFF_Status = WsfTrack::cIFF_NEUTRAL;
   }
   else
   {
      success = false;
   }
   return success;
}

// ============================================================================
bool WsfIFF_Manager::GetIFF_StatusP(WsfStringId                          aPlaformSideId,
                                    WsfStringId                          aTgtPlatformSideId,
                                    const WsfCategoryList::CategoryList& aTgtCategories,
                                    WsfTrack::IFF_Status&                aIFF_Status) const
{
   // Look for side-side IFF status
   auto iter = mIFF_SideData.find(SideMapTypePair(aPlaformSideId, aTgtPlatformSideId));
   if (iter != mIFF_SideData.end())
   {
      // found
      aIFF_Status = (*iter).second;
      return true;
   }

   // Look for side-category IFF status
   for (unsigned int i = 0; i < aTgtCategories.size(); ++i)
   {
      iter = mIFF_CategoryData.find(SideMapTypePair(aPlaformSideId, aTgtCategories[i]));
      if (iter != mIFF_CategoryData.end())
      {
         // found
         aIFF_Status = (*iter).second;
         return true;
      }
   }

   // Look for side-default IFF status
   iter = mIFF_DefaultData.find(SideMapTypePair(aPlaformSideId, mDefaultId));
   if (iter != mIFF_DefaultData.end())
   {
      aIFF_Status = (*iter).second;
      return true;
   }

   return false;
}

// ============================================================================
//! Process the IFF block.
//!
//! @param aInput    [input]  The input stream.
//!
//! @return 'true' if the command was recognized (and processed) or 'false' if the command was not
//! one recognized by this class.
void WsfIFF_Manager::ProcessSide(UtInput& aInput)
{
   // side < side_value > | [side_value] | [default]
   //    reports <iff_state> for_side     <for_value>
   //    reports <iff_state> for_category <for_value>
   //    reports <iff_state> by_default
   // end_side

   // Read until end of block
   UtInputBlock             inputBlock(aInput, "end_side");
   std::vector<WsfStringId> sideStringIDs;
   while (inputBlock.ReadCommand())
   {
      std::string command(aInput.GetCommand());
      if (command == "reports")
      {
         // Read the <iff_state>
         std::string iffStateStr;
         aInput.ReadValue(iffStateStr);
         WsfTrack::IFF_Status iffStatus(WsfTrack::cIFF_UNKNOWN);
         if (!ConvertIFF_String(iffStateStr, iffStatus))
         {
            throw UtInput::BadValue(aInput, "Unknown IFF state.");
         }

         // Read the for statement
         std::string forStatementStr;
         aInput.ReadValue(forStatementStr);
         if ((forStatementStr != "for_side") && (forStatementStr != "for_category") && (forStatementStr != "by_default"))
         {
            throw UtInput::BadValue(aInput, "Invalid IFF statement.");
         }

         // Read the value
         std::string forValueStr;
         WsfStringId forValue;
         if (forStatementStr != "by_default")
         {
            aInput.ReadValue(forValueStr);
            forValue = forValueStr;
         }

         // Add to the appropriate map
         for (unsigned int i = 0; i < sideStringIDs.size(); ++i)
         {
            SideMapTypePair     sideToPair           = std::make_pair(sideStringIDs[i], forValue);
            SideIFF_MapTypePair sideToIFF_StatusPair = std::make_pair(sideToPair, iffStatus);
            if (forStatementStr == "for_side")
            {
               mIFF_SideData.insert(sideToIFF_StatusPair);
            }
            else if (forStatementStr == "for_category")
            {
               mIFF_CategoryData.insert(sideToIFF_StatusPair);
            }
            else if (forStatementStr == "by_default")
            {
               mIFF_DefaultData.insert(sideToIFF_StatusPair);
            }
         }
      }
      else
      {
         if (command == "default")
         {
            sideStringIDs.push_back(mDefaultId);
         }
         else
         {
            sideStringIDs.emplace_back(command);
         }
      }
   }
}
