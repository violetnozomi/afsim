// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCyberVisualization.hpp"

#include <array>
#include <cmath>

#include "UtMath.hpp"
#include "WsfPlatform.hpp"

namespace wsf
{
namespace cyber
{

// =================================================================================================
VisualizationDraw::VisualizationDraw(WsfSimulation* aSimulationPtr)
   : mSimulationPtr(aSimulationPtr)
{
}

void VisualizationDraw::AttackInitiated(const Engagement& aEngagement)
{
   DrawAttack(aEngagement, Outcome::Unknown, Outcome::Unknown);
}

// =================================================================================================
void VisualizationDraw::AttackSucceeded(const Engagement& aEngagement)
{
   Outcome percievedOutcome = aEngagement.GetStatusReportSuccess() ? Outcome::Successful : Outcome::Unknown;
   DrawAttack(aEngagement, percievedOutcome, Outcome::Successful);
}

// =================================================================================================
void VisualizationDraw::AttackFailed(const Engagement& aEngagement)
{
   Outcome percievedOutcome = aEngagement.GetStatusReportSuccess() ? Outcome::Failed : Outcome::Unknown;
   DrawAttack(aEngagement, percievedOutcome, Outcome::Failed);
}

// =================================================================================================
void VisualizationDraw::ScanInitiated(const Engagement& aEngagement)
{
   DrawScan(aEngagement, Outcome::Unknown, Outcome::Unknown);
}

// =================================================================================================
void VisualizationDraw::ScanSucceeded(const Engagement& aEngagement)
{
   Outcome percievedOutcome = aEngagement.GetStatusReportSuccess() ? Outcome::Successful : Outcome::Unknown;
   DrawScan(aEngagement, percievedOutcome, Outcome::Successful);
}

// =================================================================================================
void VisualizationDraw::ScanFailed(const Engagement& aEngagement)
{
   Outcome percievedOutcome = aEngagement.GetStatusReportSuccess() ? Outcome::Failed : Outcome::Unknown;
   DrawScan(aEngagement, percievedOutcome, Outcome::Failed);
}

// =================================================================================================
std::string VisualizationDraw::GetName() const
{
   return "WsfCyberVisualizationDraw";
}

namespace
{

// =================================================================================================
void AddVertex(WsfDraw& aDraw, WsfGeoPoint aSource, double aAngle, const std::array<double, 3>& aOffset)
{
   aSource.AddOffset(aAngle, aOffset.data());
   aDraw.VertexLLA(aSource.GetLat(), aSource.GetLon(), aSource.GetAlt());
}

// =================================================================================================
void DrawLightning(WsfDraw& aDraw, const WsfGeoPoint& aSource, const WsfGeoPoint& aDest)
{
   double distXY = aSource.GroundRangeTo(aDest);
   double distZ  = aSource.GetAlt() - aDest.GetAlt();
   double ang    = aSource.TrueBearingTo(aDest);

   const std::array<double, 75> offsets = {
      {0.00, 0.07,  0.00, // Crossbar at the attacker
       0.00, -0.07, 0.00, 0.00, 0.00,  0.00,

       0.10, 0.02,  0.10, // Half of the lightning
       0.05, 0.07,  0.05, 0.20, 0.01,  0.20, 0.15, 0.07,  0.15, 0.40, 0.01,  0.40, 0.35, 0.07,  0.35,
       0.60, 0.01,  0.60, 0.45, 0.07,  0.45, 0.80, 0.01,  0.80, 0.65, 0.07,  0.65, 1.00, 0.00,  1.00,

       0.65, -0.07, 0.65, // Mirror image
       0.80, -0.01, 0.80, 0.45, -0.07, 0.45, 0.60, -0.01, 0.60, 0.35, -0.07, 0.35, 0.40, -0.01, 0.40,
       0.15, -0.07, 0.15, 0.20, -0.01, 0.20, 0.05, -0.07, 0.05, 0.10, -0.02, 0.10, 0.00, 0.00,  0.00}};

   aDraw.BeginPolyline();
   for (size_t i = 0; i < offsets.size(); i += 3)
   {
      AddVertex(aDraw, aSource, ang, {{distXY * offsets[i], distXY * offsets[i + 1], distZ * offsets[i + 2]}});
   }
   aDraw.End();
}

} // namespace

// =================================================================================================
void VisualizationDraw::DrawAttack(const Engagement& aEngagement, Outcome aPercievedOutcome, Outcome aActualOutcome)
{
   Draw(aEngagement, &DrawLightning, aPercievedOutcome, aActualOutcome);
}

namespace
{

// =================================================================================================
void DrawSpiral(WsfDraw& aDraw, const WsfGeoPoint& aSource, const WsfGeoPoint& aDest)
{
   double distXY    = aSource.GroundRangeTo(aDest);
   double distZ     = aSource.GetAlt() - aDest.GetAlt();
   double ang       = aSource.TrueBearingTo(aDest);
   double amplitude = 0.04 * distXY;

   aDraw.BeginPolyline();
   AddVertex(aDraw, aSource, ang, {{0.0, amplitude, 0.0}});
   AddVertex(aDraw, aSource, ang, {{0.0, -amplitude, 0.0}});
   AddVertex(aDraw, aSource, ang, {{0.0, 0.0, 0.0}});
   const size_t step         = 1000;
   const size_t oscillations = 24;
   for (size_t i = 0; i < step; i++)
   {
      double angle = i * (distXY / step) * oscillations * UtMath::cPI / distXY;
      AddVertex(aDraw,
                aSource,
                ang,
                {{distXY * i / step, amplitude * std::sin(angle), (distZ * i / step) + (amplitude * std::cos(angle))}});
   }
   AddVertex(aDraw, aSource, ang, {{distXY, 0.0, distZ}});
   aDraw.End();
}

} // namespace

// =================================================================================================
void VisualizationDraw::DrawScan(const Engagement& aEngagement, Outcome aPercievedOutcome, Outcome aActualOutcome)
{
   Draw(aEngagement, &DrawSpiral, aPercievedOutcome, aActualOutcome);
}

// =================================================================================================
void VisualizationDraw::Draw(const Engagement&                                              aEngagement,
                             const std::function<void(WsfDraw&, WsfGeoPoint, WsfGeoPoint)>& aDrawFunction,
                             Outcome                                                        aPercievedOutcome,
                             Outcome                                                        aActualOutcome)
{
   WsfPlatform* attackerPtr = mSimulationPtr->GetPlatformByName(aEngagement.GetAttacker());
   WsfPlatform* victimPtr   = mSimulationPtr->GetPlatformByName(aEngagement.GetVictim());
   Draw(attackerPtr, victimPtr, aDrawFunction, aPercievedOutcome, aActualOutcome);
}

// =================================================================================================
void VisualizationDraw::Draw(WsfPlatform*                                                   aAttacker,
                             WsfPlatform*                                                   aVictim,
                             const std::function<void(WsfDraw&, WsfGeoPoint, WsfGeoPoint)>& aDrawFunction,
                             Outcome                                                        aPercievedOutcome,
                             Outcome                                                        aActualOutcome)
{
   double lat;
   double lon;
   double alt;
   aAttacker->GetLocationLLA(lat, lon, alt);
   WsfGeoPoint attackerPoint(lat, lon, alt);
   aVictim->GetLocationLLA(lat, lon, alt);
   WsfGeoPoint victimPoint(lat, lon, alt);
   Draw(attackerPoint, victimPoint, aDrawFunction, aPercievedOutcome, aActualOutcome);
}

// =================================================================================================
void VisualizationDraw::Draw(WsfGeoPoint                                                    aSource,
                             WsfGeoPoint                                                    aDest,
                             const std::function<void(WsfDraw&, WsfGeoPoint, WsfGeoPoint)>& aDrawFunction,
                             Outcome                                                        aPercievedOutcome,
                             Outcome                                                        aActualOutcome)
{
   WsfDraw draw(*mSimulationPtr);
   draw.SetDuration(3.0);
   draw.SetColor(GetColor(aPercievedOutcome));
   aDrawFunction(draw, aSource, aDest);

   draw.SetColor(GetColor(aActualOutcome));
   draw.BeginEllipse(0, 8000, 8000);
   draw.VertexLLA(aDest.GetLat(), aDest.GetLon(), aDest.GetAlt());
   draw.End();
}

// =================================================================================================
UtColor VisualizationDraw::GetColor(Outcome aOutcome)
{
   switch (aOutcome)
   {
   case Outcome::Successful:
      return UtColor(0.2f, 1.0f, 0.2f); // Green
   case Outcome::Failed:
      return UtColor(1.0f, 0.2f, 0.2f); // Red
   case Outcome::Unknown:
      return UtColor(1.0f, 1.0f, 0.2f); // Yellow
   }

   throw UtException("Unsupported Outcome for WsfCyberVisualization::GetColor");
}

} // namespace cyber
} // namespace wsf
