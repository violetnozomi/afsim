// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "OrbitPreviewEntity.hpp"

#include "EventOrbitDetails.hpp"
#include "UtCalendar.hpp"
#include "UtOrbitalElements.hpp"
#include "VaAttachmentModel.hpp"
#include "VaEnvironment.hpp"
#include "VaUtils.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"
#include "WsfSpaceOrientation.hpp"
#include "orbit/WkfAttachmentOrbit.hpp"

namespace wsfg
{

namespace spaceg
{

OrbitPreviewEntity::OrbitPreviewEntity(const std::string& aName, const UtColor& aColor)
   : wkf::Entity{aName}
   , mPreviewColor{aColor}
   , mModelPtr{nullptr}
   , mOrbitBeforePtr{nullptr}
   , mOrbitAfterPtr{nullptr}
{
   SetType<OrbitPreviewEntity>();

   if (vaEnv.GetStandardViewer())
   {
      mModelPtr = vespa::make_attachment<vespa::VaAttachmentModel>(*this, vaEnv.GetStandardViewer());
      mModelPtr->SetIcon("wedge");
      mModelPtr->SetColor(mPreviewColor);
   }
}

void OrbitPreviewEntity::UpdateEventDetails(const EventOrbitDetails& aDetails)
{
   mPreviewColor = aDetails.mPreviewColor;
   AssureBothOrbitsExist();
   mOrbitBeforePtr->Clear();
   mOrbitAfterPtr->Clear();

   if (aDetails.IsValid())
   {
      // If there is a change in the orbit, display the old orbit also.
      if (!aDetails.IsNoDeltaEvent())
      {
         mOrbitBeforePtr->Add(aDetails.mSimTimeBefore,
                              aDetails.mElementsBefore.GetSemiMajorAxis(),
                              aDetails.mElementsBefore.GetEccentricity(),
                              aDetails.mElementsBefore.GetRAAN(),
                              aDetails.mElementsBefore.GetInclination(),
                              aDetails.mElementsBefore.GetArgumentOfPeriapsis(),
                              aDetails.mElementsBefore.GetTrueAnomaly(),
                              false);
         mOrbitBeforePtr->UpdateTimeAngle(aDetails.mElementsBefore.GetEpoch().GetEarthAngle(), aDetails.mSimTimeBefore);
         mOrbitBeforePtr->SetStateVisibility(true);
      }
      else
      {
         mOrbitBeforePtr->SetStateVisibility(false);
      }

      // Always display the new event.
      mOrbitAfterPtr->Add(aDetails.mSimTimeAfter,
                          aDetails.mElementsAfter.GetSemiMajorAxis(),
                          aDetails.mElementsAfter.GetEccentricity(),
                          aDetails.mElementsAfter.GetRAAN(),
                          aDetails.mElementsAfter.GetInclination(),
                          aDetails.mElementsAfter.GetArgumentOfPeriapsis(),
                          aDetails.mElementsAfter.GetTrueAnomaly(),
                          false);
      mOrbitAfterPtr->UpdateTimeAngle(aDetails.mElementsAfter.GetEpoch().GetEarthAngle(), aDetails.mSimTimeAfter);
      mOrbitAfterPtr->SetStateVisibility(true);

      UpdatePositionOrientation(aDetails.mElementsAfter, aDetails.mElementsAfter.GetEpoch());
   }
   else
   {
      SetVisible(false);
   }
}

void OrbitPreviewEntity::SetColor(const UtColor& aColor)
{
   mPreviewColor = aColor;
   AssureBothOrbitsExist();
}

void OrbitPreviewEntity::SetVisible(bool aVisible)
{
   if (mModelPtr)
   {
      mModelPtr->SetStateVisibility(aVisible);
   }
   if (mOrbitBeforePtr)
   {
      mOrbitBeforePtr->SetStateVisibility(aVisible);
   }
   if (mOrbitAfterPtr)
   {
      mOrbitAfterPtr->SetStateVisibility(aVisible);
   }
}

UtColor OrbitPreviewEntity::FadedColor() const
{
   float red, green, blue;
   mPreviewColor.Get(red, green, blue);
   return UtColor{red * 0.5f, green * 0.5f, blue * 0.5f};
}

void OrbitPreviewEntity::AssureBothOrbitsExist()
{
   if (!mOrbitBeforePtr)
   {
      mOrbitBeforePtr = vespa::make_attachment<wkf::AttachmentOrbit>(*this, vaEnv.GetStandardViewer());
      vespa::VaAttachment::LoadAttachment(*mOrbitBeforePtr);
      mOrbitBeforePtr->SetLineWidth(2);
   }

   if (!mOrbitAfterPtr)
   {
      mOrbitAfterPtr = vespa::make_attachment<wkf::AttachmentOrbit>(*this, vaEnv.GetStandardViewer());
      vespa::VaAttachment::LoadAttachment(*mOrbitAfterPtr);
      mOrbitAfterPtr->SetLineWidth(2);
   }

   UpdateColors();
}

void OrbitPreviewEntity::UpdateColors()
{
   mOrbitBeforePtr->SetColor(FadedColor());
   mOrbitAfterPtr->SetColor(mPreviewColor);
   mModelPtr->SetColor(mPreviewColor);
}

void OrbitPreviewEntity::UpdatePositionOrientation(UtOrbitalElements aElements, const UtCalendar& aEpoch)
{
   // Get position and velocity from the elements
   UtVec3d position;
   UtVec3d velocity;
   aElements.ComputeState(position, velocity);

   // Create a helper UtEntity for doing coordinate conversions.
   UtEntity helper{};
   helper.SetECI_ReferenceEpoch(aEpoch);
   helper.SetTime(0.0);
   helper.SetLocationTOD(position.GetData());
   helper.SetVelocityTOD(velocity.GetData());

   // Set the ECI orientation so that it points along the track of the orbit, with 'down'
   // constrained by the direction toward the Earth.
   UtVec3d negativePosition(position);
   negativePosition *= -1.0;
   double psiECI, thetaECI, phiECI;
   wsf::space::Orientation::CalculateX_Aligned(velocity, negativePosition, psiECI, thetaECI, phiECI);
   helper.SetOrientationECI(psiECI, thetaECI, phiECI);

   // Get the WCS frame position and orientation.
   double locationWCS[3];
   helper.GetLocationWCS(locationWCS);
   double psiWCS, thetaWCS, phiWCS;
   helper.GetOrientationWCS(psiWCS, thetaWCS, phiWCS);

   // Set the position and orientation of this entity.
   SetPositionOrientation(locationWCS, psiWCS, thetaWCS, phiWCS);

   mModelPtr->SetStateVisibility(true);
}

} // namespace spaceg

} // namespace wsfg
