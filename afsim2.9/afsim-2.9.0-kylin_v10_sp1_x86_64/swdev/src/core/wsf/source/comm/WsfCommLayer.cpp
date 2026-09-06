// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCommLayer.hpp"

#include <algorithm>

#include "WsfComm.hpp"
#include "WsfCommEvent.hpp"
#include "WsfCommMessage.hpp"

namespace wsf
{
namespace comm
{

// ****************************************************************************
// Public

// ============================================================================
Layer::Layer(Layer::LayerType aCommLayerType, LayerImp* aCommLayerImpPtr)
   : mCommLayerImpPtr(aCommLayerImpPtr)
   , mCommLayerType(aCommLayerType)
   , mCommLayerIndex(0)
   , mUpperCommLayerPtr(nullptr)
   , mLowerCommLayerPtr(nullptr)
{
}

// ============================================================================
Layer::~Layer()
{
   delete mCommLayerImpPtr;
}

// ============================================================================
Layer* Layer::Clone() const
{
   return new Layer(*this);
}

// ============================================================================
bool Layer::Initialize(double aSimTime)
{
   if (!Parent())
   {
      return false;
   }

   AssignUniqueId(Parent()->GetSimulation());
   return GetCommLayerImp()->Initialize(aSimTime);
}

// ============================================================================
bool Layer::Initialize2(double aSimTime)
{
   return GetCommLayerImp()->Initialize2(aSimTime);
}

// ============================================================================
bool Layer::ProcessInput(UtInput& aInput)
{
   return GetCommLayerImp()->ProcessInput(aInput);
}

// ============================================================================
//! Receive a message.
//! It is the responsibility of the receiver to actually determine if the message
//! can be received (i.e. line-of-sight, enough power, etc).
//! @param aSimTime  The current simulation time.
//! @param aXmtrPtr  The comm device that sent the message.
//! @param aMessage  The received message.
//! @returns true if the message was received or false if it could not.
// virtual
bool Layer::Receive(double aSimTime, Comm* aXmtrPtr, Message& aMessage)
{
   bool notifyNextLayer = false;
   bool success         = GetCommLayerImp()->Receive(aSimTime, aXmtrPtr, aMessage, notifyNextLayer);
   if (notifyNextLayer && (mUpperCommLayerPtr != nullptr))
   {
      success = mUpperCommLayerPtr->Receive(aSimTime, aXmtrPtr, aMessage);
   }

   return success;
}

// ============================================================================
//! Send a message to a specific platform.
//! @param aSimTime       The current simulation time.
//! @param aMessage       The message to be sent.
// virtual
bool Layer::Send(double aSimTime, Message& aMessage)
{
   bool notifyNextLayer = false;
   bool success         = GetCommLayerImp()->Send(aSimTime, mCommLayerIndex, aMessage, notifyNextLayer);
   if (notifyNextLayer && (mLowerCommLayerPtr != nullptr))
   {
      success = mLowerCommLayerPtr->Send(aSimTime, aMessage);
   }

   return success;
}

// ============================================================================
//! Process a layer event.
// virtual
bool Layer::ProcessLayerMessage(double aSimTime, layer::Message aLayerMessage, Message* aCommMessagePtr)
{
   return GetCommLayerImp()->ProcessLayerMessage(aSimTime, aLayerMessage, aCommMessagePtr);
}

// ============================================================================
Comm* Layer::Parent() const
{
   return mCommLayerImpPtr->Parent();
}

// ============================================================================
void Layer::SetParent(Comm* aParentCommPtr)
{
   GetCommLayerImp()->SetParent(aParentCommPtr);
}

// ============================================================================
void Layer::SetDebugEnabled(bool aDebugEnabled)
{
   GetCommLayerImp()->SetDebugEnabled(aDebugEnabled);
}

// ============================================================================
void Layer::SetID(WsfSimulation& aSim)
{
   AssignUniqueId(&aSim);
}

// ============================================================================
void Layer::SetUpperLayer(Layer* aLayerPtr)
{
   mUpperCommLayerPtr = aLayerPtr;
   if (mCommLayerImpPtr)
   {
      mCommLayerImpPtr->SetUpperLayer(aLayerPtr);
   }
}

// ============================================================================
void Layer::SetLowerLayer(Layer* aLayerPtr)
{
   mLowerCommLayerPtr = aLayerPtr;
   if (mCommLayerImpPtr)
   {
      mCommLayerImpPtr->SetLowerLayer(aLayerPtr);
   }
}

// ============================================================================
void Layer::SetIndex(size_t aIndex)
{
   mCommLayerIndex = aIndex;
   GetCommLayerImp()->SetIndex(aIndex);
}

// ****************************************************************************
// Protected

// ============================================================================
//! Copy constructor (for Clone())
Layer::Layer(const Layer& aSrc)
   : mCommLayerImpPtr(aSrc.mCommLayerImpPtr->Clone())
   , mCommLayerType(aSrc.mCommLayerType)
   , mCommLayerIndex(aSrc.mCommLayerIndex)
   , mUpperCommLayerPtr(aSrc.mUpperCommLayerPtr)
   , mLowerCommLayerPtr(aSrc.mLowerCommLayerPtr)
{
   GetCommLayerImp()->SetParent(aSrc.Parent());
}

// ============================================================================
LayerImp* Layer::GetCommLayerImp()
{
   return mCommLayerImpPtr;
}

// ============================================================================
Comm* LayerImp::Parent() const
{
   return mParentCommPtr;
}

// ============================================================================
void LayerImp::SetParent(Comm* aParentCommPtr)
{
   mParentCommPtr = aParentCommPtr;
}

} // namespace comm
} // namespace wsf
