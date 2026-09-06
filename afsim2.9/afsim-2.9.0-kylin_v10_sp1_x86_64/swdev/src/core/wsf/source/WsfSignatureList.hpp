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

#ifndef WSFSIGNATURELIST_HPP
#define WSFSIGNATURELIST_HPP

#include "wsf_export.h"

#include <string>
#include <vector>

class UtInput;
class UtScriptTypes;
class WsfPlatform;
class WsfSignature;
#include "WsfSignatureInterface.hpp"

//! Platform interface to signatures (observables).
//!
//! This class has two main purposes:
//! -) Provide the means to link the platform to its various signatures.
//! -) Provide the interface by which sensors can RAPIDLY access the signatures.
//!
//! There is an instance of this on every platform and another that is stored on the scenario. The later
//! instance is a skeleton object that is populated during scenario creation when each signature 'type
//! list' is added to the scenario. This skeleton is the initial copy that is attached to each platform.
class WSF_EXPORT WsfSignatureList
{
public:
   //! Make typing easier..
   typedef WsfSignatureInterface Interface;

   WsfSignatureList();
   WsfSignatureList(const WsfSignatureList& aSrc, WsfPlatform* aPlatformPtr);
   ~WsfSignatureList();

   void AddInterface(Interface* aInterfacePtr, size_t aIndex);

   //! Return the pointer to the interface with the specified index.
   Interface* GetInterface(size_t aIndex) { return mInterfacePtrs[aIndex]; }

   bool Initialize(double aSimTime);
   bool ProcessInput(UtInput& aInput);

   void SetState(WsfStringId aState, const std::string& aType);

#undef GetClassName // Avoid conflict with Windows macro
   std::string GetClassName(size_t aIndex) const;

   WsfStringId GetInputType(size_t aIndex) const;

   WsfStringId GetState(size_t aIndex) const;
   bool        SetState(size_t aIndex, WsfStringId aState);

   float GetScaleFactor(size_t aIndex) const;
   bool  SetScaleFactor(size_t aIndex, float aScaleFactor);

private:
   WsfPlatform*            mPlatformPtr;
   std::vector<Interface*> mInterfacePtrs;
};

#endif
