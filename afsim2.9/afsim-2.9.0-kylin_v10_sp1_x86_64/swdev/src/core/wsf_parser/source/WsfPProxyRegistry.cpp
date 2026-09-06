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

#include "WsfPProxyRegistry.hpp"

#include "UtLexicalCast.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfPProxyBasicTypes.hpp"
#include "WsfPProxyBasicValue.hpp"
#include "WsfPProxyCommon.hpp"
#include "WsfPProxyDeserialize.hpp"
#include "WsfPProxyList.hpp"
#include "WsfPProxyObjectMap.hpp"
#include "WsfPProxyStructType.hpp"
#include "WsfParseRule.hpp"
#include "WsfParseType.hpp"

WsfPProxyRegistry::WsfPProxyRegistry()
   : mApplicationPointer(nullptr)
{
}

void WsfPProxyRegistry::Setup(WsfParseStruct* aRootStructPtr, WsfParseType* aBasicTypes)
{
   mBasicTypes = aBasicTypes;

   mRootRule                         = aRootStructPtr->GetContext();
   mEntryPointRule                   = mRootRule->Find("root");
   mDataTypes["Int"]                 = ut::make_unique<WsfPProxyIntType>();
   mDataTypes["Real"]                = ut::make_unique<WsfPProxyRealType>();
   mDataTypes["Latitude"]            = ut::make_unique<WsfPProxyLatitudeType>();
   mDataTypes["Longitude"]           = ut::make_unique<WsfPProxyLongitudeType>();
   mDataTypes["Position"]            = ut::make_unique<WsfPProxyPositionType>();
   mDataTypes["Length"]              = ut::make_unique<WsfPProxyLengthType>();
   mDataTypes["Time"]                = ut::make_unique<WsfPProxyTimeType>();
   mDataTypes["Speed"]               = ut::make_unique<WsfPProxySpeedType>();
   mDataTypes["DataSize"]            = ut::make_unique<WsfPProxyDataSizeType>();
   mDataTypes["Power"]               = ut::make_unique<WsfPProxyPowerType>();
   mDataTypes["PowerDB"]             = ut::make_unique<WsfPProxyPowerDBType>();
   mDataTypes["SolidAngle"]          = ut::make_unique<WsfPProxySolidAngleType>();
   mDataTypes["Mass"]                = ut::make_unique<WsfPProxyMassType>();
   mDataTypes["Force"]               = ut::make_unique<WsfPProxyForceType>();
   mDataTypes["Torque"]              = ut::make_unique<WsfPProxyTorqueType>();
   mDataTypes["Area"]                = ut::make_unique<WsfPProxyAreaType>();
   mDataTypes["AreaDB"]              = ut::make_unique<WsfPProxyAreaDBType>();
   mDataTypes["Volume"]              = ut::make_unique<WsfPProxyVolumeType>();
   mDataTypes["Time2"]               = ut::make_unique<WsfPProxyTime2Type>();
   mDataTypes["Acceleration"]        = ut::make_unique<WsfPProxyAccelerationType>();
   mDataTypes["Frequency"]           = ut::make_unique<WsfPProxyFrequencyType>();
   mDataTypes["Angle"]               = ut::make_unique<WsfPProxyAngleType>();
   mDataTypes["AngleRate"]           = ut::make_unique<WsfPProxyAngularRateType>();
   mDataTypes["AngularAcceleration"] = ut::make_unique<WsfPProxyAngularAccelerationType>();
   mDataTypes["DataRate"]            = ut::make_unique<WsfPProxyDataRateType>();
   mDataTypes["MassDensity"]         = ut::make_unique<WsfPProxyMassDensityType>();
   mDataTypes["MassTransfer"]        = ut::make_unique<WsfPProxyMassTransferType>();
   mDataTypes["Energy"]              = ut::make_unique<WsfPProxyEnergyType>();
   mDataTypes["Fluence"]             = ut::make_unique<WsfPProxyFluenceType>();
   mDataTypes["Irradiance"]          = ut::make_unique<WsfPProxyIrradianceType>();
   mDataTypes["Ratio"]               = ut::make_unique<WsfPProxyRatioType>();
   mDataTypes["NoisePressure"]       = ut::make_unique<WsfPProxyNoisePressureType>();
   mDataTypes["Pressure"]            = ut::make_unique<WsfPProxyPressureType>();
   mDataTypes["Temperature"]         = ut::make_unique<WsfPProxyTemperatureType>();
   mDataTypes["SpecificRange"]       = ut::make_unique<WsfPProxySpecificRangeType>();
   mDataTypes["AngularInertia"]      = ut::make_unique<WsfPProxyAngularInertiaType>();
   mDataTypes["String"]              = ut::make_unique<WsfPProxyStringType>();
   mDataTypes["QuotableString"]      = ut::make_unique<WsfPProxyQuotableStringType>();
   mDataTypes["Length2"]             = ut::make_unique<WsfPProxyLength2Type>();
   mDataTypes["Length3"]             = ut::make_unique<WsfPProxyLength3Type>();
   mDataTypes["Bool"]                = ut::make_unique<WsfPProxyBoolType>();

   std::vector<WsfParseStruct*> structs;
   FindStructs(aRootStructPtr->GetContext(), structs);
   for (size_t i = 0; i < structs.size(); ++i)
   {
      GetStruct(structs[i]);
   }

   for (size_t i = 0; i < structs.size(); ++i)
   {
      WsfPProxyStructType* structPtr = GetStruct(structs[i]);
      InitializeStruct(structPtr);
   }
   for (size_t i = 0; i < structs.size(); ++i)
   {
      CreateDefaultStruct(structs[i]);
   }

   mBasicRoot = WsfPProxyValue::ConstructNew(GetStruct(aRootStructPtr));
   BuildBasicRootFromBasicTypes(mBasicRoot, mBasicTypes);
   // go through parse definition base types and instantiate into mBasicTypes
}

WsfPProxyRegistry::~WsfPProxyRegistry()
{
   mBasicRoot.Delete();
   for (auto i = mStructs.begin(); i != mStructs.end(); ++i)
   {
      i->second->mDefaultStruct.Delete();
   }
}

void WsfPProxyRegistry::BuildBasicRootFromBasicTypes(WsfPProxyValue& aOutput, WsfParseType* aInput)
{
   const WsfParseType::SymbolTable& symbols = aInput->GetSymbols();
   for (auto it = symbols.begin(); it != symbols.end(); ++it)
   {
      std::string    attrName     = it->first;
      WsfParseType*  basicTypePtr = it->second;
      WsfPProxyValue childValue   = aOutput.GetAttr(attrName);

      if (basicTypePtr->GetRule() != nullptr)
      {
         WsfPProxyStructType* structTypePtr = GetStruct(basicTypePtr->GetRule()->GetStruct());
         if (structTypePtr)
         {
            WsfPProxyValue newStruct = structTypePtr->GetDefaultValue().Copy();
            aOutput.GetObjectMap()->AddNew(attrName, newStruct);
         }
         // instantiate basicTypePtr as a child of aOutput
      }
      if (childValue.IsValid())
      {
         if (basicTypePtr->HasSymbols())
         {
            BuildBasicRootFromBasicTypes(childValue, basicTypePtr);
         }
      }
   }
}

void WsfPProxyRegistry::InitializeStruct(WsfPProxyStructType* aStructPtr)
{
   if (aStructPtr->IsInitialized())
   {
      return;
   }

   if (aStructPtr->mBasePtr)
   {
      InitializeStruct(aStructPtr->mBasePtr);
   }

   for (auto localIt = aStructPtr->mLocalAttributes.begin(); localIt != aStructPtr->mLocalAttributes.end(); ++localIt)
   {
      if (localIt->mTypePtr && localIt->mTypePtr->IsStruct())
      {
         WsfPProxyStructType* childStruct = (WsfPProxyStructType*)(localIt->mTypePtr);
         if (!childStruct->IsInitialized())
         {
            InitializeStruct(childStruct);
         }
      }
   }

   if (!aStructPtr->IsInitialized())
   {
      aStructPtr->Initialize();
   }
}

void WsfPProxyRegistry::CreateDefaultStruct(WsfParseStruct* aStructPtr)
{
   WsfPProxyStructType* structPtr = GetStruct(aStructPtr);
   if (structPtr->mDefaultStruct)
   {
      return;
   }

   WsfParseStruct* parsePtr = aStructPtr;

   WsfPProxyStructValue defaultInst(WsfPProxyValue::ConstructNew(structPtr));

   while (parsePtr)
   {
      for (size_t i = 0; i < parsePtr->mAttributes.size(); ++i)
      {
         WsfParseAttribute&  attr         = parsePtr->mAttributes[i];
         size_t              idx          = defaultInst.GetMemberIndex(attr.mName);
         WsfPProxyType*      typePtr      = defaultInst.GetMemberType(idx);
         WsfPProxyBasicType* basicTypePtr = dynamic_cast<WsfPProxyBasicType*>(typePtr);
         if (basicTypePtr)
         {
            if (!attr.mDefault.empty())
            {
               basicTypePtr->SetValue(defaultInst.GetMember(idx), attr.mDefault);
            }
            else
            {
               basicTypePtr->SetUnset(defaultInst.GetMember(idx));
            }
         }
         else
         {
            // Recurse to make sure contained structs have a default value computed
            std::string& defaultVal = attr.mDefault;
            if (typePtr->IsObjectMap() || typePtr->IsList())
            {
               WsfPProxyType* containType = typePtr->GetContainedType(0);
               if (containType && containType->IsStruct())
               {
                  CreateDefaultStruct(((WsfPProxyStructType*)containType)->mParseStructPtr);
               }
            }
            if (typePtr->IsStruct())
            {
               CreateDefaultStruct(((WsfPProxyStructType*)typePtr)->mParseStructPtr);
            }

            // A default value for a list indicates the initial size
            if (typePtr->IsList() && !defaultVal.empty())
            {
               WsfPProxyList* listPtr  = (WsfPProxyList*)defaultInst.GetMember(idx);
               auto           listSize = ut::lexical_cast<size_t>(defaultVal);
               for (size_t i = 0; i < listSize; ++i)
               {
                  listPtr->PushBack();
               }
            }

            if ((defaultVal.empty()) && (attr.mHasDefaultValue))
            {
               typePtr->SetUnset(defaultInst.GetMember(idx));
            }
            else if (typePtr->IsStruct())
            {
               WsfPProxyStructValue defaultVal = ((WsfPProxyStructType*)typePtr)->mDefaultStruct;
               if (defaultVal)
               {
                  defaultInst.GetAtIndex(idx).Copy(defaultVal);
               }
            }
         }
      }
      parsePtr = parsePtr->GetBase();
   }
   if (!aStructPtr->mInitialActions.empty())
   {
      WsfPProxyDeserialize d(this);
      d.ExecuteInitialActions(defaultInst, aStructPtr);
   }
   structPtr->mDefaultStruct = defaultInst;
}

void WsfPProxyRegistry::FindStructs(WsfParseNamedRule* aRootPtr, std::vector<WsfParseStruct*>& aStructs)
{
   WsfParseRuleDictionary* nested = aRootPtr->GetNestedRules();
   if (nested)
   {
      for (WsfParseRuleDictionary::NameRuleMap::iterator i = nested->mNameToRule.begin(); i != nested->mNameToRule.end();
           ++i)
      {
         FindStructs(i->second, aStructs);
      }
   }
   if (aRootPtr->Type() == WsfParseRule::cSTRUCT)
   {
      aStructs.push_back((WsfParseStruct*)aRootPtr);
   }
}

WsfPProxyType* WsfPProxyRegistry::GetAttributeType(WsfParseStruct* aStructPtr, WsfParseAttribute& aAttr)
{
   WsfParseNamedRule* attrRule = aStructPtr->Find(aAttr.mType);
   if (!attrRule)
   {
      return GetType(aAttr.mType);
   }
   else if (attrRule->Type() == WsfParseRule::cSTRUCT)
   {
      return GetStruct((WsfParseStruct*)attrRule);
   }
   else if (attrRule->Type() == WsfParseRule::cVALUE)
   {
      return GetType(attrRule);
   }
   return nullptr;
}

WsfPProxyStructType* WsfPProxyRegistry::GetStruct(WsfParseStruct* aStructPtr)
{
   WsfPProxyStructType*& sPtr = mStructs[aStructPtr];
   if (!sPtr)
   {
      WsfPProxyStructType::Builder builder;
      if (aStructPtr->GetBase())
      {
         builder.SetBaseStruct(GetStruct(aStructPtr->GetBase()));
      }
      builder.SetParseStruct(aStructPtr);
      for (auto& attr : aStructPtr->mAttributes)
      {
         builder.AddAttribute(attr.mName, GetAttributeType(aStructPtr, attr));
      }
      auto proxyStruct              = builder.build();
      sPtr                          = proxyStruct.get();
      mDataTypes[aStructPtr->mName] = std::move(proxyStruct);
      sPtr->mTypeName               = aStructPtr->GetFullName();
   }
   return sPtr;
}

WsfPProxyType* WsfPProxyRegistry::GetType(const std::string& aType)
{
   auto iter = mDataTypes.find(aType);
   if (iter != mDataTypes.end())
   {
      return iter->second.get();
   }
   WsfPProxyType* newTypePtr = nullptr;
   if (aType.length() > 5 && aType.substr(0, 5) == "List/")
   {
      std::string    dataType    = aType.substr(5);
      WsfPProxyType* dataTypePtr = GetType(dataType);
      newTypePtr                 = new WsfPProxyListType(dataTypePtr);
   }
   else if (aType.length() > 10 && aType.substr(0, 10) == "ObjectMap/")
   {
      std::string    dataType    = aType.substr(10);
      WsfPProxyType* dataTypePtr = GetType(dataType);
      newTypePtr                 = new WsfPProxyObjectMapType(dataTypePtr);
   }
   else
   {
      WsfParseStruct* structPtr = mRootRule->FindStruct(aType);
      if (structPtr)
      {
         return GetStruct(structPtr);
      }
   }
   if (newTypePtr)
   {
      newTypePtr->mTypeName = aType;
      mDataTypes[aType]     = std::unique_ptr<WsfPProxyType>(newTypePtr);
      return newTypePtr;
   }
   // TODO: gracefully handle errors
   // assert(false);
   // return GetType("Int");
   return nullptr;
}

WsfPProxyType* WsfPProxyRegistry::GetType(WsfParseNamedRule* aRulePtr)
{
   std::string name = aRulePtr->GetFullName();
   if (mDataTypes.find(name) == mDataTypes.end())
   {
      if (aRulePtr->Type() == WsfParseRule::cVALUE)
      {
         WsfParseEnumeration* enumRulePtr = dynamic_cast<WsfParseEnumeration*>(aRulePtr);
         if (enumRulePtr)
         {
            return GetEnum(enumRulePtr->GetFullName(), enumRulePtr->mValues);
         }
      }
      return GetType(name);
   }
   return mDataTypes[name].get();
}

WsfPProxyType* WsfPProxyRegistry::GetType(WsfParseValueType* aTypePtr)
{
   if (aTypePtr->mValuePtr)
   {
      return GetType(aTypePtr->mValuePtr);
   }
   if (aTypePtr->mStructPtr)
   {
      return GetType(aTypePtr->mStructPtr);
   }
   return GetType(aTypePtr->GetTypeName());
}

WsfPProxyType* WsfPProxyRegistry::FindBasicType(const WsfParseTypePath& aPath)
{
   WsfParseType* parseTypePtr = mBasicTypes->FindType(aPath);
   if (parseTypePtr)
   {
      return GetType((WsfParseNamedRule*)parseTypePtr->GetRule());
   }
   return nullptr;
}

WsfPProxyEnumType* WsfPProxyRegistry::GetEnum(const std::string& aFullName, WsfParseEnumerationValues& aValues)
{
   WsfPProxyEnumType* et = new WsfPProxyEnumType;
   et->mTypeName         = aFullName;
   for (size_t i = 0; i < aValues.mNameValues.size(); ++i)
   {
      std::string  text        = aValues.mNameValues[i].first;
      int          value       = aValues.mNameValues[i].second;
      std::string& firstString = et->mIntToString[value];
      if (firstString.empty())
      {
         firstString = text;
      }
      et->mStringToInt[text] = value;
   }
   mDataTypes[aFullName] = std::unique_ptr<WsfPProxyEnumType>(et);
   return et;
}

void WsfPProxyRegistry::PrintProxy(WsfPProxyValue aRoot, WsfPProxyValue aValue, ut::log::MessageStream& aStream) const
{
   WsfPProxyList*       list = aValue.GetList();
   WsfPProxyObjectMap*  map  = aValue.GetObjectMap();
   WsfPProxyStructValue inst(aValue);
   if ((inst.IsValid()) && (!inst.IsUnset()))
   {
      auto           out      = aStream.AddNote() << "Struct: " << inst.GetType()->GetTypeName();
      WsfPProxyPath* basePath = inst.GetBase();
      if (basePath)
      {
         out.AddNote() << "Base: " << basePath->ToString(aRoot);
      }
      size_t memberCount = inst.GetMemberCount();
      for (size_t i = 0; i < memberCount; ++i)
      {
         auto note = out.AddNote() << inst.GetMemberName(i) << ": ";
         PrintProxy(aRoot, inst.GetAtIndex(i), note);
      }
   }
   else if (list)
   {
      auto out = aStream.AddNote() << "List:" << ((list->Size() == 0) ? " (empty)" : "");
      for (size_t i = 0; i < list->Size(); ++i)
      {
         auto note = out.AddNote() << "[" << i << "]: ";
         PrintProxy(aRoot, list->Get(i), note);
      }
   }
   else if (map)
   {
      auto& vals = map->GetValues();
      auto  out  = aStream.AddNote() << "Map:" << (vals.empty() ? " (empty)" : "");
      for (auto i = vals.begin(); i != vals.end(); ++i)
      {
         auto note = out.AddNote() << "[" << i->first << "]: ";
         PrintProxy(aRoot, i->second, note);
      }
   }
   else if (aValue.IsBasicType())
   {
      if (!aValue.IsUnset())
      {
         WsfPProxyBasicValue bt(aValue);
         aStream.AddNote() << "Value: " << bt.ToString();
      }
      else
      {
         aStream.AddNote() << "Value: (unset)";
      }
   }
}

class WsfPProxyJSON_Writer
{
public:
   int            mFlags;
   WsfPProxyValue mRoot;
   std::ostream*  mStream;
   std::ostream&  s() { return *mStream; }

   bool IncludeValue(WsfPProxyValue aVal)
   {
      if (!aVal)
      {
         return false;
      }
      if ((mFlags & WsfProxy::cJO_HIDE_BASIC_VALUES) && aVal.IsBasicType())
      {
         return false;
      }
      if (mFlags & WsfProxy::cJO_HIDE_UNSET_VALUES && aVal.IsUnset())
      {
         return false;
      }
      if (mFlags & WsfProxy::cJO_HIDE_INHERITED_VALUES && aVal.IsInherited())
      {
         return false;
      }
      return true;
   }

   void Indent(int aIndent) { s() << std::string(aIndent, ' '); }

   // Print out the json string, with special chars escaped
   void Str(const std::string& aValue)
   {
      s() << '"';
      for (size_t i = 0; i < aValue.size(); ++i)
      {
         char c = aValue[i];
         switch (c)
         {
         case '\\':
            s() << "\\\\";
            break;
         case '\t':
            s() << "\\t";
            break;
         case '\n':
            s() << "\\n";
            break;
         case '\r':
            s() << "\\r";
            break;
         case '\f':
            s() << "\\f";
            break;
         case '\b':
            s() << "\\b";
            break;
         case '"':
            s() << "\\\"";
            break;
         default:
            s() << c;
         }
      }
      s() << '"';
   }
   void Walk(WsfPProxyValue aVal, int aIndent)
   {
      if (mFlags & WsfProxy::cJO_SHOW_ADDRESSES)
      {
         s() << " [@" << aVal.GetDataPtr() << "] ";
      }
      // Formatting:
      //    The stream is already at the desired location for this value to be printed.
      //    At exit, the stream should either be on the same line, or a subsequent line after the specified indent level
      if (aVal.IsUnset())
      {
         s() << "null";
      }
      else if (aVal.IsStruct())
      {
         bool                 needComma = false;
         WsfPProxyStructValue inst      = aVal;
         if ((mFlags & WsfProxy::cJO_SHOW_INHERITANCE) && aVal.IsInherited())
         {
            s() << " (inherited)";
         }
         s() << '\n';
         Indent(aIndent);
         s() << "{\n";
         WsfPProxyPath* basePath = inst.GetBase();
         if (basePath && !basePath->Empty())
         {
            Indent(aIndent + 2);
            needComma = true;
            Str("base_type");
            s() << ": ";
            Str(basePath->ToString(mRoot));
         }
         size_t members = inst.GetMemberCount();

         for (size_t i = 0; i < members; ++i)
         {
            WsfPProxyValue val = inst.GetAtIndex(i);
            if (IncludeValue(val))
            {
               if (needComma)
               {
                  s() << ",\n";
               }
               Indent(aIndent + 2);
               Str(inst.GetMemberName(i));
               s() << ": ";
               Walk(val, aIndent + 2);
               needComma = true;
            }
         }
         s() << "\n";
         Indent(aIndent);
         s() << "}";
      }
      else if (aVal.IsBasicType())
      {
         WsfPProxyBasicValue basicVal(aVal);
         if (basicVal.GetType())
         {
            switch (basicVal.GetType()->mTypeKind)
            {
            case WsfProxy::cBOOL_VALUE:
            case WsfProxy::cINT_VALUE:
            case WsfProxy::cDOUBLE_VALUE:
               s() << basicVal.ToString(); // Don't need quotes around types JS supports
               break;
            default:
               Str(basicVal.ToString());
            }
            if ((mFlags & WsfProxy::cJO_SHOW_INHERITANCE) && basicVal.IsInherited())
            {
               s() << " (inherited)";
            }
         }
         else
         {
            // This branch shouldn't be hit
            Str(WsfPProxyBasicValue(aVal).ToString());
         }
      }
      else if (aVal.GetList())
      {
         WsfPProxyList* listPtr = aVal.GetList();
         if (listPtr->Size() == 0)
         {
            s() << "[]";
         }
         else
         {
            s() << "[\n";
            bool needComma = false;
            for (size_t i = 0; i < listPtr->Size(); ++i)
            {
               if (needComma)
               {
                  s() << ',';
               }
               Indent(aIndent + 2);
               needComma = true;
               Walk(listPtr->Get(i), aIndent + 2);
            }
            s() << "\n";
            Indent(aIndent);
            s() << "]";
         }
      }
      else if (aVal.GetObjectMap())
      {
         s() << '\n';
         Indent(aIndent);
         s() << "{\n";
         WsfPProxyObjectMap*                          mapPtr    = aVal.GetObjectMap();
         const std::map<std::string, WsfPProxyValue>& vals      = mapPtr->GetValues();
         bool                                         needComma = false;
         for (std::map<std::string, WsfPProxyValue>::const_iterator i = vals.begin(); i != vals.end(); ++i)
         {
            if (needComma)
            {
               s() << ",\n";
            }
            Indent(aIndent + 2);
            Str(i->first);
            s() << ": ";
            Walk(i->second, aIndent + 2);
            needComma = true;
         }
         s() << "\n";
         Indent(aIndent);
         s() << "}";
      }
   }
};

void WsfPProxyRegistry::OutputJSON(WsfPProxyValue aVal, WsfPProxyValue aRoot, std::ostream& aStream, int aOutputOptions)
{
   WsfPProxyJSON_Writer w;
   w.mFlags  = aOutputOptions;
   w.mRoot   = aRoot;
   w.mStream = &aStream;
   w.Walk(aVal, 0);
}
