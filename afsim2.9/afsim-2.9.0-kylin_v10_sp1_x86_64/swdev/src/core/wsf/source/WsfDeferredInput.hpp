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

#ifndef WSFDEFERREDINPUT_HPP
#define WSFDEFERREDINPUT_HPP

#include "wsf_export.h"

#include <functional>
#include <map>
#include <set>
#include <string>

#include "UtInput.hpp"

//! Contains input utilities for WSF.
//!
//! WSF does 'delayed' loading for many types of objects.
//! When delay loading, an object's kind and name are scanned, remembered,
//! and bypassed until a later time.
//!
//! For instance:
//!   sensor MySensor MySensorTYPE
//!      ...
//!   end_sensor
//! Here, 'sensor MySensor' is saved using WsfDeferredInput::DelayLoad(), and then the
//! entire block is skipped until all initial input processing has been completed.
//! Finally, WsfDeferredInput::Resolve() finishes reading the delay load blocks.
//! If an object depends on another object at input-time, a call to
//! WsfDeferredInput::Requires() ensures that dependency is met before input processing continues.
//!
//! Any object which has input-time dependencies should use the delay load feature.
class WSF_EXPORT WsfDeferredInput
{
public:
   using UniqueId             = std::pair<std::string, std::string>;
   using ProcessInputFunction = std::function<bool(UtInput&)>;

   WsfDeferredInput();

   //! Returns true if the delay load phase is active.  In this phase, types that were
   //! skipped during the first pass are finally processed.
   bool IsDelayLoading() { return IsDelayLoadingP(); }

   // Wait until all input is read to continue reading this value.
   void DelayLoad(const std::string&             aKind,
                  const std::string&             aName,
                  const UtInput::StoredLocation& aPosition,
                  const ProcessInputFunction&    aInputFunction)
   {
      return DelayLoadP(aKind, aName, aPosition, aInputFunction);
   }

   // Ensures that the object with the specified kind and name is loaded.
   // Throws an error if a cyclic dependency is found, or the required object is not found.
   void Requires(const std::string& aKind, const std::string& aName) { RequiresP(aKind, aName); }

   void MaybeRequires(const std::string& aKind, const std::string& aName);

   // Execute all delay loaders
   void Resolve(UtInput& aInput) { ResolveP(aInput); }

   void Clear() { ClearP(); }

   void ResetDelays() { ResetDelaysP(); }

   struct Loader
   {
      Loader()  = default;
      ~Loader() = default;
      UtInput::StoredLocation mLocation;
      ProcessInputFunction    mInputFunction;
   };

protected:
   bool IsDelayLoadingP();

   // Wait until all input is read to continue reading this value.
   void DelayLoadP(const std::string&             aKind,
                   const std::string&             aName,
                   const UtInput::StoredLocation& aPosition,
                   const ProcessInputFunction&    aInputFunction);

   // Ensures that the object with the specified kind and name is loaded.
   // Throws an error if a cyclic dependency is found, or the required object is not found.
   void RequiresP(const std::string& aKind, const std::string& aName);

   // Execute all delay loaders
   void ResolveP(UtInput& aInput);

   void ResetDelaysP();

   void ClearP();

   using LoaderMap = std::multimap<UniqueId, Loader>;
   bool                  mIsDelayLoading;
   UtInput*              mRootInputPtr;
   LoaderMap             mDelayLoaders;
   std::set<UniqueId>    mCurrentlyLoading;
   std::vector<UtInput*> mInputStack;
   std::set<UniqueId>    mCompleted;
   std::vector<UniqueId> mDelayLoadOrder;
};

#endif
