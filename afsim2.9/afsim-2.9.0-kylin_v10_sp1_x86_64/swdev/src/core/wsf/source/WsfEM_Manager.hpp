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

#ifndef WsfEM_MANAGER_HPP
#define WsfEM_MANAGER_HPP

#include "wsf_export.h"

#include <vector>

class WsfEM_Rcvr;
class WsfEM_Xmtr;

//! A manager for interactions between electromagnetic devices.
//!
//! This class is responsible for maintaining the data structures necessary to
//! support the efficient handling of interactions between receivers and transmitters.
//! In particular, it does two things:
//!
//! - Within each active receiver, it maintains a list of transmitters with which the
//! receiver could potential interact (has a compatible frequency).
//! - Maintains a list of all active transmitters and receivers.
//!
//! The methods within this class are called from within the WsfEM_Rcvr and WsfEM_Xmtr
//! classes to maintain the interaction structures.  Sensor and
//! communications systems that use the WsfEM_Rcvr and WsfEM_Xmtr classes must
//! follow the proper protocol to ensure that interactions between systems can
//! occur:
//!
//! - When a system is turned on, the system must invoke the 'Activate()' method for
//! each transmitter and receiver associated with the system.
//! - When a system is turned off, the system must invoke the 'Deactivate()' method for
//! each transmitter and receiver associated with the system.

class WSF_EXPORT WsfEM_Manager
{
public:
   void ActivateRcvr(WsfEM_Rcvr* aRcvrPtr);

   void ActivateXmtr(WsfEM_Xmtr* aXmtrPtr);

   void DeactivateRcvr(WsfEM_Rcvr* aRcvrPtr);

   void DeactivateXmtr(WsfEM_Xmtr* aXmtrPtr);

   void UpdateRcvr(WsfEM_Rcvr* aRcvrPtr);

   void UpdateXmtr(WsfEM_Xmtr* aXmtrPtr);

   //! Return the number of active receivers
   //! @return The number of active receivers.
   unsigned int GetRcvrCount() { return static_cast<unsigned int>(mRcvrPtrs.size()); }

   //! Return the number of active transmitters
   //! @return The number of active transmitters.
   unsigned int GetXmtrCount() { return static_cast<unsigned int>(mXmtrPtrs.size()); }

   //! Get an entry from the list of active receivers.
   //! @param aEntry The index of the entry desired.
   //! @return The pointer to requested entry.
   //! @note aEntry must be within range: 0 <= aEntry < GetRcvrCount().
   WsfEM_Rcvr* GetRcvrEntry(unsigned int aEntry) { return mRcvrPtrs[aEntry]; }

   //! Get an entry from the list of active transmitters.
   //! @param aEntry The index of the entry desired.
   //! @return The pointer to requested entry.
   //! @note aEntry must be within range: 0 <= aEntry < GetXmtrCount().
   WsfEM_Xmtr* GetXmtrEntry(unsigned int aEntry) { return mXmtrPtrs[aEntry]; }

private:
   //! A list of active receivers
   std::vector<WsfEM_Rcvr*> mRcvrPtrs;

   //! A list of active transmitters.
   std::vector<WsfEM_Xmtr*> mXmtrPtrs;
};

#endif
