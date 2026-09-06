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

#ifndef WSFDISDRAW_HPP
#define WSFDISDRAW_HPP

#include "wsf_export.h"

class UtCallback;
class WsfDisInterface;
#include "WsfDraw.hpp"

//! Provides DIS output for WsfDraw
class WSF_EXPORT WsfDisDraw
{
public:
   WsfDisDraw(WsfDisInterface* aInterfacePtr);
   WsfDisDraw(bool aUseDeprecatedVersion, WsfDisInterface* aInterfacePtr);
   ~WsfDisDraw() = default;

private:
   void UpdateDrawEvents(const std::string& aTarget, const std::string& aLayer, const WsfDraw::DrawEventList& aEvents);

   void UpdateDrawEventsDeprecated(const std::string&            aTarget,
                                   const std::string&            aLayer,
                                   const WsfDraw::DrawEventList& aEvents);

   std::unique_ptr<UtCallback> mCallbackPtr;
   bool                        mUseDeprecatedVersion;
   WsfDisInterface*            mInterfacePtr;
   static const int            cVERSION;
};

#endif
