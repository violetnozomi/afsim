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

#ifndef WSFSIGNATURE_HPP
#define WSFSIGNATURE_HPP

#include "wsf_export.h"

#include <algorithm>
#include <iterator>

class UtInput;
#include "WsfObject.hpp"
class WsfPlatform;

//! This is the abstract base class for all platform 'signatures'.
//!
//! A 'signature' is a platform attribute that is used to define the susceptibility of the platform
//! to being detected by a sensor.
class WSF_EXPORT WsfSignature : public WsfObject
{
public:
   WsfSignature();
   WsfSignature(const WsfSignature& aSrc);
   WsfSignature& operator=(const WsfSignature&) = delete;
   ~WsfSignature() override                     = default;

   virtual bool InitializeType();

   virtual bool Initialize(double aSimTime, WsfPlatform* aPlatformPtr);

   virtual bool IsA_ValidState(WsfStringId aId) const;

   //! get the list of all valid states belonging to the signature
   virtual std::vector<WsfStringId> GetStateNames() const { return {"default"}; }

   bool ProcessInput(UtInput& aInput) override;

   //! indicates that a signature can be queried without a valid mPlatform
   bool RequiresPlatform() const { return mRequiresPlatform; }

protected:
   WsfPlatform* mPlatformPtr;
   double       mBaseTime;
   bool         mRequiresPlatform{false};

   //! Many derived signatures use the same logic in obtaining state names, but their state classes and data containers
   //! are distinct This template also uses "pointer-to-member" to access the name via different state classes.
   template<class SignatureState, WsfStringId SignatureState::*IdMember, typename Iterator>
   static std::vector<WsfStringId> GetStateNamesP(Iterator aBegin, Iterator aEnd)
   {
      std::vector<WsfStringId> names;
      std::transform(aBegin, aEnd, std::back_inserter(names), [](const SignatureState& state) { return state.*IdMember; });
      return names;
   }
};

#endif
