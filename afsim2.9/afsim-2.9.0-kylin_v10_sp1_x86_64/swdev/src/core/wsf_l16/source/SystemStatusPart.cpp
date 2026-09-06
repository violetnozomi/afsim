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
#include "SystemStatusPart.hpp"

#include "ComputerProcessor.hpp"
#include "Message13_2.hpp"
#include "Message13_3.hpp"
#include "Message13_5.hpp"
#include "WsfFuel.hpp"
#include "WsfGroundMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfStringId.hpp"
#include "WsfSubsurfaceMover.hpp"
#include "WsfSurfaceMover.hpp"
#include "WsfWeapon.hpp"

namespace WsfL16
{
using namespace Messages;
// using namespace Messages::J13_2;
// using namespace Messages::J13_5;

SystemStatusPart::SystemStatusPart(WsfScenario& aScenario)
   : ComputerPart(aScenario)
   , mCategory(cUNSET)
{
   // mSendInterval.SetUpdateInterval(24.0);
   mSendInterval.SetUpdateInterval(1.0);
   mCategoryInput << cAIR << "air";
   mCategoryInput << cSURFACE << "surface";
   mCategoryInput << cLAND << "land";
   mCategoryInput << cSUBSURFACE << "subsurface";
}

SystemStatusPart::SystemStatusPart(const SystemStatusPart& aSrc)
   : ComputerPart(aSrc)
   , mConfig(aSrc.mConfig)
   , mCategory(aSrc.mCategory)
   , mCategoryInput(aSrc.mCategoryInput)
{
}

SystemStatusPart::~SystemStatusPart() {}

ComputerPart* SystemStatusPart::Clone() const
{
   return new SystemStatusPart(*this);
}

bool SystemStatusPart::Initialize(double aSimTime, ComputerProcessor& aComputer)
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
         mCategory = cLAND;
      }
      else if (dynamic_cast<WsfSubsurfaceMover*>(mover) != nullptr ||
               plat->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_SUBSURFACE)
      {
         mCategory = cSUBSURFACE;
      }
   }
   return ok && CheckWeapon(mConfig.mWeaponNameId1) && CheckWeapon(mConfig.mWeaponNameId2) &&
          CheckWeapon(mConfig.mWeaponNameId3);
}

bool SystemStatusPart::CheckWeapon(WsfStringId aWeaponId)
{
   if (aWeaponId == nullptr)
   {
      return true;
   }

   WsfWeapon* wpn = GetPlatform()->GetComponent<WsfWeapon>(aWeaponId);
   if (!wpn)
   {
      auto logInfo = ut::log::info() << "Could not find Weapon on WsfL16 processor.";
      logInfo.AddNote() << "Weapon: " << aWeaponId;
      logInfo.AddNote() << "WsfL16 Processor: " << GetName();
   }
   return wpn != nullptr;
}

bool SystemStatusPart::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string cmd;
   aInput.GetCommand(cmd);
   std::string wpnName;

   if (cmd == "report_weapon_quantity_1")
   {
      aInput.ReadValue(wpnName);
      aInput.ReadValue(mConfig.mWeaponTypeId1);
      mConfig.mWeaponNameId1 = wpnName;
   }
   else if (cmd == "report_weapon_quantity_2")
   {
      aInput.ReadValue(wpnName);
      aInput.ReadValue(mConfig.mWeaponTypeId2);
      mConfig.mWeaponNameId2 = wpnName;
   }
   else if (cmd == "report_weapon_quantity_3")
   {
      aInput.ReadValue(wpnName);
      aInput.ReadValue(mConfig.mWeaponTypeId3);
      mConfig.mWeaponNameId3 = wpnName;
   }
   else
   {
      myCommand = ComputerPart::ProcessInput(aInput);
   }
   return myCommand;
}

void SystemStatusPart::HandleSend(double aSimTime)
{
   SendSystemStatus(aSimTime);
}

void SystemStatusPart::SendSystemStatus(double aSimTime)
{
   if (mCategory == cAIR)
   {
      SendAirSystemStatus(aSimTime);
   }
   else if (mCategory == cLAND)
   {
      SendLandPointSystemStatus(aSimTime);
   }
   else if (mCategory == cSURFACE)
   {
      SendSurfaceSystemStatus(aSimTime);
   }
   // else if (mCategory == cSUBSURFACE)
   //{
   // SendSubsurfaceSystemStatus(aSimTime, *GetPlatform());
   // }
}

void SystemStatusPart::SendAirSystemStatus(double aSimTime)
{
   using namespace Messages::J13_2;

   Initial* msg               = new Initial;
   msg->OperationalCapability = OperationalCapabilityField::cABLE_TO_PERFORM_MISSION;
   if (GetPlatform()->GetComponent<WsfFuel>())
   {
      msg->Fuel = GetPlatform()->GetComponent<WsfFuel>()->GetQuantityRemaining();
   }
   msg->RadarChannelType.SetSingleRF();
   msg->RadarChannel        = 19;
   auto c1                  = ut::make_unique<Continuation1>();
   c1->RadarStatus          = EquipmentOperationalField::cOFF;
   c1->IsSensorReportingAll = true;
   c1->AirSpecificType      = GetComputer()->Config().mSpecificTypeID;
   auto c2                  = ut::make_unique<Continuation2>();
   c2->IsSummation          = true;
   c2->NumberOfStores1      = WeaponQuantity(mConfig.mWeaponNameId1);
   c2->StoreType1           = mConfig.mWeaponTypeId1;
   c2->NumberOfStores2      = WeaponQuantity(mConfig.mWeaponNameId2);
   c2->StoreType2           = mConfig.mWeaponTypeId2;
   c2->NumberOfStores3      = WeaponQuantity(mConfig.mWeaponNameId3);
   c2->StoreType3           = mConfig.mWeaponTypeId3;

   msg->InsertContinuationWord(std::move(c1));
   msg->InsertContinuationWord(std::move(c2));
   SendJMessage(aSimTime, msg);
}

void SystemStatusPart::SendLandPointSystemStatus(double aSimTime)
{
   using namespace Messages::J13_5;

   Initial* msg = new Initial;
   if (msg)
   {
      msg->OperationalCapability = OperationalCapabilityField::cABLE_TO_PERFORM_MISSION;
      msg->TrackNumberReference.SetTrackNumber(GetComputer()->Config().mTrackNumber);
      msg->SiteType = SiteTypeField::cBM_DEFENSE_SITE;
      // unsigned int wpnCount = GetPlatform()->GetComponentCount<WsfWeapon>();
      int numHot = 0;
      for (WsfComponentList::RoleIterator<WsfWeapon> iter(*GetPlatform()); !iter.AtEnd(); ++iter)
      {
         WsfWeapon* wpnPtr = *iter;
         if (wpnPtr != nullptr)
         {
            if (wpnPtr->IsCued())
            {
               ++numHot;
            }
         }
      }
      msg->HotMissileCount           = numHot;
      msg->SamModeState              = SAM_ModeStateField::cNO_STATEMENT;
      msg->TimeFunction              = TimeFunctionField::cNO_STATEMENT;
      msg->Minute                    = MinuteField::cNO_STATEMENT_VALUE;
      msg->Hour                      = HourField::cNO_STATEMENT_VALUE;
      msg->PerimeterEngagementStatus = false;

      auto c1 = ut::make_unique<Continuation1>();
      if (c1)
      {
         c1->ColdInventory            = GetPlatform()->GetComponentCount<WsfWeapon>();
         c1->OperationalImpairment    = OperationalImpairmentField::cNO_STATEMENT;
         c1->NATOLink1Status          = SystemStatusField::cNO_STATEMENT;
         c1->Link14Status             = SystemStatusField::cNO_STATEMENT;
         c1->Link11Status             = LinkStatusField::cNO_STATEMENT;
         c1->Link11BStatus            = SystemStatusField::cNO_STATEMENT;
         c1->Link16Status             = LinkStatusField::cNO_STATEMENT;
         c1->ATDL_1Status             = SystemStatusField::cNO_STATEMENT;
         c1->IJMSStatus               = SystemStatusField::cNO_STATEMENT;
         c1->CommunicationsImpairment = CommImpairmentField::cNO_STATEMENT;
         c1->ControlPositions.SetNoStatement();
         c1->TimeFunction             = TimeFunctionField::cNO_STATEMENT;
         c1->Minute                   = MinuteField::cNO_STATEMENT_VALUE;
         c1->Hour                     = HourField::cNO_STATEMENT_VALUE;
         c1->PrimarySurvRadarStatus   = RadarSystemStatusField::cSYSTEM_OPERATIONAL;
         c1->SecondarySurvRadarStatus = RadarSystemStatusField::cSYSTEM_OFF;
         c1->TertiarySurvRadarStatus  = RadarSystemStatusField::cSYSTEM_OFF;
         c1->AcquistionRadarStatus    = RadarSystemStatusField::cSYSTEM_OPERATIONAL;
         c1->IlluminatingRadarStatus  = false;
         c1->Mode4InterrogatorStatus  = false;
         c1->IFFInterrogatorStatus    = RadarSystemStatusField::cSYSTEM_OFF;

         msg->InsertContinuationWord(std::move(c1));
      }
      /*
            Continuation2* c2 = new Continuation2;
            if (c2)
            {
               c2->ESFreqBandA.SetNoStatement();
               c2->ESFreqBandB.SetNoStatement();
               c2->ESFreqBandC.SetNoStatement();
               c2->ESFreqBandD.SetNoStatement();
               c2->ESFreqBandE.SetNoStatement();
               c2->ESFreqBandF.SetNoStatement();
               c2->ESFreqBandG.SetNoStatement();
               c2->ESFreqBandH.SetNoStatement();
               c2->ESFreqBandI.SetNoStatement();
               c2->ESFreqBandJ.SetNoStatement();
               c2->ESFreqBandK.SetNoStatement();
               c2->ESFreqBandL.SetNoStatement();
               c2->ESFreqBandM.SetNoStatement();

               c2->HFDirectionFinder.SetNoStatement();
               c2->VHFDirectionFinder.SetNoStatement();
               c2->UHFDirectionFinder.SetNoStatement();

               c2->EAFreqBandA.SetNoStatement();
               c2->EAFreqBandB.SetNoStatement();
               c2->EAFreqBandC.SetNoStatement();
               c2->EAFreqBandD.SetNoStatement();
               c2->EAFreqBandE.SetNoStatement();
               c2->EAFreqBandF.SetNoStatement();
               c2->EAFreqBandG.SetNoStatement();
               c2->EAFreqBandH.SetNoStatement();
               c2->EAFreqBandI.SetNoStatement();
               c2->EAFreqBandJ.SetNoStatement();
               c2->EAFreqBandK.SetNoStatement();
               c2->EAFreqBandL.SetNoStatement();
               c2->EAFreqBandM.SetNoStatement();

               c2->DecoyChaff.SetNoStatement();
               c2->DecoyIR.SetNoStatement();

               msg->InsertContinuationWord(c2);
           }
      */
   }
   SendJMessage(aSimTime, msg);
}


void SystemStatusPart::SendSurfaceSystemStatus(double aSimTime)
{
   using namespace Messages::J13_3;

   Initial* msg = new Initial;
   if (msg)
   {
      msg->OperationalCapability = OperationalCapabilityField::cABLE_TO_PERFORM_MISSION;
      msg->TrackNumber.SetTrackNumber(GetComputer()->Config().mTrackNumber);
      msg->FlightDeckStatus = 0;
      msg->Minute           = MinuteField::cNO_STATEMENT_VALUE;
      msg->Hour             = HourField::cNO_STATEMENT_VALUE;
   }
   SendJMessage(aSimTime, msg);
}
void SystemStatusPart::HandleReceive(double aSimTime, const Messages::InitialBase& /*aMessage*/) {}

int SystemStatusPart::WeaponQuantity(WsfStringId aWeaponId)
{
   int quantity = 0;
   if (aWeaponId != 0)
   {
      WsfWeapon* wpn = GetPlatform()->GetComponent<WsfWeapon>(aWeaponId);
      if (wpn)
      {
         quantity = (int)wpn->GetQuantityRemaining();
      }
   }
   return quantity;
}

SystemStatusPart::Config::Config()
   : mWeaponNameId1(nullptr)
   , mWeaponTypeId1(0)
   , mWeaponNameId2(nullptr)
   , mWeaponTypeId2(0)
   , mWeaponNameId3(nullptr)
   , mWeaponTypeId3(0)
{
}

} // namespace WsfL16
