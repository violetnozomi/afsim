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

#ifndef WSFEXTENSIONLIST_HPP
#define WSFEXTENSIONLIST_HPP

#include "wsf_export.h"

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "WsfException.hpp"
#include "WsfExtension.hpp"

//! Exception thrown when attempting to get an unknown extension.
class WSF_EXPORT WsfUnknownExtension : public WsfException
{
public:
   WsfUnknownExtension(const std::string& aName);
   ~WsfUnknownExtension() override = default;
};

//! An exception that is thrown when attempting to register a duplicate extension
class WSF_EXPORT WsfDuplicateExtension : public WsfException
{
public:
   WsfDuplicateExtension(const std::string& aName);
   ~WsfDuplicateExtension() override = default;
};

//! A container maintains a list of extensions.
//! Stores dependencies between extensions, and computes an initialization order to satisfy them
//! using a topological sort.
class WSF_EXPORT WsfExtensionList
{
public:
   WsfExtensionList()                        = default;
   WsfExtensionList(const WsfExtensionList&) = delete;
   WsfExtensionList& operator=(const WsfExtensionList&) = delete;
   ~WsfExtensionList()                                  = default;

   //! An application extension dependency.
   //! Dependencies affect order of initialization.
   struct Dependency
   {
      //! The name of the dependency extension.
      std::string mExtensionName;
      //! Defines if the extension is required.
      //! when 'true', the dependency extension must exist.
      // When 'false', the dependency must be ordered first when available.
      bool mRequired;
   };

   using DependencyList = std::vector<std::pair<std::string, Dependency>>;
   using ExtensionMap   = std::map<std::string, std::unique_ptr<WsfExtension>>;
   using iterator       = ExtensionMap::iterator;
   using const_iterator = ExtensionMap::const_iterator;

   //! @name Extension list methods.
   //@{
   void AddExtension(const std::string& aName, std::unique_ptr<WsfExtension> aExtensionPtr);

   WsfExtension* FindExtension(const std::string& aName) const;
   WsfExtension& GetExtension(const std::string& aName) const;

   const std::vector<std::string>& GetExtensionOrder() const { return mExtensionOrder; }
   std::vector<WsfExtension*>      GetExtensionsInOrder() const;
   const ExtensionMap&             GetExtensions() const { return mExtensions; }
   //@}

   //! @name Dependency list methods.
   //@{
   void AddDependency(const std::string& aExtensionName, const std::string& aDependsOnExtensionName, bool aRequired);
   const DependencyList& GetDependencies() const { return mDependencies; }
   //@}

   //! @name Iterators
   //@{
   const_iterator begin() const { return mExtensions.begin(); }
   const_iterator end() const { return mExtensions.end(); }
   //@}

   //! @name Utility methods
   //@{
   void SortExtensions();
   //@}

private:
   bool SortExtensionsP(const std::set<std::string>& aAllExtensionNames,
                        std::vector<std::string>&    aNewOrder,
                        std::set<std::string>&       aIncludedExtensions,
                        std::set<std::string>&       aExtensionsAdding,
                        const std::string&           aExtensionToAdd,
                        bool                         aRequired);

   DependencyList           mDependencies;
   std::vector<std::string> mExtensionOrder;
   ExtensionMap             mExtensions;
};

#endif
