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

#include "WsfPM_Comm.hpp"


WsfProxy::String WsfPM_Comm::GetNetworkName()
{
   WsfProxy::String* stringPtr = GetValue()["networkName"];
   if (stringPtr)
   {
      return *stringPtr;
   }
   return WsfProxy::String();
}

WsfPM_CommLink WsfPM_Comm::GetLink(size_t aIndex) const
{
   WsfPM_CommLinkList links = LinksList();
   if (links.size() > aIndex)
   {
      return links[aIndex];
   }
   return WsfPM_CommLink();
}
