// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFJTIDS_TERMINAL_HPP
#define WSFJTIDS_TERMINAL_HPP

#include "wsf_mil_export.h"

#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"
#include "WsfComm.hpp"
#include "WsfCommMessage.hpp"

class WsfJTIDS_SlotGroup;

/** A specialization of WsfComm representing a JTIDS terminal.

This class, along with WsfJTIDS_SlotGroup constitute the implementation
of the JTIDS model.  The implementation was derived mostly from reading:

Understanding Link-16; a Guidebook for New Users (Logicon, Inc.)
NATO STANAG 5516

The current implementation models network capacity by requiring only the
number of slots per frame needed by a slot group.  It does not require
(nor does it yet allow) a network definition via actual time slot block
definitions (The current structure will allow it to be implemented at
a future date).

The current model implements:

*) multiple networks
*) per-slot-group packing limit
*) paired slot relays
*) dedicated access
*) time-slot reuse in relays (flood relays)

The model does not yet implement:

*) Contention access.
*) Time slot reallocation access.
*) Repromulgated relays.

Note that this is NOT a Link-16 model - it doesn't explicitly model the
J-series messages.  That is left to something higher up.  This really just
models the physical transport of a message.

The model also allows changing of the basic network slot parameters
(bits per slot, seconds per slot, slots per frame) to allow modeling of
future concepts that may use the JTIDS TDMA architecture but with some
modifications.
*/

class WSF_MIL_EXPORT WsfJTIDS_Terminal : public wsf::comm::Comm
{
public:
   WsfJTIDS_Terminal(WsfScenario& aScenario);
   ~WsfJTIDS_Terminal() override;

   /** Return the RTTI class ID */
   static WsfStringId GetJTIDSClassId();

   //! @name Common infrastructure methods.
   //@{
   wsf::comm::Comm* Clone() const override;
   bool             Initialize(double aSimTime) override;
   void             TurnOff(double aSimTime) override;
   //@}

   //! @name Send/Receive methods
   //@{
   bool ReceiveSetup(double aSimTime, wsf::comm::Comm* aXmtrPtr, wsf::comm::Message& aMessage) override;

   bool SendSetup(double aSimTime, wsf::comm::Message& aMessage, const wsf::comm::Address& aAddress) override;

   bool ReceiveActions(double aSimTime, bool aReceiveStatus, Comm* aXmtrPtr, wsf::comm::Message& aMessage) override;
   //@}

   //! @name Callback lists.
   //@{
   UtCallbackListN<void(double, const WsfMessage*, int, WsfJTIDS_SlotGroup*)> MessageSent;
   //@}

protected:
   //! Copy Constructor
   WsfJTIDS_Terminal(const WsfJTIDS_Terminal& aSrc);

   void PrintDebugReception(double aSimTime, const wsf::comm::Message& aMessage) const override;

private:
   //! Prevent use of operator= by declaring, but not defining.
   WsfJTIDS_Terminal& operator=(const WsfJTIDS_Terminal& aRhs) = delete;

   void HandleMessageSent(double aSimTime, const WsfMessage* aMessagePtr, int aSlotNumber, WsfJTIDS_SlotGroup* aSlotGroup);

   UtCallbackHolder mCallbacks;
};

#endif
