// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfOrbitalAttitudeChangeEvent.hpp"

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfAttitudeController.hpp"
#include "WsfOrbitalEventVisitor.hpp"
#include "WsfSpaceMoverBase.hpp"
#include "WsfSpaceOrbitalMissionContext.hpp"

WsfOrbitalEvent* WsfOrbitalAttitudeChangeEvent::Clone() const
{
   return new WsfOrbitalAttitudeChangeEvent(*this);
}

bool WsfOrbitalAttitudeChangeEvent::ExecuteEvent(const UtCalendar& aEpoch, wsf::space::OrbitalMissionContext& aContext)
{
   WsfSpaceMoverBase* spaceMoverPtr = aContext.GetSpaceMover();
   if (spaceMoverPtr != nullptr)
   {
      spaceMoverPtr->GetAttitudeController().SetOrientation(std::move(mOrientationPtr));
   }
   return true;
}

WsfStringId WsfOrbitalAttitudeChangeEvent::GetTypeId()
{
   return "CHANGE_ATTITUDE";
}

WsfOrbitalAttitudeChangeEvent::WsfOrbitalAttitudeChangeEvent(const WsfScenario& aScenario)
   : WsfOrbitalEvent(aScenario)
{
   SetType(GetTypeId());
}

WsfOrbitalAttitudeChangeEvent::WsfOrbitalAttitudeChangeEvent(const WsfOrbitalAttitudeChangeEvent& aSrc)
   : WsfOrbitalEvent(aSrc)
{
   if (aSrc.mOrientationPtr != nullptr)
   {
      mOrientationPtr.reset(aSrc.mOrientationPtr->Clone());
   }
}

bool WsfOrbitalAttitudeChangeEvent::ProcessInput(UtInput& aInput)
{
   bool        myCommand{false};
   std::string command = aInput.GetCommand();
   if (command == "orientation")
   {
      std::string orientation;
      aInput.ReadValue(orientation);
      mOrientationPtr = wsf::space::Orientation::Factory(orientation);
      if (mOrientationPtr != nullptr)
      {
         mOrientationPtr->ProcessInput(aInput);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
      myCommand = true;
   }
   else
   {
      myCommand = WsfOrbitalEvent::ProcessInput(aInput);
   }
   return myCommand;
}

bool WsfOrbitalAttitudeChangeEvent::Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext)
{
   bool ok = ((mOrientationPtr != nullptr) && mOrientationPtr->IsValid());
   if (ok)
   {
      if (aContext.GetSpaceMover())
      {
         mOrientationPtr->SetAttitudeController(aContext.GetSpaceMover()->GetAttitudeController());
      }
   }
   return (ok && WsfOrbitalEvent::Initialize(aEpoch, aContext));
}

void WsfOrbitalAttitudeChangeEvent::SetOrientation(std::unique_ptr<wsf::space::Orientation> aOrientationPtr)
{
   mOrientationPtr = std::move(aOrientationPtr);
}

wsf::space::Orientation* WsfOrbitalAttitudeChangeEvent::GetOrientation() const
{
   return mOrientationPtr.get();
}

void WsfOrbitalAttitudeChangeEvent::Accept(WsfOrbitalEventVisitor* aVisitorPtr)
{
   aVisitorPtr->VisitOrbitalAttitudeChangeEvent(this);
}

bool WsfOrbitalAttitudeChangeEvent::ValidateParameterRanges(const std::string& aExecutingPlatformName) const
{
   bool retval{true};
   if (!mOrientationPtr)
   {
      auto logger = ut::log::error() << "No orientation set.";
      logger.AddNote() << "Type: " << GetType();
      retval = false;
   }
   else
   {
      auto entityRefPtr = dynamic_cast<wsf::space::OrientationEntity*>(mOrientationPtr.get());
      if (entityRefPtr)
      {
         if (entityRefPtr->GetEntityName().empty() && entityRefPtr->GetLocalTrackId().IsNull())
         {
            auto logger = ut::log::error()
                          << "Entity referenced orientations need to have a target entity or track specified.";
            logger.AddNote() << "Type: " << GetType();
            retval = false;
         }
      }
      else
      {
         auto geoRefPtr = dynamic_cast<wsf::space::OrientationGeoPoint*>(mOrientationPtr.get());
         if (geoRefPtr)
         {
            if (geoRefPtr->GetGeoPointName().empty())
            {
               auto logger = ut::log::error()
                             << "GeoPoint referenced orientations need to have a target point specified.";
               logger.AddNote() << "Type: " << GetType();
               retval = false;
            }
         }
      }
   }
   retval = WsfOrbitalEvent::ValidateParameterRanges(aExecutingPlatformName) && retval;
   return retval;
}
