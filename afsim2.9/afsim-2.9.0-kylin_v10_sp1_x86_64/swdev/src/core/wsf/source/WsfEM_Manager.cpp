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

#include "WsfEM_Manager.hpp"

#include <algorithm>
#include <cassert>
#include <iterator>

#include "WsfArticulatedPart.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfPlatform.hpp"

//! Activate a receiver.
//! @param aRcvrPtr The receiver being activated.
//! @note This method does nothing if the receiver is already listed as active.
// static
void WsfEM_Manager::ActivateRcvr(WsfEM_Rcvr* aRcvrPtr)
{
   // Ensure that we only activate receivers that are properly connected (A device has
   // been attached to the receiver and a platform has been associated with the device)

   assert((aRcvrPtr->GetAntenna() != nullptr) && (aRcvrPtr->GetAntenna()->GetArticulatedPart()->GetPlatform() != nullptr));

   if (find(mRcvrPtrs.begin(), mRcvrPtrs.end(), aRcvrPtr) == mRcvrPtrs.end())
   {
      // Add the receiver to the list of active receivers.
      mRcvrPtrs.push_back(aRcvrPtr);

      // Inform all the transmitters about the new receiver.
      UpdateRcvr(aRcvrPtr);
   }
}

//! Activate a transmitter.
//! @param aXmtrPtr The transmitter being activated.
//! @note This method does nothing if the transmitter is already listed as active.
// static
void WsfEM_Manager::ActivateXmtr(WsfEM_Xmtr* aXmtrPtr)
{
   // Ensure that we only activate transmitters that are properly connected (A device has
   // been attached to the transmitter and a platform has been associated with the device)

   assert((aXmtrPtr->GetAntenna() != nullptr) && (aXmtrPtr->GetAntenna()->GetArticulatedPart()->GetPlatform() != nullptr));

   if (find(mXmtrPtrs.begin(), mXmtrPtrs.end(), aXmtrPtr) == mXmtrPtrs.end())
   {
      // Add the transmitter to the list of active transmitters.
      mXmtrPtrs.push_back(aXmtrPtr);

      // Inform all the receivers about the new transmitter.
      UpdateXmtr(aXmtrPtr);
   }
}

//! Indicate a receiver is no longer active.
//! @param aRcvrPtr The receiver being deactivated.
//! @note This does not delete the receiver.  It only marks it as inactive.
//! @note This method does nothing if the receiver is not listed as active.
// static
void WsfEM_Manager::DeactivateRcvr(WsfEM_Rcvr* aRcvrPtr)
{
   std::vector<WsfEM_Rcvr*>::iterator iter = find(mRcvrPtrs.begin(), mRcvrPtrs.end(), aRcvrPtr);
   if (iter != mRcvrPtrs.end())
   {
      for (unsigned int i = 0; i < mXmtrPtrs.size(); ++i)
      {
         (*iter)->RemoveInteractor(mXmtrPtrs[i]);
      }
      // Remove the receiver being deactivated from the list of active receivers
      mRcvrPtrs.erase(iter);
   }
}

//! Indicate a transmitter is no longer active.
//! @param aXmtrPtr The transmitter being deactivated.
//! @note This does not delete the transmitter.  It only marks it as inactive.
//! @note This method does nothing if the transmitter is not listed as active.
// static
void WsfEM_Manager::DeactivateXmtr(WsfEM_Xmtr* aXmtrPtr)
{
   std::vector<WsfEM_Xmtr*>::iterator iter = find(mXmtrPtrs.begin(), mXmtrPtrs.end(), aXmtrPtr);
   if (iter != mXmtrPtrs.end())
   {
      // Remove the transmitter being deactivated from the list of active transmitters
      mXmtrPtrs.erase(iter);

      // Remove the transmitter from any receiver that may have a potential interaction with it.
      for (unsigned int i = 0; i < mRcvrPtrs.size(); ++i)
      {
         mRcvrPtrs[i]->RemoveInteractor(aXmtrPtr);
      }
   }
}

//! Notify the manager that the indicated receiver has been updated.
//! @param aRcvrPtr The receiver that was updated.
// static
void WsfEM_Manager::UpdateRcvr(WsfEM_Rcvr* aRcvrPtr)
{
   if (find(mRcvrPtrs.begin(), mRcvrPtrs.end(), aRcvrPtr) != mRcvrPtrs.end())
   {
      for (unsigned int i = 0; i < mXmtrPtrs.size(); ++i)
      {
         aRcvrPtr->UpdateInteractions(mXmtrPtrs[i]);
      }
   }
}

//! Notify the manager that the indicated transmitter has been updated.
//! @param aXmtrPtr The transmitter that was updated.
// static
void WsfEM_Manager::UpdateXmtr(WsfEM_Xmtr* aXmtrPtr)
{
   if (find(mXmtrPtrs.begin(), mXmtrPtrs.end(), aXmtrPtr) != mXmtrPtrs.end())
   {
      for (unsigned int i = 0; i < mRcvrPtrs.size(); ++i)
      {
         mRcvrPtrs[i]->UpdateInteractions(aXmtrPtr);
      }
   }
}
