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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFPPROXY_HPP
#define WSFPPROXY_HPP

#include <functional>
#include <memory>

#include "UtMemoryDebug.hpp"
#include "WsfPProxyIndex.hpp"
#include "WsfPProxyRegistry.hpp"
#include "WsfPProxyType.hpp"
#include "WsfPProxyUndo.hpp"
#include "WsfPProxyValue.hpp"

class WsfPProxyNode;

// Contains data pertaining to the proxy tree
class WSF_PARSER_EXPORT WsfPProxy
{
public:
   enum ProxyModifiedReason
   {
      // An immediate child has been added
      cPM_CHILD_ADDED,
      // An immediate child has been removed
      cPM_CHILD_REMOVED,
      // An immediate child has been renamed
      cPM_CHILD_MOVED,
      // Value at path1 has been changed directly or one of its children has been changed
      cPM_VALUE_CHANGED
   };
   using ProxyModifiedFn = std::function<void(WsfPProxy*, ProxyModifiedReason, const WsfPProxyPath&, const std::string*)>;
   using BeforeModifyFn = std::function<void(WsfPProxy*, ProxyModifiedReason, const WsfPProxyPath&, const std::string*)>;
   WsfPProxy();
   WsfPProxy(const WsfPProxy&) = delete;
   WsfPProxy& operator=(const WsfPProxy&) = delete;
   ~WsfPProxy();

   void SetProxyModifiedCallback(ProxyModifiedFn aFunctionPtr) { mModifiedCallback = aFunctionPtr; }
   void SetBeforeProxyModifiedCallback(BeforeModifyFn aFunctionPtr) { mBeforeModifiedCallback = aFunctionPtr; }

   void BuildPathMap();
   bool GetChildrenPaths(const WsfPProxyPath& nodePath, WsfPProxyPathSet& childrenPaths, WsfPProxyPath& suffixPath) const;
   const WsfPProxyPathSet* GetInheritedValuePaths(const WsfPProxyPath& nodePath) const;

   void RecordObjectRename(const WsfPProxyPath& aOldPath, const WsfPProxyPath& aNewPath);

   void NotifyProxyModified(ProxyModifiedReason aProxyModifiedReason, const WsfPProxyPath& aPath, const std::string* aRenameName)
   {
      if (!mModifiedCallback)
      {
         return;
      }
      mModifiedCallback(this, aProxyModifiedReason, aPath, aRenameName);
   }

   void NotifyBeforeProxyModified(ProxyModifiedReason  aProxyModifiedReason,
                                  const WsfPProxyPath& aPath,
                                  const std::string*   aRenameName)
   {
      if (!mBeforeModifiedCallback)
      {
         return;
      }
      mBeforeModifiedCallback(this, aProxyModifiedReason, aPath, aRenameName);
   }

   void                                        UndoPlatformRenames();
   short                                       SequenceNumber() const { return mProxySequenceNumber; }
   void                                        IncrementSequenceNumber() { ++mProxySequenceNumber; }
   void                                        SetSaveFile(const WsfPProxyPath& aPath, const std::string& aFilePath);
   const std::map<WsfPProxyPath, std::string>& SaveFiles() const { return mSaveToFile; }
   std::map<WsfPProxyPath, std::string>        RenameMappingOldToNew();
   size_t         FindAttributeIndex(const std::string& aStructName, const std::string& aAttributeName);
   WsfPProxyValue NewValue(const std::string& aTypeName);
   void           PropagateNodeChange(const WsfPProxyNode& aModifiedNode);
   void           PropagateNodeAddition(const WsfPProxyNode& aAddedNode);
   void           PropagateNodeDeletion(const WsfPProxyNode& aRemovedNode);
   void           Swap(WsfPProxy& aProxy);

   const std::shared_ptr<WsfPProxyRegistry>& GetRegistry() const { return mRegistry; }
   void Registry(const std::shared_ptr<WsfPProxyRegistry>& aRegistry) { mRegistry = aRegistry; }

   using RenameMap = std::map<WsfPProxyPath, WsfPProxyPath>;

   // Indicates the proxy has modifications made by the application after parsing
   // the input files
   bool mHasModifications;
   // A mirror of 'root', but contains only basic types, like WSF_RADAR_SENSOR
   WsfPProxyStructValue mBasicRoot;
   // The root of the proxy tree containing data translated from the input file
   WsfPProxyValue mRoot;
   // Optional.  Indexes file location to proxy data paths
   std::unique_ptr<WsfPProxyIndex> mIndex;

   // maintains a mapping between base type path and derived type paths
   // for quick lookup of derived types
   std::unique_ptr<WsfPProxyPathMap> mPathMap;
   // function pointer to be invoked when the proxy has been modified
   ProxyModifiedFn mModifiedCallback;
   // function pointer to be invoked before a change to the proxy is made
   BeforeModifyFn mBeforeModifiedCallback;
   // key:   path to newly named object
   // value: path to old name
   RenameMap mRenamedObjects;

private:
   // If a new proxy root is loaded, this number is incremented to indicate the change
   short mProxySequenceNumber;

   // Proxy data types are registered here
   std::shared_ptr<WsfPProxyRegistry> mRegistry;

   // Given a proxy path, this indicates the file to save to
   std::map<WsfPProxyPath, std::string> mSaveToFile;

   UT_MEMORY_DEBUG_MARKER(cMDB_PROXY);
};

#endif
