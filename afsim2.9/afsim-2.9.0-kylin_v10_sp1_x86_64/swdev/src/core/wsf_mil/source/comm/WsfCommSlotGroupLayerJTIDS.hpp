// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOMMSLOTGROUPLAYERJTIDS_HPP
#define WSFCOMMSLOTGROUPLAYERJTIDS_HPP

#include <map>
#include <vector>

#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"
#include "WsfStringId.hpp"

class WsfCommandChain;
class WsfMessage;
class WsfJTIDS_SlotGroup;

// Base class
#include "WsfCommLayer.hpp"

namespace wsf
{
namespace comm
{

class SlotGroupLayerJTIDS : public LayerImp
{
public:
   // Constructor and destructor
   SlotGroupLayerJTIDS();
   ~SlotGroupLayerJTIDS() override;

   //! @name Common infrastructure methods.
   //@{
   wsf::comm::LayerImp* Clone() const override;
   bool                 Initialize(double aSimTime) override;
   bool                 ProcessInput(UtInput& aInput) override;
   //@}

   //! @name Send/Receive methods
   //@{
   bool Receive(double aSimTime, Comm* aXmtrPtr, Message& aMessage, bool& aNotifyNextLayer) override;
   bool Send(double aSimTime, size_t aLayerIndex, Message& aMessage, bool& aNotifyNextLayer) override;
   //@}

   //! @name Comm event methods.
   //@{
   bool ProcessLayerMessage(double aSimTime, layer::Message aMessage, Message* aCommMessagePtr) override;
   //@}

   void TurnOff(double aSimTime);

   WsfJTIDS_SlotGroup* FindGroup(WsfStringId aGroupId);

   /** Return the number of bits per slot at standard packing. */
   int GetBitsPerSlot() const { return mBitsPerSlot; }

   /** Return the number of seconds in a slot. */
   double GetSecondsPerSlot() const { return mSecondsPerSlot; }

   /** Return the number of sets per frame. */
   int GetSetsPerFrame() const { return mSetsPerFrame; }

   /** Return the number of slots in a frame (slots/set * sets/frame). */
   int GetSlotsPerFrame() const { return mSlotsPerSet * mSetsPerFrame; }

   /** Return the number of slots per set */
   int GetSlotsPerSet() const { return mSlotsPerSet; }

   //! @name Callback lists.
   //@{
   UtCallbackListN<void(double, const WsfMessage*, int, WsfJTIDS_SlotGroup*)> MessageSent;
   //@}

protected:
   // Copy constructor
   SlotGroupLayerJTIDS(const SlotGroupLayerJTIDS& aSrc);

private:
   //! Prevent use of operator= by declaring, but not defining.
   SlotGroupLayerJTIDS& operator=(const SlotGroupLayerJTIDS& aRhs) = delete;

   bool InitializeSendRequest(double aSimTime, const WsfStringId& aCmdChainId, WsfJTIDS_SlotGroup*& aGroupPtr);

   void HandleMessageSent(double aSimTime, const WsfMessage* aMessagePtr, int aSlotNumber, WsfJTIDS_SlotGroup* aSlotGroup);

   // The amount of time in one time slot
   double mSecondsPerSlot;

   // The number of bits in one time slot at the standard packing density
   int mBitsPerSlot;

   // The number of interleaved sets (this must divide evenly in to the number of slots per frame)
   int mSetsPerFrame;

   // The number of slots per set */
   int mSlotsPerSet;

   // The time at which this terminal last received a message.
   // (Used to implement time slot reuse)
   double mLastReceiveTime;

   std::vector<WsfJTIDS_SlotGroup*>   mSlotGroups;
   std::map<WsfStringId, WsfStringId> mCmdChainToGroup;

   UtCallbackHolder mCallbacks;
};


} // namespace comm
} // namespace wsf

#endif
