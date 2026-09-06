// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "PPLI_Part.hpp"

#include "ComputerProcessor.hpp"
#include "Message2_2.hpp"
#include "Message2_3.hpp"
#include "Message2_4.hpp"
#include "Message2_5.hpp"
#include "Message2_6.hpp"
#include "WsfGroundMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfSubsurfaceMover.hpp"
#include "WsfSurfaceMover.hpp"

namespace WsfL16
{

PPLI_Part::PPLI_Part(WsfScenario& aScenario)
   : ComputerPart(aScenario)
   , mCategory(cUNSET)
   , mLandSubcategory(c_POINT)
{
   mSendInterval.SetUpdateInterval(12.0);
   mCategoryInput << cAIR << "air";
   mCategoryInput << cSURFACE << "surface";
   mCategoryInput << cLAND << "land";
   mCategoryInput << cSUBSURFACE << "subsurface";
   mLandSubcategoryInput << c_POINT << "point";
   mLandSubcategoryInput << c_LAND << "land";
}

PPLI_Part::PPLI_Part(const PPLI_Part& aSrc)
   : ComputerPart(aSrc)
   , mCategory(aSrc.mCategory)
   , mCategoryInput(aSrc.mCategoryInput)
   , mLandSubcategory(aSrc.mLandSubcategory)
   , mLandSubcategoryInput(aSrc.mLandSubcategoryInput)
{
}

ComputerPart* PPLI_Part::Clone() const
{
   return new PPLI_Part(*this);
}

bool PPLI_Part::Initialize(double aSimTime, ComputerProcessor& aComputer)
{
   bool ok = ComputerPart::Initialize(aSimTime, aComputer);

   if (mCategory == cUNSET)
   {
      // try to guess the environment
      mCategory          = cAIR;
      WsfPlatform* plat  = GetPlatform();
      WsfMover*    mover = plat->GetMover();
      if (dynamic_cast<WsfSurfaceMover*>(mover) != nullptr || plat->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_SURFACE)
      {
         mCategory = cSURFACE;
      }
      else if (dynamic_cast<WsfGroundMover*>(mover) != nullptr || plat->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_LAND)
      {
         if (mLandSubcategory == c_LAND)
         {
            mCategory = cLAND;
         }
         else
         {
            mCategory = cPOINT;
         }
      }
      else if (dynamic_cast<WsfSubsurfaceMover*>(mover) != nullptr ||
               plat->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_SUBSURFACE)
      {
         mCategory = cSUBSURFACE;
      }
   }

   return ok;
}

bool PPLI_Part::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   if (mCategoryInput.ProcessInput("environment", aInput, mCategory))
   {
   }
   else if (mLandSubcategoryInput.ProcessInput("land_or_point", aInput, mLandSubcategory))
   {
   }
   else if (ComputerPart::ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

void PPLI_Part::HandleReceive(double aSimTime, const Messages::InitialBase& /*aMessage*/) {}

void PPLI_Part::HandleSend(double aSimTime)
{
   SendPPLI(aSimTime);
}
void PPLI_Part::SendPPLI(double aSimTime)
{
   if (mCategory == cAIR)
   {
      SendAirPPLI(aSimTime, *GetPlatform());
   }
   else if (mCategory == cPOINT)
   {
      SendLandPointPPLI(aSimTime, *GetPlatform());
   }
   else if (mCategory == cLAND)
   {
      SendLandPPLI(aSimTime, *GetPlatform());
   }
   else if (mCategory == cSURFACE)
   {
      SendSurfacePPLI(aSimTime, *GetPlatform());
   }
   else if (mCategory == cSUBSURFACE)
   {
      SendSubsurfacePPLI(aSimTime, *GetPlatform());
   }
}
void PPLI_Part::SendAirPPLI(double aSimTime, WsfPlatform& aPlatform)
{
   using namespace Messages::J2_2;

   double lat, lon, alt, yaw, pitch, roll, speed;
   aPlatform.GetLocationLLA(lat, lon, alt);
   aPlatform.GetOrientationNED(yaw, pitch, roll);
   speed = aPlatform.GetSpeed();

   Initial* msg             = new Initial;
   msg->IsExerciseTrackUnit = true;
   msg->IsC2Unit            = GetComputer()->Config().mIsC2;
   msg->Altitude            = alt;
   msg->IsAirborne          = true;

   auto e0       = ut::make_unique<Extension0>();
   e0->Latitude  = lat;
   e0->Longitude = lon;
   e0->Course    = yaw;
   e0->Speed     = speed;
   msg->InsertExtensionWord(std::move(e0));

   const std::string& callsign = GetComputer()->Config().mCallsign;
   if (!callsign.empty())
   {
      auto c2      = ut::make_unique<Continuation2>();
      c2->Callsign = callsign;
      msg->InsertContinuationWord(std::move(c2));
   }

   SendJMessage(aSimTime, msg);
}

void PPLI_Part::SendSurfacePPLI(double aSimTime, WsfPlatform& aPlatform)
{
   using namespace Messages::J2_3;

   double lat, lon, alt, yaw, pitch, roll, speed;
   aPlatform.GetLocationLLA(lat, lon, alt);
   aPlatform.GetOrientationNED(yaw, pitch, roll);
   speed = aPlatform.GetSpeed();

   Initial* msg             = new Initial;
   msg->IsExerciseTrackUnit = true;
   msg->IsC2Unit            = GetComputer()->Config().mIsC2;
   msg->Altitude            = alt;

   auto e0       = ut::make_unique<Extension0>();
   e0->Latitude  = lat;
   e0->Longitude = lon;
   e0->Course    = yaw;
   e0->Speed     = speed;
   msg->InsertExtensionWord(std::move(e0));

   const std::string& callsign = GetComputer()->Config().mCallsign;
   if (!callsign.empty())
   {
      auto c2      = ut::make_unique<Continuation2>();
      c2->Callsign = callsign;
      msg->InsertContinuationWord(std::move(c2));
   }

   SendJMessage(aSimTime, msg);
}

void PPLI_Part::SendSubsurfacePPLI(double aSimTime, WsfPlatform& aPlatform)
{
   using namespace Messages::J2_4;

   double lat, lon, alt, yaw, pitch, roll, speed;
   aPlatform.GetLocationLLA(lat, lon, alt);
   aPlatform.GetOrientationNED(yaw, pitch, roll);
   speed = aPlatform.GetSpeed();

   Initial* msg             = new Initial;
   msg->IsExerciseTrackUnit = true;
   msg->IsC2Unit            = GetComputer()->Config().mIsC2;

   auto e0       = ut::make_unique<Extension0>();
   e0->Latitude  = lat;
   e0->Longitude = lon;
   e0->Course    = yaw;
   e0->Speed     = speed;
   msg->InsertExtensionWord(std::move(e0));

   const std::string& callsign = GetComputer()->Config().mCallsign;
   if (!callsign.empty())
   {
      auto c2      = ut::make_unique<Continuation2>();
      c2->Callsign = callsign;
      msg->InsertContinuationWord(std::move(c2));
   }

   SendJMessage(aSimTime, msg);
}


void PPLI_Part::SendLandPointPPLI(double aSimTime, WsfPlatform& aPlatform)
{
   using namespace Messages::J2_5;

   double lat, lon, alt;
   aPlatform.GetLocationLLA(lat, lon, alt);

   Initial* msg             = new Initial;
   msg->IsExerciseTrackUnit = true;
   msg->IsC2Unit            = GetComputer()->Config().mIsC2;
   msg->Altitude            = alt;

   auto e0       = ut::make_unique<Extension0>();
   e0->Latitude  = lat;
   e0->Longitude = lon;
   msg->InsertExtensionWord(std::move(e0));

   auto               c1       = ut::make_unique<Continuation1>();
   const std::string& callsign = GetComputer()->Config().mCallsign;
   if (!callsign.empty())
   {
      // Continuation1* c1 = new Continuation1;
      c1->Callsign = callsign;
      // msg->InsertContinuationWord(c1);
   }
   c1->LandPlatform = 33;
   msg->InsertContinuationWord(std::move(c1));

   SendJMessage(aSimTime, msg);
}

void PPLI_Part::SendLandPPLI(double aSimTime, WsfPlatform& aPlatform)
{
   using namespace Messages::J2_6;

   double lat, lon, alt, yaw, pitch, roll, speed;
   aPlatform.GetLocationLLA(lat, lon, alt);
   aPlatform.GetOrientationNED(yaw, pitch, roll);
   speed = aPlatform.GetSpeed();

   Initial* msg             = new Initial;
   msg->IsExerciseTrackUnit = true;
   msg->IsC2Unit            = GetComputer()->Config().mIsC2;
   msg->Altitude            = alt;

   auto e0       = ut::make_unique<Extension0>();
   e0->Latitude  = lat;
   e0->Longitude = lon;
   e0->Course    = yaw;
   e0->Speed     = speed;
   msg->InsertExtensionWord(std::move(e0));

   const std::string& callsign = GetComputer()->Config().mCallsign;
   if (!callsign.empty())
   {
      auto c1      = ut::make_unique<Continuation1>();
      c1->Callsign = callsign;
      msg->InsertContinuationWord(std::move(c1));
   }

   SendJMessage(aSimTime, msg);
}

} // namespace WsfL16
