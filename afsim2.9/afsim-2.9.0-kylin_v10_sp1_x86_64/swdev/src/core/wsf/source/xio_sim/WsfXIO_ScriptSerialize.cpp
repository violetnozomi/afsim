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

#include "xio_sim/WsfXIO_ScriptSerialize.hpp"

#include <map>

#include "PakSerializeImpl.hpp"
#include "UtScriptClass.hpp"
#include "UtScriptData.hpp"
#include "UtScriptRef.hpp"
#include "UtScriptTypes.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfLocalTrack.hpp"
#include "script/WsfScriptManager.hpp"
#include "xio/WsfXIO_Defines.hpp"

// This code implements serialization for script objects
// Currently supports a limited subset of application objects
class WsfXIO_ScriptSerializeP
{
public:
   enum
   {
      cTYPE_TRACK,
      cTYPE_LOCAL_TRACK,
      cTYPE_TRACK_ID,
      cTYPE_GEOPOINT,
      cTYPE_ARRAY
   };

   static void Initialize();

   template<typename AR>
   static void Load(AR& aAr, UtScriptData& aData)
   {
      ut::script::Data::Type type;
      aAr&                   PakSerialization::Enum(type);
      LoadData(aAr, aData, type);
   }
   template<typename AR>
   static void Save(AR& aAr, UtScriptData& aData)
   {
      ut::script::Data::Type type = aData.GetType();
      aAr&                   PakSerialization::Enum(type);
      SaveData(aAr, aData);
   }

   template<typename AR>
   static void SerializeAppObject(AR& aAr, void*& aAppObjectPtr, int aTypeEnum)
   {
      switch (aTypeEnum)
      {
      case cTYPE_TRACK:
         aAr&(WsfTrack*&)aAppObjectPtr;
         break;
      case cTYPE_LOCAL_TRACK:
         aAr&(WsfLocalTrack*&)aAppObjectPtr;
         break;
      case cTYPE_TRACK_ID:
         aAr&(WsfTrackId*&)aAppObjectPtr;
         break;
      case cTYPE_GEOPOINT:
         aAr&(WsfGeoPoint*&)aAppObjectPtr;
         break;
      case cTYPE_ARRAY:
         aAr&(std::vector<UtScriptData>*&)aAppObjectPtr;
         break;
      default:
      {
         int* emptyData(nullptr);
         aAr& emptyData;
         break;
      }
      }
   }

   template<typename AR>
   static void SaveData(AR& aAr, UtScriptData& aData)
   {
      switch (aData.GetType())
      {
      case ut::script::Data::Type::cBOOL:
         aAr& aData.GetBool();
         break;
      case ut::script::Data::Type::cSTRING:
         aAr& aData.GetString();
         break;
      case ut::script::Data::Type::cDOUBLE:
         aAr& aData.GetDouble();
         break;
      case ut::script::Data::Type::cINT:
         aAr& aData.GetInt();
         break;
      case ut::script::Data::Type::cPOINTER:
      {
         UtScriptRef*   refPtr         = aData.GetPointer();
         UtScriptClass* scriptClassPtr = refPtr->GetScriptClass();
         void*          appObjectPtr   = refPtr->GetAppObject();
         if (scriptClassPtr != nullptr)
         {
            int enumId;
#undef GetClassName
            std::string objectType = scriptClassPtr->GetClassName();
            bool        canSend    = GetTypeEnum(objectType, enumId);
            aAr&        objectType;
            if (canSend)
            {
               SerializeAppObject(aAr, appObjectPtr, enumId);
            }
            else
            {
               appObjectPtr = nullptr;
               SerializeAppObject(aAr, appObjectPtr, -1);
            }
         }
         else
         {
            aAr& WsfStringId();
            SerializeAppObject(aAr, appObjectPtr, -1);
         }
      }
      default:
         break;
      }
   }

   template<typename AR>
   static void LoadData(AR& aAr, UtScriptData& aData, ut::script::Data::Type aType)
   {
      switch (aType)
      {
      case ut::script::Data::Type::cBOOL:
      {
         bool data(false);
         aAr& data;
         aData.SetBool(data);
      }
      break;
      case ut::script::Data::Type::cSTRING:
      {
         std::string data;
         aAr&        data;
         aData.SetString(data);
      }
      break;
      case ut::script::Data::Type::cDOUBLE:
      {
         double data(0.0);
         aAr&   data;
         aData.SetDouble(data);
      }
      break;
      case ut::script::Data::Type::cINT:
      {
         int  data(0);
         aAr& data;
         aData.SetInt(data);
      }
      break;
      case ut::script::Data::Type::cPOINTER:
      {
         std::string    objectType;
         aAr&           objectType;
         void*          appObjectPtr   = nullptr;
         UtScriptClass* scriptClassPtr = WsfScriptManager::GetTypes().GetClass(objectType);
         int            enumId(-1);
         GetTypeEnum(objectType, enumId);
         SerializeAppObject(aAr, appObjectPtr, enumId);
         aData.SetPointer(new UtScriptRef(appObjectPtr, scriptClassPtr, UtScriptRef::cMANAGE));
      }
      default:
         break;
      }
   }

private:
   static bool GetTypeEnum(std::string& aType, int& aEnum);

   static std::map<std::string, int> mTypeToEnum;
};

std::map<std::string, int> WsfXIO_ScriptSerializeP::mTypeToEnum;

void WsfXIO_ScriptSerializeP::Initialize()
{
   mTypeToEnum["WsfTrack"]      = cTYPE_TRACK;
   mTypeToEnum["WsfLocalTrack"] = cTYPE_LOCAL_TRACK;
   mTypeToEnum["WsfTrackId"]    = cTYPE_TRACK_ID;
   mTypeToEnum["WsfGeoPoint"]   = cTYPE_GEOPOINT;
   mTypeToEnum["Array"]         = cTYPE_ARRAY;
}

bool WsfXIO_ScriptSerializeP::GetTypeEnum(std::string& aType, int& aEnum)
{
   std::map<std::string, int>::iterator iter = mTypeToEnum.find(aType);
   if (iter != mTypeToEnum.end())
   {
      aEnum = iter->second;
      return true;
   }
   // Container types will need to have a special case
   if (aType.substr(0, 6) == "Array<")
   {
      aType = WsfStringId("Array");
      aEnum = cTYPE_ARRAY;
      return true;
   }
   return false;
}

void WsfXIO_ScriptSerialize::Initialize()
{
   WsfXIO_ScriptSerializeP::Initialize();
}

void Load(PakI& aAr, UtScriptData& aScriptData)
{
   WsfXIO_ScriptSerializeP::Load(aAr, aScriptData);
}
void Save(PakO& aAr, UtScriptData& aScriptData)
{
   WsfXIO_ScriptSerializeP::Save(aAr, aScriptData);
}
