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

#ifndef WSFCOMMLAYER_HPP
#define WSFCOMMLAYER_HPP

#include "wsf_export.h"

#include <vector>

#include "WsfCommLayerMessage.hpp"
#include "WsfUniqueId.hpp"

// Forward Declarations
class UtInput;
class WsfPlatform;

namespace wsf
{
namespace comm
{

class Comm;
class LayerImp;
class Message;

//! A base class for layers in the communication stack. It uses a
//! bridge pattern to decouple the integration with the AFSIM simulation from
//! the detailed implementation in a layer. Each layer has a unique ID
//! assigned at initialization for potential runtime management of layers.
class WSF_EXPORT Layer : public WsfUniqueId
{
public:
   //! Communication layer as defined by the
   //! Open Systems Interconnection (OSI) model
   enum class LayerType : size_t
   {
      cPHYSICAL,
      cDATALINK,
      cNETWORK,
      cTRANSPORT,
      cSESSION,
      cPRESENTATION,
      cAPPLICATION,
      cNONE
   };

   // Constructor and destructor
   Layer(Layer::LayerType aCommLayerType, LayerImp* aCommLayerImpPtr);
   virtual ~Layer();

   //! @name Common infrastructure methods.
   //@{
   virtual Layer* Clone() const;
   virtual bool   Initialize(double aSimTime);
   virtual bool   Initialize2(double aSimTime);
   virtual bool   ProcessInput(UtInput& aInput);
   //@}

   //! @name Send/Receive methods
   //@{
   virtual bool Receive(double aSimTime, Comm* aXmtrPtr, Message& aMessage);
   virtual bool Send(double aSimTime, Message& aMessage);
   //@}

   //! @name Comm event methods.
   //@{
   virtual bool ProcessLayerMessage(double aSimTime, layer::Message aLayerMessage, Message* aCommMessagePtr);
   //@}

   //! @name Methods to access the owning communication device.
   //@{
   Comm* Parent() const;
   void  SetParent(Comm* aParentCommPtr);
   //@}

   void SetDebugEnabled(bool aDebugEnabled);

   LayerType GetLayerType() const { return mCommLayerType; }

   //! @name Runtime assignment of unique id.
   //! Wrapper for WsfUniqueId::AssignUniqueId for public usage.
   void SetID(WsfSimulation& aSim);

   //! Methods to inform a layer of its neighbors and location in the protocol stack.
   //! These methods also propagate such data to the implementation for more
   //! complicated layer implementation that may need to know this data at runtime.
   void   SetUpperLayer(Layer* aLayerPtr);
   void   SetLowerLayer(Layer* aLayerPtr);
   void   SetIndex(size_t aIndex);
   Layer* GetUpperLayer() const { return mUpperCommLayerPtr; }
   Layer* GetLowerLayer() const { return mLowerCommLayerPtr; }


   // Access to implementation
   LayerImp* GetCommLayerImp();

protected:
   // Copy constructor
   Layer(const Layer& aSrc);

private:
   //! Prevent use of operator= by declaring, but not defining.
   Layer& operator=(const Layer& aRhs) = delete;

   // Pointer to the layer implementation (Implementor)
   LayerImp* mCommLayerImpPtr;

   LayerType mCommLayerType;  // OSI layer type
   size_t    mCommLayerIndex; // Layer index is 0-based

   Layer* mUpperCommLayerPtr; // Pointer to the layer above, if it exists
   Layer* mLowerCommLayerPtr; // Pointer to the layer below, if it exists
};

// ****************************************************************************
//! A base class defining the interface for the implementation classes. (Implementor)
class WSF_EXPORT LayerImp
{
public:
   virtual ~LayerImp() = default;

   //! @name Common infrastructure methods.
   //@{
   virtual LayerImp* Clone() const               = 0;
   virtual bool      Initialize(double aSimTime) = 0;
   virtual bool      Initialize2(double aSimTime) { return true; }
   virtual bool      ProcessInput(UtInput& aInput) = 0;
   //@}

   //! Receive the message in this layer.
   //! @param aSimTime         [input] The current simulation time.
   //! @param aXmtrPtr         [input] The comm device that sent the message.
   //! @param aMessage         [input] The communication message being received.
   //! @param aNotifyNextLayer [output] True if the next layer above this one should immediately process this message.
   //! @return 'true' if layer successfully processed the message.
   virtual bool Receive(double aSimTime, Comm* aXmtrPtr, Message& aMessage, bool& aNotifyNextLayer) = 0;

   //! Send the message to this layer.
   //! @param aSimTime         [input] The current simulation time.
   //! @param aLayerIndex      [input] The index in the protocol stack for this layer.
   //! @param aMessage         [input] The communication message being received.
   //! @param aNotifyNextLayer [output] True if the next layer below this one should immediately process this message.
   //! @return 'true' if layer successfully processed the message.
   virtual bool Send(double aSimTime, size_t aLayerIndex, Message& aMessage, bool& aNotifyNextLayer) = 0;

   //! Process a layer event.
   //! @param aSimTime        [input] The current simulation time.
   //! @param aLayerMessage   [input] The layer message.
   //! @param aCommMessagePtr [input] The message being processed.
   virtual bool ProcessLayerMessage(double aSimTime, layer::Message aLayerMessage, Message* aCommMessagePtr) = 0;

   //! @name Methods to access the owning communication device.
   //@{
   Comm* Parent() const;
   void  SetParent(Comm* aParentCommPtr);
   //@}

   //! @name Indexing methods, pushed down to this object for retrieval directly from
   //! the implementation from the layer.
   void   SetIndex(size_t aIndex) { mIndex = aIndex; }
   size_t GetIndex() const { return mIndex; }

   //! @name Upper/Lower layer methods, pushed down to this object for retrieval directly from
   //! the implementation from the layer.
   void   SetUpperLayer(Layer* aLayerPtr) { mUpperCommLayerPtr = aLayerPtr; }
   Layer* GetUpperLayer() const { return mUpperCommLayerPtr; }
   void   SetLowerLayer(Layer* aLayerPtr) { mLowerCommLayerPtr = aLayerPtr; }
   Layer* GetLowerLayer() const { return mLowerCommLayerPtr; }

   //! Determine if debugging is enabled.
   bool DebugEnabled() const { return mDebugEnabled; }

   //! Set the debug state.
   //! @param aDebugEnabled New debug state
   void SetDebugEnabled(bool aDebugEnabled) { mDebugEnabled = aDebugEnabled; }
   //@}

private:
   Comm*  mParentCommPtr{nullptr}; // Pointer back to the owning communications device
   bool   mDebugEnabled{false};
   size_t mIndex;
   Layer* mUpperCommLayerPtr{nullptr};
   Layer* mLowerCommLayerPtr{nullptr};
};

} // namespace comm
} // namespace wsf

#endif
