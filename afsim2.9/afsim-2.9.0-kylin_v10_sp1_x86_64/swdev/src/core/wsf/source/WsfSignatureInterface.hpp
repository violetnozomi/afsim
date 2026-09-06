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

#ifndef WSFSIGNATUREINTERFACE_HPP
#define WSFSIGNATUREINTERFACE_HPP

#include "wsf_export.h"

#include <string>

class UtInput;
class WsfPlatform;
class WsfScenario;
class WsfSignature;
#include "WsfStringId.hpp"
#include "WsfVariable.hpp"

//! Abstract class that defines the interface between a platform and a signature.
//!
//! A signature class must provide at least the GetClassName(), GetShortName(), Clone() and CloneSignature()
//! methods. The other virtual methods may be overridden if something other than the default behavior is desired
//! for platform input and initialization procession.
class WSF_EXPORT WsfSignatureInterface
{
public:
   WsfSignatureInterface();
   WsfSignatureInterface(const WsfSignatureInterface& aSrc);
   WsfSignatureInterface& operator=(const WsfSignatureInterface&) = delete;
   virtual ~WsfSignatureInterface();

   virtual std::string            GetClassName() const                               = 0;
   virtual std::string            GetShortName() const                               = 0;
   virtual WsfSignatureInterface* Clone() const                                      = 0;
   virtual WsfSignature*          CloneSignature(const WsfScenario& aScenario) const = 0;

   virtual bool Initialize(double aSimTime, WsfPlatform* aPlatformPtr);
   virtual bool ProcessInput(UtInput& aInput);

   //! @name Get/Set methods for the member variables.
   //! No extra documentation is provided as they should be self-explanatory.
   //@{
   WsfStringId   GetInputType() const { return mInputType; }
   WsfStringId   GetState() const { return mState; }
   void          SetState(WsfStringId aState) { mState = aState; }
   float         GetScaleFactor() const { return mScaleFactor; }
   void          SetScaleFactor(float aScaleFactor) { mScaleFactor = aScaleFactor; }
   WsfSignature* GetSignature() const { return mSignaturePtr; }
   void          SetSignature(WsfSignature* aSignaturePtr);
   //@}

protected:
   //! The type of the signature specified by the user.
   WsfVariable<WsfStringId> mInputType;
   //! The current configuration state.
   WsfStringId mState;
   //! An additional scale factor to be applied to the raw signature value before returning it to the user.
   float mScaleFactor;
   //! Pointer to the raw signature.
   WsfSignature* mSignaturePtr;
};

#endif
