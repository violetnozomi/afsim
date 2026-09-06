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

#ifndef WSFIFFMANAGER_HPP
#define WSFIFFMANAGER_HPP

#include "wsf_export.h"

#include <map>
#include <string>

#include "WsfCategoryList.hpp"
#include "WsfTrack.hpp"

class UtInput;

//! IFF manager (Singleton)
class WSF_EXPORT WsfIFF_Manager
{
public:
   //! Constructor
   WsfIFF_Manager();

   //! Virtual destructor
   virtual ~WsfIFF_Manager();

   //! Process the IFF block.
   bool ProcessInput(UtInput& aInput);

   //! Determine if debugging is enabled.
   bool DebugEnabled() const { return mDebugEnabled; }

   //! Set the debug state.
   //! @param aDebugEnabled New debug state
   void SetDebugEnabled(bool aDebugEnabled) { mDebugEnabled = aDebugEnabled; }

   //! @name IFF check methods
   //@{
   WsfTrack::IFF_Status GetIFF_Status(WsfStringId                          aPlaformSideId,
                                      WsfStringId                          aTgtPlatformSideId,
                                      const WsfCategoryList::CategoryList& aTgtCategories) const;
   //@}

   typedef std::pair<WsfStringId, WsfStringId>                       SideMapTypePair;
   typedef std::pair<SideMapTypePair, WsfTrack::IFF_Status>          SideIFF_MapTypePair;
   typedef std::map<SideMapTypePair, WsfTrack::IFF_Status>           SideIFF_MapType;
   typedef std::map<SideMapTypePair, WsfTrack::IFF_Status>::iterator SideIFF_MapTypeIterator;

protected:
private:
   WsfIFF_Manager(const WsfIFF_Manager& aSrc);
   //! Prevent use of operator= by declaring, but not defining.
   WsfIFF_Manager& operator=(const WsfIFF_Manager& aRhs) = delete;

   //! Converts a string to the enumeration
   bool ConvertIFF_String(const std::string& aIFF_Str, WsfTrack::IFF_Status& aIFF_Status);

   //! Private implementation
   bool GetIFF_StatusP(WsfStringId                          aPlaformSideId,
                       WsfStringId                          aTgtPlatformSideId,
                       const WsfCategoryList::CategoryList& aTgtCategories,
                       WsfTrack::IFF_Status&                aIFF_Status) const;

   //! Process side information
   void ProcessSide(UtInput& aInput);

   //! IFF data map - ((side, side), IFF)
   SideIFF_MapType mIFF_SideData;

   //! IFF data map - ((side, category), IFF)
   SideIFF_MapType mIFF_CategoryData;

   //! IFF data map - ((side, ?), IFF)
   SideIFF_MapType mIFF_DefaultData;

   //! Debug flag
   bool mDebugEnabled;

   WsfStringId mDefaultId;
};

#endif
