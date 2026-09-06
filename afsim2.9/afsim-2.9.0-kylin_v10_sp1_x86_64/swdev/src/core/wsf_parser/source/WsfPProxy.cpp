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

#include "WsfPProxy.hpp"

#include "UtMemory.hpp"
#include "WsfPM_Platform.hpp"
#include "WsfPProxyIndex.hpp"
#include "WsfPProxyVisitor.hpp"

namespace
{
// Updates inherited proxy values due to base value changes
class UpdateChildNode
{
public:
   UpdateChildNode() = default;

   bool operator()(WsfPProxyNode& childNode)
   {
      WsfPProxyValue       childValue    = childNode.GetValue();
      const WsfPProxyType* childType     = childValue.GetType();
      bool                 continueVisit = childValue.IsInherited() && !childType->IsList();

      if (childValue.IsInherited() && (childType->IsBasicType() || childType->IsList()))
      {
         childValue.Copy(childNode.GetInherited().GetValue(), WsfProxy::cCOPY_INHERITED);
         childValue.SetInherited(true);
         childNode.NotifyValueChanged();
      }

      return continueVisit;
   }
};
} // namespace

WsfPProxy::WsfPProxy()
   : mHasModifications(false)
   , mIndex(nullptr)
   , mPathMap(nullptr)
   , mModifiedCallback(nullptr)
   , mBeforeModifiedCallback(nullptr)
   , mProxySequenceNumber(0)
{
}

WsfPProxy::~WsfPProxy()
{
   mRoot.Delete();
}

/// Functor for use with the WsfPProxyVisitor::Visit function. This will create
/// a map from proxy paths to their children paths. So given a path, you can
/// get the set of paths that derive/inherit from it.
struct PathMapBuilder
{
public:
   PathMapBuilder()
      : mPathMapPtr(new WsfPProxyPathMap())
   {
   }

   WsfPProxyPathMap* GetResult() const { return mPathMapPtr; }

   bool operator()(WsfPProxyNode& aNode)
   {
      const WsfPProxyPath* basePath = aNode.GetValue().GetBasePath();

      if (basePath && !basePath->Empty())
      {
         (*mPathMapPtr)[*basePath].insert(aNode.GetPath());
      }

      return true;
   }

private:
   WsfPProxyPathMap* mPathMapPtr;
};

void WsfPProxy::BuildPathMap()
{
   PathMapBuilder builder;
   WsfPProxyNode  rootNode(this, WsfPProxyPath());

   WsfPProxyVisitor::VisitNodes(builder, rootNode);

   mPathMap.reset(builder.GetResult());
}

bool WsfPProxy::GetChildrenPaths(const WsfPProxyPath& nodePath, WsfPProxyPathSet& childrenPaths, WsfPProxyPath& suffixPath) const
{
   WsfPProxyPath path = nodePath;

   WsfPProxyPathMap::const_iterator childPathsIter = mPathMap->find(path);
   while ((childPathsIter == mPathMap->end()) && !path.Empty())
   {
      suffixPath = WsfPProxyPath(path.Back()) + suffixPath;
      path.Pop();
      childPathsIter = mPathMap->find(path);
   }

   const bool found = (childPathsIter != mPathMap->end());

   if (found)
   {
      childrenPaths = childPathsIter->second;
   }

   return found;
}

const WsfPProxyPathSet* WsfPProxy::GetInheritedValuePaths(const WsfPProxyPath& nodePath) const
{
   WsfPProxyPathMap::const_iterator iter = mPathMap->find(nodePath);
   if (iter != mPathMap->end())
   {
      return &iter->second;
   }
   return nullptr;
}

// Records the fact that aOldPath was renamed to aNewPath
// Does not actually do the rename operation, but only saves information about the rename so that
// the proxy data can be serialized to the file correctly
void WsfPProxy::RecordObjectRename(const WsfPProxyPath& aOldPath, const WsfPProxyPath& aNewPath)
{
   // rename B->C
   // If B was not previously renamed:
   //       add(C=>B)
   // if B was previously renamed from A; we instead:
   //       remove(B=>A) and add(C=>A)
   WsfPProxyPath       oldPath = aOldPath;
   RenameMap::iterator oldIter = mRenamedObjects.find(aOldPath);
   if (oldIter != mRenamedObjects.end())
   {
      oldPath = oldIter->second;
      mRenamedObjects.erase(oldIter);
   }
   if (oldPath != aNewPath)
   {
      mRenamedObjects[aNewPath] = oldPath;
   }

   // If a file has been designated for the platform, update the map
   if (mSaveToFile.find(aOldPath) != mSaveToFile.end())
   {
      mSaveToFile[aNewPath] = mSaveToFile[aOldPath];
      mSaveToFile.erase(aOldPath);
   }
}

void WsfPProxy::UndoPlatformRenames()
{
   for (const auto& renameInfo : mRenamedObjects)
   {
      const WsfPProxyNode node(this, renameInfo.first);
      if (node.IsOfType("Platform"))
      {
         WsfPM_Platform plat(node);
         const auto&    oldPath = renameInfo.second;
         plat.Rename(oldPath.Back().GetMapKey());
      }
   }
}

// gets a mapping from old proxy path to new name
// this mapping is the opposite of renamedObjects
std::map<WsfPProxyPath, std::string> WsfPProxy::RenameMappingOldToNew()
{
   std::map<WsfPProxyPath, std::string> mapping;
   for (RenameMap::iterator i = mRenamedObjects.begin(); i != mRenamedObjects.end(); ++i)
   {
      assert(i->first.Back().IsString());
      mapping[i->second] = i->first.Back().GetMapKey();
   }
   return mapping;
}

// Swap the contents of this with aProxy
void WsfPProxy::Swap(WsfPProxy& aProxy)
{
   std::swap(mHasModifications, aProxy.mHasModifications);
   std::swap(mBasicRoot, aProxy.mBasicRoot);
   std::swap(mRoot, aProxy.mRoot);
   std::swap(mIndex, aProxy.mIndex);
   std::swap(mRegistry, aProxy.mRegistry);
   std::swap(mPathMap, aProxy.mPathMap);
   std::swap(mModifiedCallback, aProxy.mModifiedCallback);
   std::swap(mRenamedObjects, aProxy.mRenamedObjects);
   std::swap(mSaveToFile, aProxy.mSaveToFile);
   // the sequence numbers are incremented to indicate an update has occurred
   ++mProxySequenceNumber;
   ++aProxy.mProxySequenceNumber;
}

// Finds an attribute index in a struct
//    findAttributeIndex("Platform", "icon") = 1
size_t WsfPProxy::FindAttributeIndex(const std::string& aStructName, const std::string& aAttributeName)
{
   WsfPProxyStructType* structPtr = dynamic_cast<WsfPProxyStructType*>(mRegistry->GetType(aStructName));
   if (structPtr)
   {
      return structPtr->GetMemberIndex(aAttributeName);
   }
   return ut::npos;
}

WsfPProxyValue WsfPProxy::NewValue(const std::string& aTypeName)
{
   auto* type = mRegistry->GetType(aTypeName);
   return WsfPProxyValue::ConstructNew(type);
}

// When the value of a proxy node changes, this method will propagate that
// change to all inheriting proxy nodes.
void WsfPProxy::PropagateNodeChange(const WsfPProxyNode& aModifiedNode)
{
   WsfPProxyPathSet childrenPaths;
   WsfPProxyPath    suffixPath;

   if (GetChildrenPaths(aModifiedNode.GetPath(), childrenPaths, suffixPath))
   {
      for (WsfPProxyPathSet::const_iterator i = childrenPaths.begin(); i != childrenPaths.end(); ++i)
      {
         WsfPProxyPath childPath = *i + suffixPath;
         WsfPProxyNode childNode(this, childPath);

         if (childNode.IsValid() && childNode.GetValue().IsInherited())
         {
            UpdateChildNode childUpdater;
            WsfPProxyVisitor::VisitNodes(childUpdater, childNode);
         }
      }
   }
}

// When an object is added to an object map, this method will copy that object
// into all inheriting object maps.
//
// May possibly need to be adapted in the future to handle lists.
void WsfPProxy::PropagateNodeAddition(const WsfPProxyNode& aAddedNode)
{
   WsfPProxyPathSet childrenPaths;
   WsfPProxyPath    suffixPath;

   if (GetChildrenPaths(aAddedNode.GetPath(), childrenPaths, suffixPath))
   {
      for (WsfPProxyPathSet::const_iterator i = childrenPaths.begin(); i != childrenPaths.end(); ++i)
      {
         WsfPProxyPath childPath = *i + suffixPath;
         WsfPProxyNode childNode(this, childPath);

         assert(!childNode.IsValid());
         assert(childNode.GetParent().IsValid());

         // If the object is in the parent map, and it isn't in the child map,
         // and it isn't in the child map's deletion list, then add it to the child.
         WsfPProxyNode        parentNode = childNode.GetParent();
         const WsfPProxyType* parentType = parentNode.GetValue().GetType();

         if (parentType->IsObjectMap())
         {
            WsfPM_ObjectMap     parentMap(parentNode);
            WsfPProxyObjectMap* parentObjMap = parentMap.GetObjectMap();

            if (!parentObjMap->HasUserDeleted(childPath.Back().ToString()))
            {
               WsfPProxyValue newChildVal = aAddedNode.GetValue().Copy(WsfProxy::cCOPY_INHERITED);
               newChildVal.SetInherited(true);
               parentMap.Add(childPath.Back().ToString(), newChildVal);
            }
         }
      }
   }
}

// When an object is removed from an object map, this method will remove that
// object from all inheriting object maps.
//
// May possibly need to be adapted in the future to handle lists.
void WsfPProxy::PropagateNodeDeletion(const WsfPProxyNode& aRemovedNode)
{
   WsfPProxyPathSet childrenPaths;
   WsfPProxyPath    suffixPath;

   if (GetChildrenPaths(aRemovedNode.GetPath(), childrenPaths, suffixPath))
   {
      for (WsfPProxyPathSet::const_iterator i = childrenPaths.begin(); i != childrenPaths.end(); ++i)
      {
         WsfPProxyPath childPath = *i + suffixPath;
         WsfPProxyNode childNode(this, childPath);

         if (childNode.GetValue().IsInherited())
         {
            WsfPProxyNode        childMapNode = childNode.GetParent();
            const WsfPProxyType* childMapType = childMapNode.GetValue().GetType();

            if (childMapType->IsObjectMap())
            {
               WsfPM_ObjectMap childObjMap(childMapNode);
               childObjMap.Remove(childPath.Back().ToString());
            }
         }
      }
   }
}

void WsfPProxy::SetSaveFile(const WsfPProxyPath& aPath, const std::string& aFilePath)
{
   mSaveToFile[aPath] = aFilePath;
}
