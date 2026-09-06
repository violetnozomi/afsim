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

#ifndef WSFCOMMPROTOCOLSTACK_HPP
#define WSFCOMMPROTOCOLSTACK_HPP

#include "wsf_export.h"

#include <memory>
#include <vector>

#include "WsfCommLayer.hpp"
#include "WsfCommLayerMessage.hpp"
#include "WsfStringId.hpp"

class UtInput;
class WsfPlatform;

namespace wsf
{
namespace comm
{

class Comm;
class Message;

//! This class represents a communication devices protocol stack.
class WSF_EXPORT ProtocolStack
{
public:
   using LayerType = Layer::LayerType;

   using ProtocolVector    = std::vector<std::unique_ptr<Layer>>;
   using RawProtocolVector = std::vector<Layer*>;

   //! Constructors and virtual destructor
   ProtocolStack() = default;
   ProtocolStack(const ProtocolStack& aSrc);
   ProtocolStack& operator=(const ProtocolStack& aRhs) = delete;
   virtual ~ProtocolStack()                            = default;

   //! Adds a layer at the top (end) of the stack.
   //! Returns a pointer to the added layer.
   Layer* AddLayer(std::unique_ptr<Layer> aLayerPtr);

   //! Removes a layer at the indicated position.
   bool RemoveLayer(size_t aLayerIndex);

   //! Removes a layer via unique ID.
   bool RemoveLayerByID(size_t aLayerID);

   //! Replaces a layer with the provided layer. The replaced layer is
   //! returned for potential further usage.
   std::unique_ptr<Layer> ReplaceLayer(size_t aLayerIndex, std::unique_ptr<Layer> aLayerPtr);

   //! Inserts a layer at the indicated position. Returns a pointer to
   //! inserted layer, or a nullptr upon failure.
   Layer* InsertLayer(size_t aLayerIndex, std::unique_ptr<Layer> aLayerPtr);

   //! @name Layer Retrieval Methods
   //@{

   //! Returns a pointer to the layer at the indicated position.
   Layer*            GetLayer(size_t aLayerIndex) const;
   Layer*            GetLayer(LayerType aLayerType) const;
   RawProtocolVector GetLayers(LayerType aLayerType) const;

   //! Template method for return of the first layer of the template object type.
   template<typename T>
   T* GetLayer() const
   {
      for (const auto& layer : mProtocolStack)
      {
         auto layerTypePtr = dynamic_cast<T*>(layer->GetCommLayerImp());
         if (layerTypePtr)
         {
            return layerTypePtr;
         }
      }
      return nullptr;
   }

   //! Template method for return of all layers of the template type.
   template<typename T>
   std::vector<T*> GetLayers() const
   {
      std::vector<T*> layers;
      for (const auto& layer : mProtocolStack)
      {
         auto layerTypePtr = dynamic_cast<T*>(layer->GetCommLayerImp());
         if (layerTypePtr)
         {
            layers.push_back(layerTypePtr);
         }
      }
      return layers;
   }

   Layer* GetLayerByID(size_t aLayerID) const;
   //@}


   //! Returns the number of layers in the stack.
   size_t Size() const { return mProtocolStack.size(); }

   //! Removes all layers from the stack.
   void ClearLayers() { mProtocolStack.clear(); }

   //! @name Common infrastructure methods.
   //@{
   virtual bool Initialize(double aSimTime);
   virtual bool Initialize2(double aSimTime);
   virtual bool ProcessInput(UtInput& aInput);
   //@}

   void  SetParent(Comm* aParentCommPtr);
   Comm* GetParent() const { return mParent; }
   void  SetDebugEnabled(bool aDebugEnabled);

   //! @name Send/Receive methods
   //@{
   virtual bool Receive(double aSimTime, Comm* aXmtrPtr, Message& aMessage);
   virtual bool Send(double aSimTime, Message& aMessage);
   //@}

   //! @name Comm event methods.
   //@{
   virtual bool ProcessLayerMessage(double aSimTime, size_t aLayerIndex, layer::Message aLayerMessage, Message* aCommMessagePtr);
   //@}

protected:
   //! Reconciles upper/lower stack settings upon modifications.
   void ValidateAdjacentLayers();

   //! Wrapped helper methods for finding a layer by ID.
   //@{
   ProtocolVector::iterator       FindLayerByID(size_t aID);
   ProtocolVector::const_iterator FindLayerByID(size_t aID) const;
   //@}

   //! Protocol stack for communication device.
   ProtocolVector mProtocolStack{};

   //! Pointer to the stack Parent.
   Comm* mParent{nullptr};
};

} // namespace comm
} // namespace wsf

#endif
