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

#ifndef WSFDISLASERDESIGNATIONS_HPP
#define WSFDISLASERDESIGNATIONS_HPP

class DisDesignator;
class DisEntityId;

class UtInput;

class WsfDisDesignator;
#include "wsf_mil_export.h"

#include "WsfLaserDesignations.hpp"
class WsfDisInterface;
class WsfMilDisInterface;

//! A class to receive Callbacks from or Push laser spot updates.
//! In the case of an incoming DisDesignator PDU, the new spot information
//! is pushed into WsfLaserDesignations.  In the case of an WSF-updated
//! laser spot, this class subscribes to a callback to be able to send the
//! new laser spot information externally via DisDesignator PDU.

class WSF_MIL_EXPORT WsfDisLaserDesignations
{
public:
   WsfDisLaserDesignations(WsfMilDisInterface* aInterfacePtr);
   ~WsfDisLaserDesignations() = default;

   bool Initialize();
   bool ProcessInput(UtInput& aInput);
   bool DebugEnabled() const { return mDebug; }

private:
   // External PDU received.  Inform WSF of the update.
   void ProcessPDU(WsfDisInterface* aInterfacePtr, const WsfDisDesignator& aPdu);

   // WSF spot was updated.  Send the update externally, if needed.
   void SpotUpdated(const WsfLaserDesignations::Spot& aSpot);

   // Utility function to populate an WSF Spot from a PDU.
   void SetSpotFromPduP(WsfLaserDesignations::Spot& aSpot,
                        size_t                      aDesignatingIndex,
                        size_t                      aDesignatedIndex,
                        const WsfDisDesignator&     aDesignation);

   // Utility function to populate an WSF Spot from a PDU.
   void SetPduFromSpotP(const WsfLaserDesignations::Spot& aSpot,
                        const DisEntityId&                aDesignatingId,
                        const DisEntityId&                aDesignatedId,
                        DisDesignator&                    aDesignation);


   bool                  mDebug;
   unsigned int          mSendCount;
   unsigned int          mReceiveCount;
   UtCallbackHolder      mCallbacks;
   WsfMilDisInterface*   mItarPtr;
   WsfDisInterface*      mDisInterfacePtr;
   WsfLaserDesignations* mLaserDesignationsPtr;
   WsfSimulation&        mSimulation;
};

#endif
