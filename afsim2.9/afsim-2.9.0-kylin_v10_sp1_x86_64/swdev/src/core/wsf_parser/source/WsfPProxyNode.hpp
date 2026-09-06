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

#ifndef WSFPPROXYNODE_HPP
#define WSFPPROXYNODE_HPP

#include "UtCompilerAttributes.hpp"
#include "UtTextDocument.hpp"
#include "WsfPProxy.hpp"
#include "WsfPProxyPath.hpp"
#include "WsfPProxyValue.hpp"

class WsfPProxyIndexNode;

/*
Undo for IDE
------------
Can't use the standard proxy undo class because it won't emit notifications for changed items on undo
   -need to emit notifications for proxy changes made while doing undo
   -need to propagate type information when doing the undo
plan:
   Make an undo construct at the proxy node level
      -notifications are available
      -path info is available
   Any time a change is made, go through these methods:
      void WsfPProxyNode::assign(const BasicType& aValue);
      void WsfPProxyNode::AssignAttribute(const std::string& aName, const BasicType& aValue);
      void WsfPM_ObjectMap::remove()
      void WsfPM_ObjectMap::add()
      void WsfPM_List::pushBack(WsfPProxyValue)
      void WsfPM_List::popBack();

   Each of which will do this:
      if can do operation:
         call WsfPProxy::beforeModify()
            (application handles to save undo data)
         apply changes to proxy
         call WsfPProxy::modify...()


*/

// Represents a position in the proxy.
// This is a combination of a proxy root and a proxy path.
// Keeps reference to the value currently pointed to by the proxy path.
// Users of this class need to call invalidate() if it is possible the value no
// longer exists in the proxy.
class WSF_PARSER_EXPORT WsfPProxyNode
{
public:
   void MoveTo(WsfPProxy* aProxy, const WsfPProxyPath& aPath, size_t aSubPathLength);

   WsfPProxyNode();
   WsfPProxyNode(WsfPProxy* aProxy, const WsfPProxyPath& aPath);
   WsfPProxyNode(WsfPProxy* aProxy, const WsfPProxyPath& aPath, size_t aSubPathLength);
   WsfPProxyNode(WsfPProxy* aProxy, const WsfPProxyPath& aPath, WsfPProxyValue aValue);
   WsfPProxyNode(const WsfPProxyNode& aSrc);

   static WsfPProxyNode FromProxy(WsfPProxy* aProxy);

   WsfPProxyNode            Root() const;
   WsfPProxy*               GetProxy() const { return mProxy; }
   WsfPProxyValue           GetRoot() const;
   WsfPProxyValue           GetValue() const;
   WsfPProxyNode            GetParent() const;
   WsfPProxyNode            GetInherited() const;
   const WsfPProxyPath&     GetPath() const;
   std::string              GetPathString() const;
   WsfPProxyPath            GetStablePath() const;
   WsfPProxyPath            GetInheritedPath() const;
   const WsfPProxyPath*     GetTypePath() const;
   const WsfPProxyPath*     GetBasePath() const;
   std::string              GetName() const;
   std::string              GetAttrName() const;
   std::string              GetInheritedName() const;
   std::string              GetTypeName() const;
   std::vector<std::string> GetInheritedTypeNames() const;
   //! Get the document range associated with this WsfPProxyNode
   //! @par Document Ranges
   //!   When the WsfParser creates WsfParseNodes, each node contains a UtTextDocumentRange corresponding to a
   //!   particular piece of AFSIM input. The WsfPProxyDeserialize process then creates WsfPProxyIndexNodes when
   //!   applying WsfParseActions to the WsfParseNodes. Each WsfPProxyIndexEntry may contain zero or more
   //!   WsfPProxyIndexEntries. These WsfPProxyIndexEntrys may contain a WsfParseNode.
   //! @return the document range
   //! @par Deprecation
   //!   GetSourceDocumentRange may be deprecated in a future release.
   //!   Please use GetFirstDocumentRange instead.
   /*CPP14_DEPRECATED*/ UtTextDocumentRange GetSourceDocumentRange() const;
   //! Get the document range contained in the first WsfPProxyIndexEntry
   //! @return the first document range
   UtTextDocumentRange GetFirstDocumentRange() const noexcept;
   //! Get the document range contained in the last WsfPProxyIndexEntry
   //! @return the last document range
   //! @note For most cases, this is what the developer will want to use.
   UtTextDocumentRange GetLastDocumentRange() const noexcept;
   std::string         GetSourceText() const;

   WsfPProxyIndexNode* FindIndexNode() const;

   const std::set<WsfPProxyPath>* GetInheritedValuePaths() const;

   //! @name Methods to determine the type of WsfProxyNode
   //@{

   //! Determines whether this WsfProxyNode points to a value in the WsfPProxy::mBasicRoot
   //! @par details
   //!   This is the accessor for mIsOfBasicType.
   //!   The mIsOfBasicType flag denotes whether this WsfProxyNode points to a value in the WsfPProxy::mBasicRoot.
   //!   Despite it denoting a basic type, the mIsOfBasicType variable <strong>DOES NOT</strong> determine whether this
   //!   WsfProxyNode is of a WsfProxyBasicType. The value of mIsOfBasicType toggles with the calling of
   //!   SwitchToBasicTypes and SwitchToUserTypes.
   //! @retval mIsOfBasicType whether this WsfProxyNode points to a value in the WsfPProxy::mBasicRoot
   //! @retval false when the developer constructs a WsfParseNode normally (assumes user-defined type, I guess...)
   //! @warning DO NOT CONFUSE THIS WITH IsA_BasicType!
   //! @see SwitchToBasicTypes
   //! @see SwitchToUserTypes
   bool IsOfBasicType() const noexcept;

   //! Determines whether this WsfProxyNode is of a WsfProxyBasicType
   //! @par details
   //!   This function checks the underlying value of this WsfProxyNode's value to see whether it is a
   //!   WsfProxyBasicType. It DOES NOT check whether this WsfProxyNode points to a value in the WsfPProxy::mBasicRoot.
   //! @return whether this WsfProxyNode is of a WsfProxyBasicType
   //! @warning DO NOT CONFUSE THIS WITH IsOfBasicType!
   bool IsA_BasicType() const noexcept;
   //! Determines whether this WsfProxyNode is of a WsfProxyStructType
   //! @return whether this WsfProxyNode is of a WsfProxyStructType
   bool IsA_StructType() const noexcept;
   //! Determines whether this WsfProxyNode is of a WsfProxyListType
   //! @return whether this WsfProxyNode is of a WsfProxyListType
   bool IsA_ListType() const noexcept;
   //! Determines whether this WsfProxyNode is of a WsfProxyObjectMapType
   //! @return whether this WsfProxyNode is of a WsfProxyObjectMapType
   bool IsA_MapType() const noexcept;

   //! Determines whether this WsfProxyNode is of the pre-defined WsfProxyBasicType of the given name
   //! @param aTypeName is the given type name
   //! @return whether this WsfProxyNode is of the pre-defined WsfProxyBasicType of the given name
   bool IsOfBasicType(const std::string& aTypeName) const noexcept;
   //! Determines whether this WsfProxyNode is of the pre-defined WsfProxyStructType of the given name
   //! @param aTypeName is the given type name
   //! @return whether this WsfProxyNode is of the pre-defined WsfProxyStructType of the given name
   bool IsA_PredefinedStructTypeOf(const std::string& aTypeName) const noexcept;
   //! Determines whether this WsfProxyNode is of the user-defined WsfProxyStructType of the given name
   //! @param aTypeName is the given type name
   //! @return whether this WsfProxyNode is of the user-defined WsfProxyStructType of the given name
   bool IsOfUserType(const std::string& aTypeName) const noexcept;
   //! Determines whether this WsfProxyNode is of the WsfProxyStructType of the given name
   //! @param aTypeName is the given type name
   //! @return whether this WsfProxyNode is of the WsfProxyStructType of the given name
   bool IsA_StructTypeOf(const std::string& aTypeName) const noexcept;
   //! Determines whether this WsfProxyNode is of the pre-defined WsfProxyListType of the given name
   //! @param aTypeName is the given type name
   //! @return whether this WsfProxyNode is of the pre-defined WsfProxyListType of the given name
   bool IsA_ListTypeOf(const std::string& aTypeName) const noexcept;
   //! Determines whether this WsfProxyNode is of the pre-defined WsfProxyObjectMapType of the given name
   //! @param aTypeName is the given type name
   //! @return whether this WsfProxyNode is of the pre-defined WsfProxyObjectMapType of the given name
   bool IsA_MapTypeOf(const std::string& aTypeName) const noexcept;
   //! Determines whether this WsfProxyNode is of the WsfProxyType of the given name
   //! @param aTypeName is the given type name
   //! @return whether this WsfProxyNode is of the WsfProxyType of the given name
   bool IsOfType(const std::string& aTypeName) const noexcept;

   //@}

   bool IsInherited() const;
   bool IsInheritedFromParent() const;

   void Invalidate();

   //! @name Switch between value trees
   //! @par details
   //!   The proxy contains two trees for WsfPProxyValues:  the basic tree and the complete tree.
   //!   The basic tree contains only basic types, such as primitives (int, real, string, et cetera) and the unit types.
   //!   The complete tree contains not only the basic types, but also the user-defined types.
   //! @see UtUnits
   //@{

   //! Switches the value to which this ProxyNode points to the respective value in the basic value tree.
   //! @pre mIsOfBasicType is false
   //! @post mIsOfBasicType is true
   bool SwitchToBasicTypes();
   //! Switches the value to which this ProxyNode points to the respective value in the complete value tree.
   //! @pre mIsOfBasicType is true
   //! @post mIsOfBasicType is false
   bool SwitchToUserTypes();

   //@}

   bool UpdateParentPath(const WsfPProxyPath& oldParentPath, const WsfPProxyPath& newParentPath);
   void ResetToInheritedValue();

   // Is the node pointing to a valid value
        operator bool() const { return GetValue(); }
        operator bool() { return GetValue(); }
   bool IsValid() const { return GetValue(); }
   bool IsNull() const { return !(bool)GetValue(); }

   bool operator==(const WsfPProxyNode& aRhs) const { return mPath == aRhs.mPath; }
   bool operator!=(const WsfPProxyNode& aRhs) const { return mPath != aRhs.mPath; }

   // Navigation methods
   // These methods access child proxy nodes
   WsfPProxyNode& operator+=(size_t aIndex);
   WsfPProxyNode& operator+=(const std::string& aStr);
   WsfPProxyNode& operator+=(const WsfPProxyPath& aSuffixPath);
   WsfPProxyNode& operator+=(const WsfPProxyKey& aEntry);
   WsfPProxyNode& operator+=(const WsfParseTypePath& aParseTypePath);
   WsfPProxyNode  operator+(const WsfPProxyPath& aSuffixPath) const { return WsfPProxyNode(*this) += aSuffixPath; }
   WsfPProxyNode  operator+(const WsfPProxyKey& aEntry) const;
   WsfPProxyNode  operator+(size_t aEntryIndex) const;
   WsfPProxyNode  operator+(const std::string& aEntry) const;
   WsfPProxyNode  operator+(const char* aEntry) const;
   WsfPProxyNode  operator+(const WsfParseTypePath& aParseTypePath) const;

   WsfPProxyNode& ToParent()
   {
      mIsCurrent = false;
      mPath.Pop();
      return *this;
   }
   WsfPProxyNode& TrimPath(size_t aPathLength)
   {
      if (mPath.size() > aPathLength)
      {
         mPath.Trim(aPathLength);
         mIsCurrent = false;
      }
      return *this;
   }
   // Comparison operator for ordering by path
   bool operator<(const WsfPProxyNode& aRhs) const;

   void SetBasePath(const WsfPProxyPath& aNewPath);

   void NotifyValueChanged() const;

   void NotifyValueAdded() const;
   void NotifyValueRemoved() const;
   void NotifyValueMoved(const std::string& aOldName) const;
   void NotifyAttributeChanged(const char* aAttributeName) const;

   void NotifyBeforeValueChanged() const;
   void NotifyBeforeAttributeChanged(const char* aAttributeName) const;

   template<typename T>
   void ChangeAttribute(const char* aAttributeName, const T& aValue)
   {
      ChangeAttributeP(aAttributeName, GetValue().GetAttr(aAttributeName), (void*)&aValue, T::cPROXY_TYPE_ID);
   }
   bool Assign(const WsfPProxyValue& aValue) const;
   bool Assign(const std::string& aValue) const;
   bool Assign(int aIntValue) const;
   bool Assign(double aDoubleVal) const;
   bool Assign(bool aBoolVal) const;
   bool ClearUnset() const;
   bool Unset() const;
   bool UnsetAttribute(const std::string& aAttrName) const;

   bool AssignAttribute(const std::string& aAttrName, const std::string& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, int aIntValue) const;
   bool AssignAttribute(const std::string& aAttrName, double aDoubleVal) const;
   bool AssignAttribute(const std::string& aAttrName, bool aBoolVal) const;
   /*       Attribute assignment
    * Assigns a value to a child attribute.  Invokes notification callbacks if available.
    * returns 'false' if the attribute remains unchanged (due to invalid type or assignment to same value)
    */
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::String& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::Double& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::Int& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::Latitude& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::Longitude& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::Length& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::Length2& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::Time& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::Speed& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::DataSize& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::Power& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::PowerDB& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::SolidAngle& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::Mass& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::Force& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::Torque& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::Area& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::AreaDB& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::Volume& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::Time2& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::Acceleration& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::Frequency& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::Angle& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::AngularRate& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::AngularAcceleration& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::DataRate& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::MassDensity& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::MassTransfer& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::Energy& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::Fluence& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::Irradiance& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::Ratio& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::NoisePressure& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::Pressure& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::Temperature& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::SpecificRange& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::AngularInertia& aValue) const;
   bool AssignAttribute(const std::string& aAttrName, const WsfProxy::Position& aValue) const;

   /*       assignment
    * Assigns a basic value to a node.  Invokes notification callbacks if available.
    * returns 'false' if the value remains unchanged (due to invalid type or assignment to same value)
    */
   bool Assign(const WsfProxy::String& aValue) const;
   bool Assign(const WsfProxy::Double& aValue) const;
   bool Assign(const WsfProxy::Int& aValue) const;
   bool Assign(const WsfProxy::Latitude& aValue) const;
   bool Assign(const WsfProxy::Longitude& aValue) const;
   bool Assign(const WsfProxy::Length& aValue) const;
   bool Assign(const WsfProxy::Length2& aValue) const;
   bool Assign(const WsfProxy::Time& aValue) const;
   bool Assign(const WsfProxy::Speed& aValue) const;
   bool Assign(const WsfProxy::DataSize& aValue) const;
   bool Assign(const WsfProxy::Power& aValue) const;
   bool Assign(const WsfProxy::PowerDB& aValue) const;
   bool Assign(const WsfProxy::SolidAngle& aValue) const;
   bool Assign(const WsfProxy::Mass& aValue) const;
   bool Assign(const WsfProxy::Force& aValue) const;
   bool Assign(const WsfProxy::Torque& aValue) const;
   bool Assign(const WsfProxy::Area& aValue) const;
   bool Assign(const WsfProxy::AreaDB& aValue) const;
   bool Assign(const WsfProxy::Volume& aValue) const;
   bool Assign(const WsfProxy::Time2& aValue) const;
   bool Assign(const WsfProxy::Acceleration& aValue) const;
   bool Assign(const WsfProxy::Frequency& aValue) const;
   bool Assign(const WsfProxy::Angle& aValue) const;
   bool Assign(const WsfProxy::AngularRate& aValue) const;
   bool Assign(const WsfProxy::AngularAcceleration& aValue) const;
   bool Assign(const WsfProxy::DataRate& aValue) const;
   bool Assign(const WsfProxy::MassDensity& aValue) const;
   bool Assign(const WsfProxy::MassTransfer& aValue) const;
   bool Assign(const WsfProxy::Energy& aValue) const;
   bool Assign(const WsfProxy::Fluence& aValue) const;
   bool Assign(const WsfProxy::Irradiance& aValue) const;
   bool Assign(const WsfProxy::Ratio& aValue) const;
   bool Assign(const WsfProxy::NoisePressure& aValue) const;
   bool Assign(const WsfProxy::Pressure& aValue) const;
   bool Assign(const WsfProxy::Temperature& aValue) const;
   bool Assign(const WsfProxy::SpecificRange& aValue) const;
   bool Assign(const WsfProxy::AngularInertia& aValue) const;
   bool Assign(const WsfProxy::Position& aValue) const;

   // see JSON_OutputOptions
   std::string ToJSON(int aOutputFlags = 0) const;

protected:
   template<typename T>
   T GetAttributeT(const std::string& aName) const
   {
      T* attrPtr = GetValue().GetAttr(aName);
      if (attrPtr)
      {
         return *attrPtr;
      }
      return T();
   }

private:
   // assign an attribute
   template<typename T>
   bool AssignAttributeT(const std::string& aAttrName, const T& aValue) const
   {
      // we could just navigate to attribute node and call assignT(), but this
      // avoids making a new node instance if the value isn't changing
      bool                 modified = false;
      WsfPProxyValue       val      = GetValue().GetAttr(aAttrName);
      const WsfPProxyType* typePtr  = val.GetType();
      if (typePtr && typePtr->mTypeStoredKind == T::cPROXY_TYPE_ID)
      {
         T& thisValue = *(T*)val.GetDataPtr();
         if (!(thisValue == aValue))
         {
            WsfPProxyNode attrNode = *this + aAttrName;
            attrNode.NotifyBeforeValueChanged();
            thisValue = aValue;
            attrNode.NotifyValueChanged();
            modified = true;
         }
      }
      return modified;
   }

   template<typename T>
   bool AssignT(const T& aValue) const
   {
      bool                 modified = false;
      WsfPProxyValue       val      = GetValue();
      const WsfPProxyType* typePtr  = val.GetType();
      if (typePtr && typePtr->mTypeStoredKind == T::cPROXY_TYPE_ID)
      {
         T& thisValue = *(T*)val.GetDataPtr();
         if (!(thisValue == aValue))
         {
            NotifyBeforeValueChanged();
            thisValue = aValue;
            NotifyValueChanged();
            modified = true;
         }
      }
      return modified;
   }

   template<typename NUMBER_TYPE>
   bool AssignNumericP(NUMBER_TYPE aValue) const
   {
      WsfPProxyValue       value    = GetValue();
      const WsfPProxyType* typePtr  = value.GetType();
      bool                 modified = false;
      if (typePtr)
      {
         switch (typePtr->mTypeStoredKind)
         {
         case WsfProxy::cINT_VALUE:
         {
            WsfProxy::Int& val = *((WsfProxy::Int*)value.GetDataPtr());
            if (val.GetValue() != static_cast<int>(aValue))
            {
               NotifyBeforeValueChanged();
               val.SetValue(static_cast<int>(aValue));
               NotifyValueChanged();
               modified = true;
            }
            break;
         }
         case WsfProxy::cDOUBLE_VALUE:
         {
            WsfProxy::Double& val = *((WsfProxy::Double*)value.GetDataPtr());
            if (val.GetValue() != static_cast<double>(aValue))
            {
               NotifyBeforeValueChanged();
               val.SetValue(static_cast<double>(aValue));
               NotifyValueChanged();
               modified = true;
            }
            break;
         }
         }
      }
      return modified;
   }

   void ChangeAttributeP(const char* aAttrName, const WsfPProxyValue& aAttribute, void* aNewValuePtr, int aNewValueType) const;

   // true if mValue points to the correct value
   mutable bool mIsCurrent;
   // true if mValue points to a value in the 'basic root'
   mutable bool mIsOfBasicType;
   // A number which should match the sequence number in mProxy
   // A number mismatch indicates the proxy has been refreshed with new data
   // and mValue should be looked up again
   mutable short mProxySequenceNumber;

protected:
   WsfPProxyPath mPath;

private:
   // Access to all proxy data
   WsfPProxy* mProxy;
   // The value pointed to by this node, when it's current
   mutable WsfPProxyValue mValue;
};


#endif
