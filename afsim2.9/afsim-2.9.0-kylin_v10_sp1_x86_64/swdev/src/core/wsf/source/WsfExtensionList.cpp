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

#include "WsfExtensionList.hpp"

#include "UtLog.hpp"

// ================================================================================================
//! Adds an extension to the list.
//! This class assumes ownership of aExtensionPtr.
//! @throws WsfDuplicateExtension if an extension with that name already exists.
void WsfExtensionList::AddExtension(const std::string& aName, std::unique_ptr<WsfExtension> aExtensionPtr)
{
   auto result = mExtensions.emplace(aName, std::move(aExtensionPtr));
   if (!result.second)
   {
      throw WsfDuplicateExtension(aName);
   }
   mExtensionOrder.push_back(aName);
}

// ================================================================================================
//! Return a pointer to the extension with the specified name.
//! @param aName The name of the extension to be located.
//! @returns The pointer to the extension, or nullptr if the extension could not be found.
WsfExtension* WsfExtensionList::FindExtension(const std::string& aName) const
{
   auto i = mExtensions.find(aName);
   if (i != mExtensions.end())
   {
      return i->second.get();
   }
   return nullptr;
}

// ================================================================================================
//! Return a reference to the extension with the specified name.
//! @param aName The name of the extension to be located.
//! @returns A reference to the extension.
//! @throws WsfUnknownExtension if the extension could not be found.
WsfExtension& WsfExtensionList::GetExtension(const std::string& aName) const
{
   auto i = mExtensions.find(aName);
   if (i != mExtensions.end())
   {
      return *i->second;
   }
   throw WsfUnknownExtension(aName);
}

// ================================================================================================
//! Returns a list of extensions in the order of initialization
std::vector<WsfExtension*> WsfExtensionList::GetExtensionsInOrder() const
{
   std::vector<WsfExtension*> extensions;
   for (const auto& ext : mExtensionOrder)
   {
      extensions.push_back(FindExtension(ext));
   }
   return extensions;
}

// ================================================================================================
//! Add a dependency between an extension and another extension.
void WsfExtensionList::AddDependency(const std::string& aExtensionName,
                                     const std::string& aDependsOnExtensionName,
                                     bool               aRequired)
{
   Dependency dep = {aDependsOnExtensionName, aRequired};
   mDependencies.emplace_back(aExtensionName, dep);
}

// ================================================================================================
//! Sort the extensions while accounting for dependencies.
void WsfExtensionList::SortExtensions()
{
   // sort extensions by dependencies using a stable topological sort
   std::set<std::string>    allExtensions(mExtensionOrder.begin(), mExtensionOrder.end());
   std::vector<std::string> newOrder;
   std::set<std::string>    included, adding;
   for (const auto& name : mExtensionOrder)
   {
      if (included.find(name) == included.end())
      {
         if (!SortExtensionsP(allExtensions, newOrder, included, adding, name, true))
         {
            throw WsfException("Extension sort error");
         }
      }
   }
   mExtensionOrder = newOrder;
}

// ================================================================================================
//! Recursive method to do a topological sort on extensions given the dependencies
bool WsfExtensionList::SortExtensionsP(const std::set<std::string>& aAllExtensionNames,
                                       std::vector<std::string>&    aNewOrder,
                                       std::set<std::string>&       aIncludedExtensions,
                                       std::set<std::string>&       aExtensionsAdding,
                                       const std::string&           aExtensionToAdd,
                                       bool                         aRequired)
{
   if (aExtensionsAdding.find(aExtensionToAdd) != aExtensionsAdding.end())
   {
      // found loop!
      auto out = ut::log::error() << "Circular extension dependency.";
      out.AddNote() << "Dependency: " << aExtensionToAdd;
      return false;
   }
   if (aIncludedExtensions.find(aExtensionToAdd) != aIncludedExtensions.end())
   {
      return true;
   }
   if (aAllExtensionNames.find(aExtensionToAdd) == aAllExtensionNames.end())
   {
      if (aRequired)
      {
         auto out = ut::log::error() << "Extension dependency missing.";
         out.AddNote() << "Dependency: " << aExtensionToAdd;
      }
      return !aRequired;
   }
   aExtensionsAdding.insert(aExtensionToAdd);
   for (const auto& depend : mDependencies)
   {
      if (depend.first == aExtensionToAdd)
      {
         const Dependency& dep = depend.second;
         if (!SortExtensionsP(aAllExtensionNames, aNewOrder, aIncludedExtensions, aExtensionsAdding, dep.mExtensionName, dep.mRequired))
         {
            return false;
         }
      }
   }
   aExtensionsAdding.erase(aExtensionToAdd);
   aIncludedExtensions.insert(aExtensionToAdd);
   aNewOrder.push_back(aExtensionToAdd);
   return true;
}

// ================================================================================================
WsfDuplicateExtension::WsfDuplicateExtension(const std::string& aName)
   : WsfException("Duplicate extension: " + aName)
{
}

// ================================================================================================
WsfUnknownExtension::WsfUnknownExtension(const std::string& aName)
   : WsfException("Unknown extension: " + aName)
{
}
