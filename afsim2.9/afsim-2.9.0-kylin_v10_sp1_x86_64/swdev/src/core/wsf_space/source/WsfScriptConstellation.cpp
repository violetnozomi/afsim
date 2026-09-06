// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptConstellation.hpp"

#include <iostream>

#include "UtCast.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfConstellation.hpp"
#include "WsfConstellationManager.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptDefs.hpp"

WsfScriptConstellation::WsfScriptConstellation(UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass("WsfConstellation", aScriptTypesPtr)
{
   SetClassName("WsfConstellation");

   AddStaticMethod(ut::make_unique<FindConstellation>());
   AddStaticMethod(ut::make_unique<AllConstellations>());
   AddMethod(ut::make_unique<IsGeneral>());
   AddMethod(ut::make_unique<IsWalkerDelta>());
   AddMethod(ut::make_unique<IsWalkerStar>());
   AddMethod(ut::make_unique<TotalSats>());
   AddMethod(ut::make_unique<NumPlanes>());
   AddMethod(ut::make_unique<SatsPerPlane>());
   AddMethod(ut::make_unique<WalkerF>());
   AddMethod(ut::make_unique<AnomalyAlias>());
   AddMethod(ut::make_unique<RAAN_Range>());
   AddMethod(ut::make_unique<ConstellationName>());
   AddMethod(ut::make_unique<PlatformType>());
   AddMethod(ut::make_unique<SemiMajorAxis>());
   AddMethod(ut::make_unique<CircularAltitude>());
   AddMethod(ut::make_unique<OrbitalPeriod>());
   AddMethod(ut::make_unique<RevolutionsPerDay>());
   AddMethod(ut::make_unique<Inclination>());
   AddMethod(ut::make_unique<InitialRAAN>());
   AddMethod(ut::make_unique<InitialAnomaly>());
   AddMethod(ut::make_unique<NextSatelliteInPlane>());
   AddMethod(ut::make_unique<PreviousSatelliteInPlane>());
   AddMethod(ut::make_unique<NextPlane>());
   AddMethod(ut::make_unique<PreviousPlane>());
   AddMethod(ut::make_unique<MemberName>());
   AddMethod(ut::make_unique<Member>());
   AddMethod(ut::make_unique<Apply>());
}

void WsfScriptConstellation::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfConstellation*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, FindConstellation, 1, "WsfConstellation", "string")
{
   WsfConstellation* constellationPtr =
      WsfConstellationManager::Get(SIMULATION).FindConstellation(aVarArgs[0].GetString());
   aReturnVal.SetPointer(new UtScriptRef(constellationPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, AllConstellations, 0, "Array<WsfConstellation>", "")
{
   std::vector<WsfConstellation*> constellations     = WsfConstellationManager::Get(SIMULATION).GetConstellations();
   auto                           retvalPtr          = ut::make_unique<std::vector<UtScriptData>>();
   UtScriptClass*                 constellationClass = aContext.GetTypes()->GetClass("WsfConstellation");
   for (auto i : constellations)
   {
      retvalPtr->push_back(UtScriptData(new UtScriptRef(i, constellationClass)));
   }
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, IsGeneral, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsGeneral());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, IsWalkerDelta, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsWalkerDelta());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, IsWalkerStar, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsWalkerStar());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, TotalSats, 0, "int", "")
{
   aReturnVal.SetInt(ut::safe_cast<int, unsigned int>(aObjectPtr->GetTotalSats()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, NumPlanes, 0, "int", "")
{
   aReturnVal.SetInt(ut::safe_cast<int, unsigned int>(aObjectPtr->GetNumPlanes()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, SatsPerPlane, 0, "int", "")
{
   aReturnVal.SetInt(ut::safe_cast<int, unsigned int>(aObjectPtr->GetSatsPerPlane()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, WalkerF, 0, "int", "")
{
   aReturnVal.SetInt(ut::safe_cast<int, unsigned int>(aObjectPtr->GetWalkerF()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, AnomalyAlias, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetAnomalyAlias().GetAsUnit(UtUnitAngle::cDEGREES));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, RAAN_Range, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRAAN_Range().GetAsUnit(UtUnitAngle::cDEGREES));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, ConstellationName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, PlatformType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetPlatformType());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, SemiMajorAxis, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetSemiMajorAxis().GetAsUnit(UtUnitLength::cMETERS));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, CircularAltitude, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetCircularAltitude().GetAsUnit(UtUnitLength::cMETERS));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, OrbitalPeriod, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetOrbitalPeriod().GetAsUnit(UtUnitTime::cSECONDS));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, RevolutionsPerDay, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRevolutionsPerDay());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, Inclination, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetInclination().GetAsUnit(UtUnitAngle::cDEGREES));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, InitialRAAN, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetInitialRAAN().GetAsUnit(UtUnitAngle::cDEGREES));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, InitialAnomaly, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetInitialAnomaly().GetAsUnit(UtUnitAngle::cDEGREES));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, NextSatelliteInPlane, 1, "int", "int")
{
   int satellite = aVarArgs[0].GetInt() + 1;
   if (satellite >= ut::safe_cast<int, unsigned int>(aObjectPtr->GetSatsPerPlane()))
   {
      satellite = 0;
   }
   aReturnVal.SetInt(satellite);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, PreviousSatelliteInPlane, 1, "int", "int")
{
   int satellite = aVarArgs[0].GetInt() - 1;
   if (satellite < 0)
   {
      satellite = ut::safe_cast<int, unsigned int>(aObjectPtr->GetSatsPerPlane()) - 1;
   }
   aReturnVal.SetInt(satellite);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, NextPlane, 1, "int", "int")
{
   int plane = aVarArgs[0].GetInt() + 1;
   if (plane >= ut::safe_cast<int, unsigned int>(aObjectPtr->GetNumPlanes()))
   {
      plane = 0;
   }
   aReturnVal.SetInt(plane);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, PreviousPlane, 1, "int", "int")
{
   int plane = aVarArgs[0].GetInt() - 1;
   if (plane < 0)
   {
      plane = ut::safe_cast<int, unsigned int>(aObjectPtr->GetNumPlanes()) - 1;
   }
   aReturnVal.SetInt(plane);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, MemberName, 2, "string", "int, int")
{
   bool ok{true};

   int plane = aVarArgs[0].GetInt();
   if (plane < 0 || plane >= ut::safe_cast<int, unsigned int>(aObjectPtr->GetNumPlanes()))
   {
      auto logger = ut::log::error() << "WsfConstellation.GetMemberName: provided plane must be in range.";
      logger.AddNote() << "Range: 0 - " << aObjectPtr->GetNumPlanes();
      logger.AddNote() << "Provided: " << plane;
      ok = false;
   }

   int member = aVarArgs[1].GetInt();
   if (member < 0 || member >= ut::safe_cast<int, unsigned int>(aObjectPtr->GetSatsPerPlane()))
   {
      auto logger = ut::log::error() << "WsfConstellation.GetMemberName: provided member must be in range.";
      logger.AddNote() << "Range: 0 - " << aObjectPtr->GetSatsPerPlane();
      logger.AddNote() << "Provided: " << member;
      ok = false;
   }

   if (ok)
   {
      aReturnVal.SetString(
         aObjectPtr->GetMemberName(ut::safe_cast<unsigned int, int>(plane), ut::safe_cast<unsigned int, int>(member)));
   }
   else
   {
      aReturnVal.SetNullString();
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, Member, 2, "WsfPlatform", "int, int")
{
   bool ok{true};

   int plane = aVarArgs[0].GetInt();
   if (plane < 0 || plane >= ut::safe_cast<int, unsigned int>(aObjectPtr->GetNumPlanes()))
   {
      auto logger = ut::log::error() << "WsfConstellation.GetMemberName: provided plane must be in range.";
      logger.AddNote() << "Range: 0 - " << aObjectPtr->GetNumPlanes();
      logger.AddNote() << "Provided: " << plane;
      ok = false;
   }

   int member = aVarArgs[1].GetInt();
   if (member < 0 || member >= ut::safe_cast<int, unsigned int>(aObjectPtr->GetSatsPerPlane()))
   {
      auto logger = ut::log::error() << "WsfConstellation.GetMemberName: provided member must be in range.";
      logger.AddNote() << "Range: 0 - " << aObjectPtr->GetSatsPerPlane();
      logger.AddNote() << "Provided: " << member;
      ok = false;
   }

   WsfPlatform* platformPtr{nullptr};
   if (ok)
   {
      platformPtr =
         aObjectPtr->GetMember(ut::safe_cast<unsigned int, int>(plane), ut::safe_cast<unsigned int, int>(member));
   }
   aReturnVal.SetPointer(new UtScriptRef(platformPtr, aReturnClassPtr));
}

namespace // {anonymous}
{

UtScript* ValidateScriptForApply(const std::string& aScriptName, WsfScriptContext& aContext)
{
   UtScript* retval{nullptr};
   if (aScriptName.empty())
   {
      ut::log::error() << "WsfConstellation.Apply: Empty script name provided.\n";
   }
   else
   {
      UtScript* applyScriptPtr = aContext.FindScript(aScriptName);
      if (applyScriptPtr != nullptr)
      {
         if (aContext.ValidateScript(applyScriptPtr, "void", "WsfConstellation, int, int, WsfPlatform"))
         {
            retval = applyScriptPtr;
         }
         else
         {
            auto logger = ut::log::error() << "WsfConstellation.Apply: Cannot be used in Apply. ";
            logger.AddNote() << "Script Name: " << aScriptName;
            logger.AddNote() << "The required signature is void (WsfConstellation, int, int, WsfPlatform).";
         }
      }
      else
      {
         auto logger = ut::log::error()
                       << "WsfConstellation.Apply: There is no script in the global context with the specified name.";
         logger.AddNote() << "Script Name: " << aScriptName;
      }
   }
   return retval;
}

} // namespace

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellation, WsfConstellation, Apply, 1, "bool", "string")
{
   std::string       scriptName     = aVarArgs[0].GetString();
   WsfScriptContext& context        = SIMULATION->GetScriptContext();
   UtScript*         applyScriptPtr = ValidateScriptForApply(scriptName, context);
   bool              retval         = applyScriptPtr != nullptr;

   if (retval)
   {
      unsigned int numPlanes       = aObjectPtr->GetNumPlanes();
      unsigned int numSatsPerPlane = aObjectPtr->GetSatsPerPlane();
      for (unsigned int plane = 0; plane < numPlanes; ++plane)
      {
         for (unsigned int sat = 0; sat < numSatsPerPlane; ++sat)
         {
            WsfPlatform* memberPtr = aObjectPtr->GetMember(plane, sat);
            if (memberPtr != nullptr)
            {
               UtScriptDataList scriptArgs;
               scriptArgs.emplace_back(new UtScriptRef(aObjectPtr, context.GetClass("WsfConstellation")));
               scriptArgs.emplace_back(ut::safe_cast<int, unsigned int>(plane));
               scriptArgs.emplace_back(ut::safe_cast<int, unsigned int>(sat));
               scriptArgs.emplace_back(new UtScriptRef(memberPtr, context.GetClass("WsfPlatform")));

               UtScriptData scriptRetval;
               context.ExecuteScript(TIME_NOW, applyScriptPtr, scriptRetval, scriptArgs);
            }
         }
      }
   }

   aReturnVal.SetBool(retval);
}
