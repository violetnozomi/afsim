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

#include "WsfDeferredInput.hpp"

#include <cassert>
#include <sstream>

#include "UtInputDocument.hpp"
#include "UtLog.hpp"

WsfDeferredInput::WsfDeferredInput()
   : mIsDelayLoading(false)
   , mRootInputPtr(nullptr)
   , mDelayLoaders()
   , mCurrentlyLoading()
   , mInputStack()
   , mCompleted()
   , mDelayLoadOrder()
{
}

void WsfDeferredInput::ClearP()
{
   mIsDelayLoading = false;
   mRootInputPtr   = nullptr;
   mDelayLoaders.clear();
   mCurrentlyLoading.clear();
   mInputStack.clear();
   mCompleted.clear();
   mDelayLoadOrder.clear();
}

// Wait until all input is read to continue reading this value.
void WsfDeferredInput::DelayLoadP(const std::string&             aKind,
                                  const std::string&             aName,
                                  const UtInput::StoredLocation& aPosition,
                                  const ProcessInputFunction&    aInputFunction)
{
   std::string name(aName);
   if (aName.empty())
   {
      // This is an unnamed object. Generate a unique name so there are no dependencies.
      std::ostringstream oss;
      oss << '#' << mDelayLoadOrder.size() + 1;
      name = oss.str();
   }
   UniqueId key(aKind, name);
   // Insert after identical entries to preserve input order when possible
   auto    iter          = mDelayLoaders.upper_bound(key);
   Loader& loader        = mDelayLoaders.insert(iter, make_pair(key, Loader()))->second;
   loader.mInputFunction = aInputFunction;
   loader.mLocation      = aPosition;
   mDelayLoadOrder.push_back(key);
}

// Ensures that the object with the specified kind and name is loaded.
// Throws an error if a cyclic dependency is found, or the required object is not found.
void WsfDeferredInput::RequiresP(const std::string& aKind, const std::string& aName)
{
   // If not in the delayed loading state of scenario input processing
   // then this method is being called outside of normal processing. This is most
   // likely to occur when using the WsfScriptObjectClass.ProcessInput script method.
   // Users will get notified they are doing something unexpected.
   if (!mIsDelayLoading)
   {
      ut::log::info() << "Unexpected deferred input processing for " << aKind << " " << aName;
   }

   UniqueId id(aKind, aName);
   if (mCompleted.find(id) != mCompleted.end())
   {
      return;
   }

   if (mCurrentlyLoading.find(id) != mCurrentlyLoading.end())
   {
      std::stringstream ss;
      ss << "Cyclic Dependency detected, trying to load " << aKind << " " << aName;
      if (!mInputStack.empty())
      {
         throw UtInput::BadValue(*mInputStack.back(), ss.str());
      }
      else
      {
         throw UtInput::BadValue(*mRootInputPtr, ss.str());
      }
   }

   mCurrentlyLoading.insert(id);

   std::vector<Loader> allLoaders;
   {
      auto i = mDelayLoaders.lower_bound(id);
      while (i != mDelayLoaders.end() && i->first == id)
      {
         allLoaders.push_back(i->second);
         ++i;
      }
   }

   if (allLoaders.empty())
   {
      std::stringstream ss;
      ss << "Could not find " << aKind << " " << aName;
      if (!mInputStack.empty())
      {
         throw UtInput::BadValue(*mInputStack.back(), ss.str());
      }
      else
      {
         throw UtInput::BadValue(*mRootInputPtr, ss.str());
      }
   }
   else
   {
      for (Loader& loader : allLoaders)
      {
         UtInput newInput;
         newInput.LinkInputs(*mRootInputPtr);
         newInput.RestoreLocation(loader.mLocation);
         mInputStack.push_back(&newInput);
         if (!loader.mInputFunction(newInput))
         {
            throw UtInput::UnknownCommand(newInput);
         }
         mInputStack.pop_back();
      }

      mCurrentlyLoading.erase(id);
      mCompleted.insert(id);
   }
}

// If the user has provided an input for the specified object, this calls Requires() to ensure that object is loaded.
// Otherwise, this method does nothing.
void WsfDeferredInput::MaybeRequires(const std::string& aKind, const std::string& aName)
{
   UniqueId id(aKind, aName);
   if (mDelayLoaders.find(id) != mDelayLoaders.end())
   {
      Requires(aKind, aName);
   }
}

// Execute all delay loaders
void WsfDeferredInput::ResolveP(UtInput& aInput)
{
   mRootInputPtr   = &aInput;
   mIsDelayLoading = true;
   for (auto& dlo : mDelayLoadOrder)
   {
      Requires(dlo.first, dlo.second);
   }
   mIsDelayLoading = false;
}

bool WsfDeferredInput::IsDelayLoadingP()
{
   return mIsDelayLoading;
}

void WsfDeferredInput::ResetDelaysP()
{
   mDelayLoadOrder.clear();
   mCompleted.clear();
   mDelayLoaders.clear();
}
