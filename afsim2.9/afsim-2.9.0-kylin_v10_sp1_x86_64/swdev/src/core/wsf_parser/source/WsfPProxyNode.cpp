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

#include "WsfPProxyNode.hpp"

#include "WsfPProxyBasicValue.hpp"
#include "WsfPProxyIndex.hpp"
#include "WsfPProxyList.hpp"
#include "WsfPProxyObjectMap.hpp"
#include "WsfPProxyStructType.hpp"
#include "WsfParseNode.hpp"

WsfPProxyNode::WsfPProxyNode()
   : mIsCurrent(false)
   , mIsOfBasicType(false)
   , mProxySequenceNumber(-1)
   , mProxy(nullptr)
{
}

WsfPProxyNode::WsfPProxyNode(const WsfPProxyNode& aSrc)
   : mIsCurrent(aSrc.mIsCurrent)
   , mIsOfBasicType(aSrc.mIsOfBasicType)
   , mProxySequenceNumber(aSrc.mProxySequenceNumber)
   , mPath(aSrc.mPath)
   , mProxy(aSrc.mProxy)
   , mValue(aSrc.mValue)
{
}

WsfPProxyNode::WsfPProxyNode(WsfPProxy* aProxy, const WsfPProxyPath& aPath)
   : mIsCurrent(false)
   , mIsOfBasicType(false)
   , mProxySequenceNumber(-1)
   , mPath(aPath)
   , mProxy(aProxy)
{
}

WsfPProxyNode::WsfPProxyNode(WsfPProxy* aProxy, const WsfPProxyPath& aPath, size_t aSubPathLength)
   : mIsCurrent(false)
   , mIsOfBasicType(false)
   , mProxySequenceNumber(-1)
   , mPath(aPath, aSubPathLength)
   , mProxy(aProxy)
{
}
WsfPProxyNode::WsfPProxyNode(WsfPProxy* aProxy, const WsfPProxyPath& aPath, WsfPProxyValue aValue)
   : mIsCurrent(aProxy != nullptr)
   , mIsOfBasicType(false)
   , mProxySequenceNumber(aProxy ? aProxy->SequenceNumber() : -1)
   , mPath(aPath)
   , mProxy(aProxy)
   , mValue(aValue)
{
}

WsfPProxyNode WsfPProxyNode::FromProxy(WsfPProxy* aProxy)
{
   return WsfPProxyNode(aProxy, WsfPProxyPath());
}

WsfPProxyValue WsfPProxyNode::GetRoot() const
{
   return mProxy->mRoot;
}

WsfPProxyValue WsfPProxyNode::GetValue() const
{
   if (!mProxy)
   {
      return mValue;
   }
   if (!mIsCurrent || mProxy->SequenceNumber() != mProxySequenceNumber)
   {
      mProxySequenceNumber = mProxy->SequenceNumber();
      mValue               = mProxy->mRoot.Lookup(mPath);
      mIsOfBasicType       = !mValue.IsValid();
      if (mIsOfBasicType)
      {
         mValue = mProxy->mBasicRoot.Lookup(mPath);
      }
      mIsCurrent = true;
   }

   return mValue;
}

WsfPProxyNode WsfPProxyNode::GetParent() const
{
   WsfPProxyNode p(*this);
   p.mIsCurrent = false;
   p.mPath.Pop();
   return p;
}

// Retrieves the node that this node inherits from.
WsfPProxyNode WsfPProxyNode::GetInherited() const
{
   WsfPProxyNode inheritedValue;
   WsfPProxyPath curPath(mPath);
   WsfPProxyPath suffixPath;

   while (!curPath.Empty())
   {
      WsfPProxyNode  curNode(mProxy, curPath);
      WsfPProxyPath* curBasePath = curNode.GetValue().GetBasePath();
      if (curBasePath)
      {
         WsfPProxyPath checkPath = *curBasePath + suffixPath;
         WsfPProxyNode checkNode(mProxy, checkPath);
         if (!checkNode.IsNull() && (checkNode.GetValue().GetType() == GetValue().GetType()))
         {
            inheritedValue = checkNode;
            break;
         }
      }
      suffixPath = WsfPProxyPath(curPath.Back()) + suffixPath;
      curPath.Pop();
   }

   return inheritedValue;
}

const WsfPProxyPath& WsfPProxyNode::GetPath() const
{
   return mPath;
}

std::string WsfPProxyNode::GetPathString() const
{
   if (mProxy)
   {
      return mPath.ToString(mProxy->mRoot);
   }
   return "";
}

// Creates a proxy path that will remain valid even if a new grammar file is loaded
// This is accomplished by storing strings for struct members instead of integer offsets
// The resulting path is not in standard form, and will be incompatible with most methods
WsfPProxyPath WsfPProxyNode::GetStablePath() const
{
   WsfPProxyPath  stable;
   WsfPProxyValue root = mProxy->mRoot;
   for (size_t i = 0; i < mPath.size(); ++i)
   {
      WsfPProxyKey key = mPath[i];
      if (root.IsStruct())
      {
         stable += root.GetType()->GetNameAtIndex(key.GetIndex());
      }
      else
      {
         // maps key by strings, lists key by integers
         stable += key;
      }
      root = root.GetAttr(key);
   }
   return stable;
}

WsfPProxyPath WsfPProxyNode::GetInheritedPath() const
{
   return GetInherited().GetPath();
}

const WsfPProxyPath* WsfPProxyNode::GetTypePath() const
{
   const WsfPProxyPath* typePath = nullptr;
   WsfPProxyNode        curNode  = *this;

   while ((typePath == nullptr) && !curNode.IsNull())
   {
      const WsfPProxyPath* basePath = curNode.GetValue().GetBasePath();
      if ((basePath != nullptr) && !basePath->Empty())
      {
         typePath = basePath;
      }
      else
      {
         curNode = curNode.GetInherited();
      }
   }

   return typePath;
}

// Returns the 'base' path.  If not null, this indicates the proxy path this object directly inherits from.
// If null, this object my indirectly inherit through its parent
const WsfPProxyPath* WsfPProxyNode::GetBasePath() const
{
   WsfPProxyValue val = GetValue();
   return val.GetBasePath();
}

std::string WsfPProxyNode::GetName() const
{
   if (!mPath.Empty() && mPath.Back().IsString())
   {
      return mPath.Back().GetMapKey();
   }
   return std::string();
}
// Same as getName(), except that this works for struct attributes too
std::string WsfPProxyNode::GetAttrName() const
{
   if (mPath.Empty())
   {
      return "";
   }
   if (mPath.Back().IsString())
   {
      return mPath.Back().GetMapKey();
   }
   const WsfPProxyType* typePtr = GetParent().GetValue().GetType();
   if (typePtr && typePtr->IsStruct())
   {
      return typePtr->GetNameAtIndex(mPath.Back().GetIndex());
   }
   return "";
}

std::string WsfPProxyNode::GetInheritedName() const
{
   std::string         inheritedName;
   const WsfPProxyPath path = GetInherited().GetPath();

   if (!path.Empty() && path.Back().IsString())
   {
      inheritedName = path.Back().GetMapKey();
   }

   return inheritedName;
}

std::string WsfPProxyNode::GetTypeName() const
{
   std::string          typeName;
   const WsfPProxyPath* path = GetTypePath();

   if (path && !path->Empty() && path->Back().IsString())
   {
      typeName = path->Back().GetMapKey();
   }

   return typeName;
}

std::vector<std::string> WsfPProxyNode::GetInheritedTypeNames() const
{
   std::vector<std::string>   typeNames;
   const WsfPProxyType*       pType   = GetValue().GetType();
   const WsfPProxyStructType* pStruct = dynamic_cast<const WsfPProxyStructType*>(pType);

   while (pStruct != nullptr)
   {
      typeNames.push_back(pStruct->mTypeName);
      pStruct = pStruct->mBasePtr;
   }

   return typeNames;
}

UtTextDocumentRange WsfPProxyNode::GetSourceDocumentRange() const
{
   return GetFirstDocumentRange();
}

UtTextDocumentRange WsfPProxyNode::GetFirstDocumentRange() const noexcept
{
   WsfPProxyIndexNode* proxyIndexNode = FindIndexNode();
   WsfPProxyNode       baseProxy{GetInherited()};
   // Search the type hierarchy for this index node
   while (baseProxy && !proxyIndexNode)
   {
      proxyIndexNode = baseProxy.FindIndexNode();
      baseProxy      = baseProxy.GetInherited();
   }
   // This proxy node has a valid index node
   if (proxyIndexNode && !proxyIndexNode->mEntries.empty())
   {
      const WsfPProxyIndexEntry& proxyIndexEntry = proxyIndexNode->mEntries.front();
      const WsfParseNode*        parseNode       = proxyIndexEntry.mNodePtr;
      if (parseNode && parseNode->GetParent())
      {
         WsfParseNode*              parent       = parseNode->GetParent();
         const UtTextDocumentRange& subtreeRange = parent->SubtreeRange();
         return subtreeRange;
      }
   }
   return UtTextDocumentRange{};
}

UtTextDocumentRange WsfPProxyNode::GetLastDocumentRange() const noexcept
{
   WsfPProxyIndexNode* proxyIndexNode = FindIndexNode();
   WsfPProxyNode       baseProxy{GetInherited()};
   // Search the type hierarchy for this index node
   while (baseProxy && !proxyIndexNode)
   {
      proxyIndexNode = baseProxy.FindIndexNode();
      baseProxy      = baseProxy.GetInherited();
   }
   // This proxy node has a valid index node
   if (proxyIndexNode && !proxyIndexNode->mEntries.empty())
   {
      const WsfPProxyIndexEntry& proxyIndexEntry = proxyIndexNode->mEntries.back();
      const WsfParseNode*        parseNode       = proxyIndexEntry.mNodePtr;
      if (parseNode && parseNode->GetParent())
      {
         WsfParseNode*              parent       = parseNode->GetParent();
         const UtTextDocumentRange& subtreeRange = parent->SubtreeRange();
         return subtreeRange;
      }
   }
   return UtTextDocumentRange();
}

// Returns the text that defines this node from the wsf core source file.
std::string WsfPProxyNode::GetSourceText() const
{
   const UtTextDocumentRange& docRange = GetSourceDocumentRange();
   if (docRange.Valid())
   {
      const std::string& sourceText = docRange.Text();
      return sourceText;
   }
   return std::string();
}

// Returns whether or not the inherit bit is set on the node's value.
bool WsfPProxyNode::IsInherited() const
{
   return GetValue().IsInherited();
}

// Returns whether or not the inherit bit is set and, if it's a struct,
// whether it also does not have a base path. Structs always have the inherit
// bit set, so we check for a base path to its type. If it has no base path,
// it means it's inheriting from a parent.
bool WsfPProxyNode::IsInheritedFromParent() const
{
   const WsfPProxyValue& value    = GetValue();
   const bool            isStruct = value.IsStruct();

   return IsInherited() && (!isStruct || (isStruct && !value.GetBasePath()));
}

void WsfPProxyNode::Invalidate()
{
   mValue     = WsfPProxyValue();
   mIsCurrent = false;
}

bool WsfPProxyNode::SwitchToBasicTypes()
{
   // Remember the original value
   const bool alreadyBasicType = mIsOfBasicType;

   // Avoid changing the value if already pointing to the respective value in the basic proxy tree
   if (!alreadyBasicType && (mProxy != nullptr))
   {
      mValue               = mProxy->mBasicRoot.Lookup(mPath);
      mProxySequenceNumber = mProxy->SequenceNumber();
      mIsOfBasicType       = true;
      mIsCurrent           = true;
   }

   // This operation is a success if and only if this node was not pointing to the value in the basic proxy tree and the
   // value is valid
   return !alreadyBasicType && mValue.IsValid();
}

// Inverse of SwitchToBasicTypes()
bool WsfPProxyNode::SwitchToUserTypes()
{
   // Remember the original value
   const bool alreadyUserType = !mIsOfBasicType;

   // Avoid changing the value if already pointing to the respective value in the complete proxy tree
   if (!alreadyUserType && (mProxy != nullptr))
   {
      mValue               = mProxy->mRoot.Lookup(mPath);
      mProxySequenceNumber = mProxy->SequenceNumber();
      mIsOfBasicType       = false;
      mIsCurrent           = true;
   }

   // This operation is a success if and only if this node was not pointing to the value in the complete proxy tree and
   // the value is valid
   return !alreadyUserType && mValue.IsValid();
}

bool WsfPProxyNode::UpdateParentPath(const WsfPProxyPath& oldParentPath, const WsfPProxyPath& newParentPath)
{
   WsfPProxyPath oldPath = mPath;
   WsfPProxyPath suffix;

   while (!oldPath.Empty() && oldParentPath != oldPath)
   {
      suffix = WsfPProxyPath(oldPath.Back()) + suffix;
      oldPath.Pop();
   }

   mPath = newParentPath + suffix;
   Invalidate();

   return !oldPath.Empty();
}

void WsfPProxyNode::ResetToInheritedValue()
{
   const WsfPProxyValue inheritedValue = GetInherited().GetValue();

   NotifyBeforeValueChanged();
   GetValue().Copy(inheritedValue);
   GetValue().SetInherited(true);
   NotifyValueChanged();
}

WsfPProxyNode& WsfPProxyNode::operator+=(size_t aIndex)
{
   mPath += aIndex;
   if (mIsCurrent)
   {
      mValue = mValue.GetAtIndex(aIndex);
   }
   return *this;
}

WsfPProxyNode& WsfPProxyNode::operator+=(const std::string& aStr)
{
   GetValue();

   if (mIsCurrent)
   {
      if (mPath.Push(mValue, aStr))
      {
         mValue = mValue.GetAttr(mPath.Back());
      }
      else
      {
         mValue = WsfPProxyValue();
      }
   }
   else
   {
      mPath += aStr;
   }

   return *this;
}

WsfPProxyNode& WsfPProxyNode::operator+=(const WsfPProxyPath& aSuffixPath)
{
   mPath += aSuffixPath;
   mIsCurrent = false;
   return *this;
}

WsfPProxyNode& WsfPProxyNode::operator+=(const WsfPProxyKey& aEntry)
{
   GetValue();
   if (mIsCurrent)
   {
      mValue = mValue.GetAttr(aEntry);
   }
   mPath += aEntry;
   return *this;
}

WsfPProxyNode& WsfPProxyNode::operator+=(const WsfParseTypePath& aParseTypePath)
{
   for (size_t i = 0; i < aParseTypePath.size(); ++i)
   {
      (*this) += aParseTypePath[i].Get();
   }
   return *this;
}

WsfPProxyNode WsfPProxyNode::operator+(const WsfPProxyKey& aEntry) const
{
   WsfPProxyNode p(*this);
   p += aEntry;
   return p;
}

WsfPProxyNode WsfPProxyNode::operator+(size_t aEntryIndex) const
{
   WsfPProxyNode p(*this);
   p += aEntryIndex;
   return p;
}

WsfPProxyNode WsfPProxyNode::operator+(const std::string& aEntry) const
{
   WsfPProxyNode n(*this);
   n += aEntry;
   return n;
}

WsfPProxyNode WsfPProxyNode::operator+(const char* aEntry) const
{
   return *this + std::string(aEntry);
}

WsfPProxyNode WsfPProxyNode::operator+(const WsfParseTypePath& aParseTypePath) const
{
   return WsfPProxyNode(*this) += aParseTypePath;
}

// Only for values in WsfPProxy::mBasicRoot.
bool WsfPProxyNode::IsOfBasicType() const noexcept
{
   return mIsOfBasicType;
}

// Only for predefined WsfProxyBasicTypes.
bool WsfPProxyNode::IsA_BasicType() const noexcept
{
   if (!mProxy)
   {
      return false;
   }

   if (!GetValue().IsValid())
   {
      return false;
   }

   mIsOfBasicType = GetValue().GetType()->IsBasicType();
   return mIsOfBasicType;
}

// Only for WsfProxyStructTypes.
bool WsfPProxyNode::IsA_StructType() const noexcept
{
   if (!mProxy)
   {
      return false;
   }

   if (!GetValue().IsValid())
   {
      return false;
   }

   return GetValue().GetType()->IsStruct();
}

// Only for WsfProxyListTypes.
bool WsfPProxyNode::IsA_ListType() const noexcept
{
   if (!mProxy)
   {
      return false;
   }

   if (!GetValue().IsValid())
   {
      return false;
   }

   return GetValue().GetType()->IsList();
}

// Only for WsfProxyObjectMapTypes.
bool WsfPProxyNode::IsA_MapType() const noexcept
{
   if (!mProxy)
   {
      return false;
   }

   if (!GetValue().IsValid())
   {
      return false;
   }

   return GetValue().GetType()->IsObjectMap();
}

// Only for predefined WsfProxyBasicTypes.
bool WsfPProxyNode::IsOfBasicType(const std::string& aTypeName) const noexcept
{
   if (!mProxy)
   {
      return false;
   }

   if (!GetValue().IsValid())
   {
      return false;
   }

   // Enforce correct usage of this function by dynamic_cast.
   const auto* pBasicType = dynamic_cast<const WsfPProxyBasicType*>(GetValue().GetType());
   if (!pBasicType)
   {
      return false;
   }
   return pBasicType->IsOfBasicType(aTypeName);
}

// Only for pre-defined WsfProxyStructTypes.
bool WsfPProxyNode::IsA_PredefinedStructTypeOf(const std::string& aTypeName) const noexcept
{
   if (!mProxy)
   {
      return false;
   }

   if (!GetValue().IsValid())
   {
      return false;
   }

   // Enforce correct usage of this function by dynamic_cast.
   const auto* pStructType = dynamic_cast<const WsfPProxyStructType*>(GetValue().GetType());
   if (!pStructType)
   {
      return false;
   }
   return pStructType->IsOfBasicType(aTypeName);
}

// Only for user-defined WsfProxyStructTypes (from the input).
bool WsfPProxyNode::IsOfUserType(const std::string& aTypeName) const noexcept
{
   if (!mProxy)
   {
      return false;
   }

   // Note:  This may not get deeper than the first pre-defined type.
   //        If you need to check pre-defined types, use IsA_StructTypeOf instead.
   WsfPProxyValue val         = GetValue();
   WsfPProxyPath* basePathPtr = val.GetBasePath();
   WsfPProxyValue root        = GetRoot();
   bool           isTypeOf    = false;

   while (val && basePathPtr)
   {
      if (!basePathPtr->Empty() && (basePathPtr->Back().GetMapKey() == aTypeName))
      {
         isTypeOf = true;
         break;
      }
      val = root.Lookup(*basePathPtr);
      if (val)
      {
         basePathPtr = val.GetBasePath();
      }
   }

   return isTypeOf;
}

// Only for WsfProxyStructTypes.
bool WsfPProxyNode::IsA_StructTypeOf(const std::string& aTypeName) const noexcept
{
   return IsA_PredefinedStructTypeOf(aTypeName) || IsOfUserType(aTypeName);
}

// Only for WsfProxyListTypes.
bool WsfPProxyNode::IsA_ListTypeOf(const std::string& aTypeName) const noexcept
{
   if (!mProxy)
   {
      return false;
   }

   if (!GetValue().IsValid())
   {
      return false;
   }

   // Enforce correct usage of this function by dynamic_cast.
   const auto* pListType = dynamic_cast<const WsfPProxyListType*>(GetValue().GetType());
   if (!pListType)
   {
      return false;
   }

   // The contained type could be basic or struct, so do not dynamic_cast.
   const WsfPProxyType* pContainedType{GetValue().GetType()->GetContainedType(0)};
   std::string          containedTypeName{WsfParseValueType::GetContainedTypeName(aTypeName)};

   return (pContainedType && pContainedType->IsOfBasicType(containedTypeName));
}

// Only for WsfProxyObjectMapTypes.
bool WsfPProxyNode::IsA_MapTypeOf(const std::string& aTypeName) const noexcept
{
   if (!mProxy)
   {
      return false;
   }

   if (!GetValue().IsValid())
   {
      return false;
   }

   // Enforce correct usage of this function by dynamic_cast.
   const auto* pMapType = dynamic_cast<const WsfPProxyObjectMapType*>(GetValue().GetType());
   if (!pMapType)
   {
      return false;
   }

   // The contained type could be basic or struct, so do not dynamic_cast.
   const WsfPProxyType* pContainedType{GetValue().GetType()->GetContainedType(0)};
   std::string          containedTypeName{WsfParseValueType::GetContainedTypeName(aTypeName)};

   return (pContainedType && pContainedType->IsOfBasicType(containedTypeName));
}

// Delegate to all of the IsA_TypeOf methods for determination.
bool WsfPProxyNode::IsOfType(const std::string& aTypeName) const noexcept
{
   return IsOfBasicType(aTypeName) || IsA_ListTypeOf(aTypeName) || IsA_MapTypeOf(aTypeName) ||
          IsA_PredefinedStructTypeOf(aTypeName) || IsOfUserType(aTypeName);
}

WsfPProxyNode WsfPProxyNode::Root() const
{
   return WsfPProxyNode(mProxy, WsfPProxyPath());
}

const std::set<WsfPProxyPath>* WsfPProxyNode::GetInheritedValuePaths() const
{
   return mProxy->GetInheritedValuePaths(mPath);
}

void WsfPProxyNode::SetBasePath(const WsfPProxyPath& aNewPath)
{
   WsfPProxyStructValue val(GetValue());
   if (val)
   {
      val.SetBase(aNewPath);
   }
}

bool WsfPProxyNode::operator<(const WsfPProxyNode& aRhs) const
{
   return mPath < aRhs.mPath;
}

void WsfPProxyNode::NotifyValueChanged() const
{
   assert(GetProxy());
   GetProxy()->NotifyProxyModified(WsfPProxy::cPM_VALUE_CHANGED, mPath, nullptr);
}

void WsfPProxyNode::NotifyBeforeValueChanged() const
{
   assert(GetProxy());
   GetProxy()->NotifyBeforeProxyModified(WsfPProxy::cPM_VALUE_CHANGED, mPath, nullptr);
}

void WsfPProxyNode::NotifyValueAdded() const
{
   assert(GetProxy());
   GetProxy()->NotifyProxyModified(WsfPProxy::cPM_CHILD_ADDED, mPath, nullptr);
}

void WsfPProxyNode::NotifyValueRemoved() const
{
   assert(GetProxy());
   GetProxy()->IncrementSequenceNumber();
   GetProxy()->NotifyProxyModified(WsfPProxy::cPM_CHILD_REMOVED, mPath, nullptr);
}

void WsfPProxyNode::NotifyValueMoved(const std::string& aOldName) const
{
   assert(GetProxy());
   assert(!mPath.Empty());
   assert(mPath.Back().IsString());
   WsfPProxyPath oldPath = mPath;
   oldPath.Back()        = aOldName;
   GetProxy()->NotifyProxyModified(WsfPProxy::cPM_CHILD_MOVED, oldPath, &mPath.Back().GetMapKey());
}

WsfPProxyIndexNode* WsfPProxyNode::FindIndexNode() const
{
   if (mProxy && mProxy->mIndex)
   {
      return mProxy->mIndex->Find(mPath);
   }
   return nullptr;
}

// Helper method to notify of child attribute changing
void WsfPProxyNode::NotifyAttributeChanged(const char* aAttributeName) const
{
   WsfPProxyNode attr(*this);
   attr += aAttributeName;
   attr.NotifyValueChanged();
}

void WsfPProxyNode::NotifyBeforeAttributeChanged(const char* aAttributeName) const
{
   WsfPProxyNode attr(*this);
   attr += aAttributeName;
   attr.NotifyBeforeValueChanged();
}

std::string WsfPProxyNode::ToJSON(int aOutputFlags) const
{
   WsfPProxyValue val  = GetValue();
   WsfPProxyValue root = GetRoot();
   if (mIsOfBasicType)
   {
      root = GetProxy()->mBasicRoot;
   }
   std::stringstream ss;
   GetProxy()->GetRegistry()->OutputJSON(val, root, ss, aOutputFlags);
   return ss.str();
}

void WsfPProxyNode::ChangeAttributeP(const char*           aAttrName,
                                     const WsfPProxyValue& aAttribute,
                                     void*                 aNewValuePtr,
                                     int                   aNewValueType) const
{
   const WsfPProxyType* typePtr = aAttribute.GetType();
   if (typePtr && typePtr->mTypeKind == aNewValueType)
   {
      bool valueEqual = ((const WsfPProxyBasicType*)typePtr)->Equal(aAttribute.GetDataPtr(), aNewValuePtr);
      if (!valueEqual)
      {
         WsfPProxyNode attrNode = *this + aAttrName;
         attrNode.NotifyBeforeValueChanged();
         typePtr->Copy(aAttribute.GetDataPtr(), aNewValuePtr, WsfProxy::cCOPY_ALL);
         attrNode.NotifyValueChanged();
      }
   }
}

bool WsfPProxyNode::Assign(bool aBoolVal) const
{
   WsfPProxyValue       value   = GetValue();
   const WsfPProxyType* typePtr = value.GetType();
   if (typePtr && typePtr->mTypeStoredKind == WsfProxy::cBOOL_VALUE)
   {
      WsfProxy::Bool& val = *((WsfProxy::Bool*)value.GetDataPtr());
      if (val.IsTrue() != aBoolVal)
      {
         NotifyBeforeValueChanged();
         val = aBoolVal;
         NotifyValueChanged();
         return true;
      }
      return false;
   }
   return false;
}

bool WsfPProxyNode::Assign(const WsfPProxyValue& aValue) const
{
   const bool sameType       = (GetValue().GetType() == aValue.GetType());
   bool       differentValue = true;

   WsfPProxyBasicValue rhs   = aValue;
   WsfPProxyBasicValue value = GetValue();

   if (value && rhs)
   {
      differentValue = !value.IsEqual(rhs);
   }

   const bool differentInheritance = (GetValue().IsInherited() != aValue.IsInherited());

   if (sameType && (differentValue || differentInheritance))
   {
      NotifyBeforeValueChanged();
      GetValue().Copy(aValue);
      NotifyValueChanged();
      return true;
   }

   return false;
}

bool WsfPProxyNode::Assign(const std::string& aValue) const
{
   WsfPProxyValue       value   = GetValue();
   const WsfPProxyType* typePtr = value.GetType();
   if (typePtr && typePtr->IsBasicType())
   {
      if (((const WsfPProxyBasicType*)typePtr)->ToString(value.GetDataPtr()) != aValue)
      {
         NotifyBeforeValueChanged();
         ((WsfPProxyBasicType*)typePtr)->SetValue(value.GetDataPtr(), aValue);
         NotifyValueChanged();
         return true;
      }
      return false;
   }
   return false;
}

bool WsfPProxyNode::Assign(int aIntValue) const
{
   return AssignNumericP<int>(aIntValue);
}

bool WsfPProxyNode::Assign(double aDoubleVal) const
{
   return AssignNumericP<double>(aDoubleVal);
}

bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const std::string& aValue) const
{
   WsfPProxyNode attr = *this + aAttrName;
   return attr.Assign(aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, int aIntValue) const
{
   WsfPProxyNode attr = *this + aAttrName;
   return attr.Assign(aIntValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, double aDoubleVal) const
{
   WsfPProxyNode attr = *this + aAttrName;
   return attr.Assign(aDoubleVal);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, bool aBoolVal) const
{
   WsfPProxyNode attr = *this + aAttrName;
   return attr.Assign(aBoolVal);
}

bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::String& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::Double& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::Int& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::Latitude& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::Longitude& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::Length& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::Length2& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::Time& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::Speed& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::DataSize& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::Power& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::PowerDB& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::SolidAngle& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::Mass& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::Force& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::Torque& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::Area& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::AreaDB& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::Volume& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::Time2& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::Acceleration& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::Frequency& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::Angle& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::AngularRate& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::AngularAcceleration& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::DataRate& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::MassDensity& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::MassTransfer& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::Energy& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::Fluence& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::Irradiance& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::Ratio& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::NoisePressure& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::Pressure& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::Temperature& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::SpecificRange& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::AngularInertia& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}
bool WsfPProxyNode::AssignAttribute(const std::string& aAttrName, const WsfProxy::Position& aValue) const
{
   return AssignAttributeT(aAttrName, aValue);
}

bool WsfPProxyNode::Assign(const WsfProxy::String& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::Double& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::Int& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::Latitude& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::Longitude& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::Length& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::Length2& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::Time& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::Speed& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::DataSize& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::Power& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::PowerDB& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::SolidAngle& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::Mass& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::Force& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::Torque& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::Area& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::AreaDB& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::Volume& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::Time2& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::Acceleration& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::Frequency& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::Angle& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::AngularRate& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::AngularAcceleration& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::DataRate& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::MassDensity& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::MassTransfer& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::Energy& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::Fluence& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::Irradiance& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::Ratio& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::NoisePressure& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::Pressure& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::Temperature& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::SpecificRange& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::AngularInertia& aValue) const
{
   return AssignT(aValue);
}
bool WsfPProxyNode::Assign(const WsfProxy::Position& aValue) const
{
   return AssignT(aValue);
}

void WsfPProxyNode::MoveTo(WsfPProxy* aProxy, const WsfPProxyPath& aPath, size_t aSubPathLength)
{
   mIsCurrent = mIsOfBasicType = false;
   mPath.Clear();
   size_t count = std::min(aSubPathLength, aPath.size());
   for (size_t i = 0; i < count; ++i)
   {
      mPath += aPath[i];
   }
   mProxy = aProxy;
}

bool WsfPProxyNode::Unset() const
{
   WsfPProxyValue val = GetValue();
   if (!val)
   {
      return false;
   }
   if (val.IsUnset())
   {
      return true;
   }
   NotifyBeforeValueChanged();
   GetValue().SetUnset();
   NotifyValueChanged();
   return true;
}

bool WsfPProxyNode::ClearUnset() const
{
   WsfPProxyValue val = GetValue();
   if (!val)
   {
      return false;
   }
   if (!val.IsUnset())
   {
      return true;
   }
   NotifyBeforeValueChanged();
   GetValue().ClearUnset();
   NotifyValueChanged();
   return true;
}

bool WsfPProxyNode::UnsetAttribute(const std::string& aAttrName) const
{
   WsfPProxyValue thisValue = GetValue();
   if (!thisValue)
   {
      return false;
   }
   WsfPProxyValue attrVal = thisValue.GetAttr(aAttrName);
   if (!attrVal)
   {
      return false;
   }
   if (attrVal.IsUnset())
   {
      return true;
   }
   return (*this + aAttrName).Unset();
}
