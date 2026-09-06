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

#ifndef WSFXIO_OBJECTINFOLOADHELPERS_HPP
#define WSFXIO_OBJECTINFOLOADHELPERS_HPP

//! This is a collection of templates for use in implementation files that load information
//! from WSF objects into XIO objects.
//!
//! These are all put into a local anonymous namespace.
//
//! WsfXIO_ObjectInfoLoad.hpp must also be included in the implementation file.

namespace
{
//! the modes from a WSF 'moded' platform part into the XIO 'moded part'
template<typename T>
void Load_ModedPart(WsfXIO_Simulation& aXIO_Simulation, WsfXIO_ModedPartInfo& aInfo, T& aPart)
{
   WsfXIO_ObjectInfoLoad::Load_ArticulatedPart(aXIO_Simulation, aInfo, aPart); // Load base class
   aInfo.mCurrentModeName = aPart.GetCurrentModeName();
   for (size_t i = 0; i < aPart.GetModeCount(); ++i)
   {
      aInfo.mModeNames.push_back(WsfStringId(aPart.GetModeName(i)));
      if (aPart.GetModeEntry(i) && aPart.GetModeEntry(i)->IsSelected())
      {
         aInfo.mSelectedModeNames.push_back(WsfStringId(aPart.GetModeName(i)));
      }
   }
   if (aInfo.mSelectedModeNames.empty())
   {
      aInfo.mSelectedModeNames.push_back(WsfStringId(aPart.GetCurrentModeName()));
   }
}

//! Load transmitters from a WSF platform part into a vector of XIO transmitters
template<typename PART>
void Load_Transmitters(WsfXIO_Simulation& aXIO_Simulation, PART& aPart, std::vector<WsfXIO_EM_XmtrInfo*>& aXmtrInfos)
{
   aXmtrInfos.resize(aPart.GetEM_XmtrCount());
   for (unsigned int i = 0; i < aPart.GetEM_XmtrCount(); ++i)
   {
      WsfEM_Xmtr&         xmtr    = aPart.GetEM_Xmtr(i);
      WsfXIO_EM_XmtrInfo* infoPtr = new WsfXIO_EM_XmtrInfo;
      WsfXIO_ObjectInfoLoad::Load_EM_Xmtr(aXIO_Simulation, *infoPtr, xmtr);
      aXmtrInfos[i] = infoPtr;
   }
}

//! Load receivers from a WSF platform part into a vector of XIO receivers
template<typename PART>
void Load_Receivers(WsfXIO_Simulation& aXIO_Simulation, PART& aPart, std::vector<WsfXIO_EM_RcvrInfo*>& aRcvrInfos)
{
   aRcvrInfos.resize(aPart.GetEM_RcvrCount());
   for (unsigned int i = 0; i < aPart.GetEM_RcvrCount(); ++i)
   {
      WsfEM_Rcvr&         rcvr    = aPart.GetEM_Rcvr(i);
      WsfXIO_EM_RcvrInfo* infoPtr = new WsfXIO_EM_RcvrInfo;
      WsfXIO_ObjectInfoLoad::Load_EM_Rcvr(aXIO_Simulation, *infoPtr, rcvr);
      aRcvrInfos[i] = infoPtr;
   }
}
} // namespace

#endif
