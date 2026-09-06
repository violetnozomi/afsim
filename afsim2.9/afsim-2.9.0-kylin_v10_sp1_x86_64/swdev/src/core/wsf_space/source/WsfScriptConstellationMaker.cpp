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

#include "WsfScriptConstellationMaker.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include "UtCast.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtPath.hpp"
#include "UtScriptPath.hpp"
#include "WsfConstellationMaker.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptDefs.hpp"

WsfScriptConstellationMaker::WsfScriptConstellationMaker(UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass("WsfConstellationMaker", aScriptTypesPtr)
{
   SetClassName("WsfConstellationMaker");

   AddStaticMethod(ut::make_unique<CreateWalkerDelta>());
   AddStaticMethod(ut::make_unique<CreateWalkerStar>());
   AddStaticMethod(ut::make_unique<CreateGeneral>());

   AddMethod(ut::make_unique<WriteToFile>());
   AddMethod(ut::make_unique<Create_1>("Create"));
   AddMethod(ut::make_unique<Create_2>("Create"));
   AddMethod(ut::make_unique<DetectConjunction>());

   AddMethod(ut::make_unique<IsSetup>());
   AddMethod(ut::make_unique<Validate>());
   AddMethod(ut::make_unique<IsWalkerDelta>());
   AddMethod(ut::make_unique<IsWalkerStar>());
   AddMethod(ut::make_unique<IsGeneral>());
   AddMethod(ut::make_unique<TotalSatellites>());
   AddMethod(ut::make_unique<Planes>());
   AddMethod(ut::make_unique<SatellitesPerPlane>());
   AddMethod(ut::make_unique<WalkerF>());
   AddMethod(ut::make_unique<AnomalyAlias>());
   AddMethod(ut::make_unique<RAAN_Range>());

   AddMethod(ut::make_unique<SemiMajorAxis>());
   AddMethod(ut::make_unique<Altitude>());
   AddMethod(ut::make_unique<RevolutionsPerDay>());
   AddMethod(ut::make_unique<Period>());
   AddMethod(ut::make_unique<Inclination>());
   AddMethod(ut::make_unique<InitialRAAN>());
   AddMethod(ut::make_unique<InitialAnomaly>());
   AddMethod(ut::make_unique<ConstellationName>());
   AddMethod(ut::make_unique<PlatformType>());
   AddMethod(ut::make_unique<BasePath>());

   AddMethod(ut::make_unique<SetSemiMajorAxis>());
   AddMethod(ut::make_unique<SetAltitude>());
   AddMethod(ut::make_unique<SetRevolutionsPerDay>());
   AddMethod(ut::make_unique<SetPeriod>());
   AddMethod(ut::make_unique<SetInclination>());
   AddMethod(ut::make_unique<SetInitialRAAN>());
   AddMethod(ut::make_unique<SetInitialAnomaly>());
   AddMethod(ut::make_unique<SetConstellationName>());
   AddMethod(ut::make_unique<SetPlatformType>());
   AddMethod(ut::make_unique<SetBasePath>());
}

void WsfScriptConstellationMaker::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfConstellationMaker*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, CreateWalkerDelta, 3, "WsfConstellationMaker", "int, int, int")
{
   int  totalArg  = aVarArgs[0].GetInt();
   int  planesArg = aVarArgs[1].GetInt();
   int  fArg      = aVarArgs[2].GetInt();
   bool takeAction{false};
   if (totalArg <= 0)
   {
      auto logger = ut::log::error() << "Constellation must have positive number of members.";
      logger.AddNote() << "Provided: " << totalArg;
   }
   else if (planesArg <= 0)
   {
      auto logger = ut::log::error() << "Constellation must have positive number of planes.";
      logger.AddNote() << "Provided: " << planesArg;
   }
   else if (fArg < 0)
   {
      auto logger = ut::log::error() << "Constellation must have non-negative f parameter.";
      logger.AddNote() << "Provided: " << fArg;
   }
   else
   {
      takeAction = true;
   }

   std::unique_ptr<WsfConstellationMaker> retval{nullptr};
   if (takeAction)
   {
      retval = WsfConstellationMaker::CreateWalkerDelta(ut::safe_cast<unsigned int>(totalArg),
                                                        ut::safe_cast<unsigned int>(planesArg),
                                                        ut::safe_cast<unsigned int>(fArg));
   }
   aReturnVal.SetPointer(new UtScriptRef(retval.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, CreateWalkerStar, 3, "WsfConstellationMaker", "int, int, int")
{
   int  totalArg  = aVarArgs[0].GetInt();
   int  planesArg = aVarArgs[1].GetInt();
   int  fArg      = aVarArgs[2].GetInt();
   bool takeAction{false};
   if (totalArg <= 0)
   {
      auto logger = ut::log::error() << "Constellation must have positive number of members.";
      logger.AddNote() << "Provided: " << totalArg;
   }
   else if (planesArg <= 0)
   {
      auto logger = ut::log::error() << "Constellation must have positive number of planes.";
      logger.AddNote() << "Provided: " << planesArg;
   }
   else if (fArg < 0)
   {
      auto logger = ut::log::error() << "Constellation must have non-negative f parameter.";
      logger.AddNote() << "Provided: " << fArg;
   }
   else
   {
      takeAction = true;
   }

   std::unique_ptr<WsfConstellationMaker> retval{nullptr};
   if (takeAction)
   {
      retval = WsfConstellationMaker::CreateWalkerStar(ut::safe_cast<unsigned int>(totalArg),
                                                       ut::safe_cast<unsigned int>(planesArg),
                                                       ut::safe_cast<unsigned int>(fArg));
   }
   aReturnVal.SetPointer(new UtScriptRef(retval.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker,
                        WsfConstellationMaker,
                        CreateGeneral,
                        4,
                        "WsfConstellationMaker",
                        "int, int, double, double")
{
   std::unique_ptr<WsfConstellationMaker> retval{nullptr};
   int                                    planesArg = aVarArgs[0].GetInt();
   int                                    sppArg    = aVarArgs[1].GetInt();
   if (planesArg <= 0)
   {
      auto logger = ut::log::error() << "Constellation must have positive number of planes.";
      logger.AddNote() << "Provided: " << planesArg;
   }
   else if (sppArg <= 0)
   {
      auto logger = ut::log::error() << "Constellation must have positive number of satellites per plane.";
      logger.AddNote() << "Provided: " << sppArg;
   }
   else
   {
      retval = WsfConstellationMaker::CreateGeneral(ut::safe_cast<unsigned int>(aVarArgs[0].GetInt()),
                                                    ut::safe_cast<unsigned int>(aVarArgs[1].GetInt()),
                                                    UtAngleValue{aVarArgs[2].GetDouble(), UtUnitAngle::cDEGREES},
                                                    UtAngleValue{aVarArgs[3].GetDouble(), UtUnitAngle::cDEGREES});
   }
   aReturnVal.SetPointer(new UtScriptRef(retval.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, WriteToFile, 0, "bool", "")
{
   bool retval{true};
   try
   {
      aObjectPtr->WriteToFile();
   }
   catch (std::runtime_error& err)
   {
      auto logger = ut::log::error() << " WsfConstellationMaker.WriteToFile";
      logger.AddNote() << err.what();
      retval = false;
   }
   aReturnVal.SetBool(retval);
}

namespace // {anonymous}
{

UtScript* ValidateFilterScript(const std::string& aFilterScript, const WsfScriptContext& aContext)
{
   UtScript* retvalPtr{nullptr};

   if (!aFilterScript.empty())
   {
      retvalPtr = aContext.FindScript(aFilterScript);
      if (retvalPtr != nullptr)
      {
         if (!aContext.ValidateScript(retvalPtr, "bool", "int, int"))
         {
            std::stringstream err{};
            err << "Filter script '" << aFilterScript << "' must have the signature bool (int, int).";
            throw std::runtime_error(err.str());
         }
      }
      else
      {
         std::stringstream err{};
         err << "Unable to find filter script '" << aFilterScript << "'.";
         throw std::runtime_error(err.str());
      }
   }

   return retvalPtr;
}

UtScript* ValidateSetupScript(const std::string& aSetupScript, const WsfScriptContext& aContext)
{
   UtScript* retvalPtr{nullptr};

   if (!aSetupScript.empty())
   {
      retvalPtr = aContext.FindScript(aSetupScript);
      if (retvalPtr != nullptr)
      {
         if (!aContext.ValidateScript(retvalPtr, "void", "WsfConstellation, int, int, WsfPlatform"))
         {
            std::stringstream err{};
            err << "Setup script '" << aSetupScript << "' must have the signature "
                << "void (WsfConstellation, int, int, WsfPlatform).";
            throw std::runtime_error(err.str());
         }
      }
      else
      {
         std::stringstream err{};
         err << "Unable to find setup script '" << aSetupScript << "'.";
         throw std::runtime_error(err.str());
      }
   }

   return retvalPtr;
}

WsfConstellation* CreateConstellation(double                       aSimTime,
                                      const std::string&           aFilterScript,
                                      const std::string&           aSetupScript,
                                      WsfScriptContext&            aContext,
                                      const WsfConstellationMaker* aMakerPtr)
{
   WsfConstellation* retvalPtr{nullptr};
   try
   {
      UtScript* filterScriptPtr = ValidateFilterScript(aFilterScript, aContext);
      UtScript* setupScriptPtr  = ValidateSetupScript(aSetupScript, aContext);
      retvalPtr = aMakerPtr->Create(aSimTime, *aContext.GetSimulation(), aContext, filterScriptPtr, setupScriptPtr);
   }
   catch (std::runtime_error& err)
   {
      auto logger = ut::log::error() << "WsfConstellationMaker.Create: ";
      logger.AddNote() << err.what();
   }
   return retvalPtr;
}

} // namespace

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, Create_1, 0, "WsfConstellation", "")
{
   WsfConstellation* constellationPtr = CreateConstellation(TIME_NOW, "", "", SIMULATION->GetScriptContext(), aObjectPtr);
   aReturnVal.SetPointer(new UtScriptRef(constellationPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, Create_2, 2, "WsfConstellation", "string, string")
{
   std::string       filterScript = aVarArgs[0].GetString();
   std::string       setupScript  = aVarArgs[1].GetString();
   WsfConstellation* constellationPtr =
      CreateConstellation(TIME_NOW, filterScript, setupScript, SIMULATION->GetScriptContext(), aObjectPtr);
   aReturnVal.SetPointer(new UtScriptRef(constellationPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, DetectConjunction, 0, "bool", "")
{
   bool retval{false};
   try
   {
      retval = aObjectPtr->DetectConjunction();
   }
   catch (std::runtime_error& err)
   {
      auto logger = ut::log::error() << "All constellation parameters must be set.";
      logger.AddNote() << err.what();
   }
   aReturnVal.SetBool(retval);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, IsSetup, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsSetup());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, Validate, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->Validate());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, IsWalkerDelta, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsWalkerDelta());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, IsWalkerStar, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsWalkerStar());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, IsGeneral, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsGeneral());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, TotalSatellites, 0, "int", "")
{
   aReturnVal.SetInt(ut::safe_cast<int>(aObjectPtr->GetNumTotalSats()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, Planes, 0, "int", "")
{
   aReturnVal.SetInt(ut::safe_cast<int>(aObjectPtr->GetNumPlanes()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, SatellitesPerPlane, 0, "int", "")
{
   aReturnVal.SetInt(ut::safe_cast<int>(aObjectPtr->GetSatsPerPlane()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, WalkerF, 0, "int", "")
{
   aReturnVal.SetInt(ut::safe_cast<int>(aObjectPtr->GetWalkerF()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, AnomalyAlias, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetAnomalyAlias().GetAsUnit(UtUnitAngle::cDEGREES));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, RAAN_Range, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRAAN_Range().GetAsUnit(UtUnitAngle::cDEGREES));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, SemiMajorAxis, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetSemiMajorAxis());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, Altitude, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetCircularAltitude());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, RevolutionsPerDay, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRevolutionsPerDay());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, Period, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetOrbitalPeriod());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, Inclination, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetInclination().GetAsUnit(UtUnitAngle::cDEGREES));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, InitialRAAN, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetInitialRAAN().GetAsUnit(UtUnitAngle::cDEGREES));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, InitialAnomaly, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetInitialAnomaly().GetAsUnit(UtUnitAngle::cDEGREES));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, ConstellationName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetConstellationName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, PlatformType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetPlatformType());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, BasePath, 0, "Path", "")
{
   aReturnVal.SetPointer(UtScriptPath::Create(aObjectPtr->GetBasePath()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, SetSemiMajorAxis, 1, "void", "double")
{
   aObjectPtr->SetSemiMajorAxis(UtLengthValue{aVarArgs[0].GetDouble(), UtUnitLength::cMETERS});
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, SetAltitude, 1, "void", "double")
{
   aObjectPtr->SetCircularAltitude(UtLengthValue{aVarArgs[0].GetDouble(), UtUnitLength::cMETERS});
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, SetRevolutionsPerDay, 1, "void", "double")
{
   aObjectPtr->SetRevolutionsPerDay(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, SetPeriod, 1, "void", "double")
{
   aObjectPtr->SetOrbitalPeriod(UtTimeValue{aVarArgs[0].GetDouble(), UtUnitTime::cSECONDS});
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, SetInclination, 1, "void", "double")
{
   aObjectPtr->SetInclination(UtAngleValue{aVarArgs[0].GetDouble(), UtUnitAngle::cDEGREES});
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, SetInitialRAAN, 1, "void", "double")
{
   aObjectPtr->SetInitialRAAN(UtAngleValue{aVarArgs[0].GetDouble(), UtUnitAngle::cDEGREES});
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, SetInitialAnomaly, 1, "void", "double")
{
   aObjectPtr->SetInitialAnomaly(UtAngleValue{aVarArgs[0].GetDouble(), UtUnitAngle::cDEGREES});
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, SetConstellationName, 1, "void", "string")
{
   aObjectPtr->SetConstellationName(aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, SetPlatformType, 1, "void", "string")
{
   aObjectPtr->SetPlatformType(aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptConstellationMaker, WsfConstellationMaker, SetBasePath, 1, "void", "Path")
{
   auto pathPtr = aVarArgs[0].GetPointer()->GetAppObject<UtPath>();
   aObjectPtr->SetBasePath(*pathPtr);
}
