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

#include "WsfCommProtocolStack.hpp"

#include <iostream>

#include "WsfComm.hpp"
#include "WsfCommMessage.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfMessage.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace comm
{

// ****************************************************************************
// Public

// ============================================================================
ProtocolStack::ProtocolStack(const ProtocolStack& aSrc)
   : mProtocolStack()
   , mParent(aSrc.mParent)
{
   // Clone all the layers; this class owns the layers and must clean them up on destruction
   for (const auto& layer : aSrc.mProtocolStack)
   {
      AddLayer(ut::clone(layer));
   }
}

// ============================================================================
//! Adds the layer on the top of the stack. The stack takes ownership.
//! @param aLayerPtr [input] A pointer to a communication protocol layer.
//! @note Pointer must be non-null. Implementation does not support 'empty' or null layer.
Layer* ProtocolStack::AddLayer(std::unique_ptr<Layer> aLayerPtr)
{
   // Quick return for null pointer
   if (aLayerPtr == nullptr)
   {
      return nullptr;
   }

   // If this is not the first layer in the stack then let the new layer
   // know what layer is it's service provider (i.e. the layer below it)
   if (!mProtocolStack.empty())
   {
      const auto& lowerLayerPtr = mProtocolStack[mProtocolStack.size() - 1];
      aLayerPtr->SetLowerLayer(lowerLayerPtr.get());

      // Let the lower layer know what is above it
      lowerLayerPtr->SetUpperLayer(aLayerPtr.get());
   }

   // Set the index for the new layer
   aLayerPtr->SetIndex(mProtocolStack.size());

   // Set the layer parent.
   aLayerPtr->SetParent(GetParent());

   // Finally add the new layer to the stack
   mProtocolStack.emplace_back(std::move(aLayerPtr));
   return mProtocolStack.back().get();
}

// ============================================================================
bool ProtocolStack::RemoveLayer(size_t aLayerIndex)
{
   if (aLayerIndex >= mProtocolStack.size())
   {
      return false;
   }

   mProtocolStack.erase(std::begin(mProtocolStack) + aLayerIndex);
   ValidateAdjacentLayers();

   return true;
}

// ============================================================================
bool ProtocolStack::RemoveLayerByID(size_t aLayerID)
{
   auto it = FindLayerByID(aLayerID);
   if (it != std::end(mProtocolStack))
   {
      mProtocolStack.erase(it);
      ValidateAdjacentLayers();
      return true;
   }

   return false;
}

// ============================================================================
//! Replace a layer and returns the layer being removed.
//! @param aLayerIndex [input] The index of the layer to replace.
//! @param aLayerPtr    [input] A pointer to the new communication protocol layer.
//! @return A pointer to the layer being removed. It is left to the caller to delete.
std::unique_ptr<Layer> ProtocolStack::ReplaceLayer(size_t aLayerIndex, std::unique_ptr<Layer> aLayerPtr)
{
   // Check to see if the index is valid
   if (aLayerIndex >= mProtocolStack.size())
   {
      return nullptr;
   }

   // Perform the replacement
   auto layerToRemovePtr       = std::move(mProtocolStack[aLayerIndex]);
   mProtocolStack[aLayerIndex] = std::move(aLayerPtr);
   ValidateAdjacentLayers();
   layerToRemovePtr->SetParent(nullptr);
   return layerToRemovePtr;
}

// ============================================================================
Layer* ProtocolStack::InsertLayer(size_t aLayerIndex, std::unique_ptr<Layer> aLayerPtr)
{
   if (aLayerIndex >= mProtocolStack.size())
   {
      return nullptr;
   }

   auto it = mProtocolStack.insert(std::begin(mProtocolStack) + aLayerIndex, std::move(aLayerPtr));
   ValidateAdjacentLayers();
   return it->get();
}

// ============================================================================
// Given a valid index, returns a pointer to the layer in the stack.
//! @param aLayerIndex [input] The index of the layer to return.
//! @return A pointer to the layer at the provided index.
Layer* ProtocolStack::GetLayer(size_t aLayerIndex) const
{
   return mProtocolStack.at(aLayerIndex).get();
}

// ============================================================================
Layer* ProtocolStack::GetLayer(LayerType aLayerType) const
{
   for (const auto& layer : mProtocolStack)
   {
      if (layer->GetLayerType() == aLayerType)
      {
         return layer.get();
      }
   }
   return nullptr;
}

// ============================================================================
ProtocolStack::RawProtocolVector ProtocolStack::GetLayers(LayerType aLayerType) const
{
   RawProtocolVector layers;
   for (const auto& layer : mProtocolStack)
   {
      if (layer->GetLayerType() == aLayerType)
      {
         layers.push_back(layer.get());
      }
   }
   return layers;
}

// ============================================================================
Layer* ProtocolStack::GetLayerByID(size_t aLayerID) const
{
   auto it = FindLayerByID(aLayerID);
   if (it != std::end(mProtocolStack))
   {
      return it->get();
   }

   return nullptr;
}

// ============================================================================
// virtual
//! Initializes the layers in the protocol stack.
//! @param aSimTime [input] The current simulation time.
//! @return 'true' if all layers are successfully initialized or 'false' if any
//! one layer failed initialization.
bool ProtocolStack::Initialize(double aSimTime)
{
   bool ok = true;

   // Loop from bottom layer to the top
   for (const auto& layer : mProtocolStack)
   {
      if (!layer->Initialize(aSimTime))
      {
         ok = false;
         break; // Break out if any layer cannot initialize
      }
   }
   return ok;
}

// ============================================================================
// virtual
//! Initializes the layers in the protocol stack.
//! @param aSimTime [input] The current simulation time.
//! @return 'true' if all layers are successfully initialized or 'false' if any
//! one layer failed initialization.
bool ProtocolStack::Initialize2(double aSimTime)
{
   bool ok = true;

   for (const auto& layer : mProtocolStack)
   {
      if (!layer->Initialize2(aSimTime))
      {
         ok = false;
         break;
      }
   }
   return ok;
}

// ============================================================================
// virtual
bool ProtocolStack::ProcessInput(UtInput& aInput)
{
   bool        myCommand = false;
   std::string command   = aInput.GetCommand();

   // Loop from bottom layer to the top
   for (const auto& layer : mProtocolStack)
   {
      if (layer->ProcessInput(aInput))
      {
         myCommand = true;
         break; // Break out if any layer can process the input
      }
   }
   return myCommand;
}

// ============================================================================
//! Provides each layer with a pointer to the parent communication device.
//! @param aParentCommPtr [input] A pointer to the communication device that owns this stack.
//! @note Pointer must be non-null. Implementation does not support 'empty' or null pointer.
void ProtocolStack::SetParent(Comm* aParentCommPtr)
{
   // Quick return for null pointer
   if (aParentCommPtr == nullptr)
   {
      return;
   }

   mParent = aParentCommPtr;

   // Loop from bottom to top layer
   for (const auto& layer : mProtocolStack)
   {
      layer->SetParent(aParentCommPtr);
   }
}

// ============================================================================
void ProtocolStack::SetDebugEnabled(bool aDebugEnabled)
{
   // Loop from bottom to top layer
   for (const auto& layer : mProtocolStack)
   {
      layer->SetDebugEnabled(aDebugEnabled);
   }
}

// ============================================================================
// virtual
//! Called from the communication device on receipt of a communication message.
//! @param aSimTime [input] The current simulation time.
//! @param aXmtrPtr {input] The comm device that sent the message.
//! @param aMessage [input] The communication message being received.
//! @return 'true' if protocol stack successfully processed the message.
bool ProtocolStack::Receive(double aSimTime, Comm* aXmtrPtr, Message& aMessage)
{
   // Start at the bottom
   return mProtocolStack[0]->Receive(aSimTime, aXmtrPtr, aMessage);
}

// ============================================================================
// virtual
//! Called by the communication device to send a communication message.
//! @param aSimTime [input] The current simulation time.
//! @param aMessage [input] The communication message being received.
//! @return 'true' if protocol stack successfully processed the message.
bool ProtocolStack::Send(double aSimTime, Message& aMessage)
{
   //! Set the required fields for the message. We don't leave this up to
   //! the specific implementations of comm objects due to being required
   //! across all comm objects. In addition, this avoids having to query
   //! the network manager within the implementations.
   aMessage.SourceMessage()->SetSrcAddr(GetParent()->GetAddress());

   // Start at the top
   return mProtocolStack.back()->Send(aSimTime, aMessage);
}

// ============================================================================
//! Process a layer event.
// virtual
bool ProtocolStack::ProcessLayerMessage(double aSimTime, size_t aLayerIndex, layer::Message aLayerMessage, Message* aCommMessagePtr)
{
   if (aLayerIndex < mProtocolStack.size())
   {
      return mProtocolStack[aLayerIndex]->ProcessLayerMessage(aSimTime, aLayerMessage, aCommMessagePtr);
   }

   return false;
}

// ============================================================================
void ProtocolStack::ValidateAdjacentLayers()
{
   for (size_t i = 0; i < mProtocolStack.size(); ++i)
   {
      if (i != 0)
      {
         mProtocolStack[i]->SetLowerLayer(mProtocolStack[i - 1U].get());
      }
      else
      {
         mProtocolStack[i]->SetLowerLayer(nullptr);
      }

      if (i != (mProtocolStack.size() - 1U))
      {
         mProtocolStack[i]->SetUpperLayer(mProtocolStack[i + 1U].get());
      }
      else
      {
         mProtocolStack[i]->SetUpperLayer(nullptr);
      }

      mProtocolStack[i]->SetIndex(i);
      mProtocolStack[i]->SetParent(GetParent());
   }
}

// ============================================================================
ProtocolStack::ProtocolVector::iterator ProtocolStack::FindLayerByID(size_t aID)
{
   auto it = std::find_if(std::begin(mProtocolStack),
                          std::end(mProtocolStack),
                          [aID](const std::unique_ptr<Layer>& aLayer) { return (aLayer->GetUniqueId() == aID); });

   return it;
}

// ============================================================================
ProtocolStack::ProtocolVector::const_iterator ProtocolStack::FindLayerByID(size_t aID) const
{
   auto it = std::find_if(std::begin(mProtocolStack),
                          std::end(mProtocolStack),
                          [aID](const std::unique_ptr<Layer>& aLayer) { return (aLayer->GetUniqueId() == aID); });

   return it;
}


} // namespace comm
} // namespace wsf
