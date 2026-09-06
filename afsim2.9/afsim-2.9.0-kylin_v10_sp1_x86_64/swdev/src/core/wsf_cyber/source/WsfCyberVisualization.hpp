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

#ifndef WSFCYBERVISUALIZATION_HPP
#define WSFCYBERVISUALIZATION_HPP

#include <functional>

#include "WsfCyberEngagement.hpp"
#include "WsfDraw.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace cyber
{

class Visualization
{
public:
   virtual ~Visualization() = default;

   //! A cyber attack has been initiated
   //! @param aEngagement A pointer to the cyber engagement
   virtual void AttackInitiated(const Engagement& aEngagement) = 0;

   //! A cyber attack succeeded
   //! @param aEngagement A pointer to the cyber engagement
   virtual void AttackSucceeded(const Engagement& aEngagement) = 0;

   //! A cyber attack failed
   //! @param aEngagement A pointer to the cyber engagement
   virtual void AttackFailed(const Engagement& aEngagement) = 0;

   //! A cyber scan has been initiated
   //! @param aEngagement A pointer to the cyber engagement
   virtual void ScanInitiated(const Engagement& aEngagement) = 0;

   //! A cyber scan succeeded
   //! @param aEngagement A pointer to the cyber engagement
   virtual void ScanSucceeded(const Engagement& aEngagement) = 0;

   //! A cyber scan failed
   //! @param aEngagement A pointer to the cyber engagement
   virtual void ScanFailed(const Engagement& aEngagement) = 0;

   //! Get the name of the cyber visualization
   virtual std::string GetName() const = 0;
};

class VisualizationDraw : public Visualization
{
public:
   VisualizationDraw(WsfSimulation* aSimulationPtr);
   ~VisualizationDraw() override = default;

   void AttackInitiated(const Engagement& aEngagement) override;
   void AttackSucceeded(const Engagement& aEngagement) override;
   void AttackFailed(const Engagement& aEngagement) override;

   void ScanInitiated(const Engagement& aEngagement) override;
   void ScanSucceeded(const Engagement& aEngagement) override;
   void ScanFailed(const Engagement& aEngagement) override;

   std::string GetName() const override;

private:
   enum class Outcome
   {
      Successful,
      Failed,
      Unknown
   };

   void Draw(const Engagement&                                              aEngagement,
             const std::function<void(WsfDraw&, WsfGeoPoint, WsfGeoPoint)>& aDrawFunc,
             Outcome                                                        aPercievedOutcome,
             Outcome                                                        aActualOutcome);
   void Draw(WsfPlatform*                                                   aAttacker,
             WsfPlatform*                                                   aVictim,
             const std::function<void(WsfDraw&, WsfGeoPoint, WsfGeoPoint)>& aDrawFunc,
             Outcome                                                        aPercievedOutcome,
             Outcome                                                        aActualOutcome);
   void Draw(WsfGeoPoint                                                    aSource,
             WsfGeoPoint                                                    aDest,
             const std::function<void(WsfDraw&, WsfGeoPoint, WsfGeoPoint)>& aDrawFunc,
             Outcome                                                        aPercievedOutcome,
             Outcome                                                        aActualOutcome);
   void DrawAttack(const Engagement& aEngagement, Outcome aPercievedOutcome, Outcome aActualOutcome);
   void DrawScan(const Engagement& aEngagement, Outcome aPercievedOutcome, Outcome aActualOutcome);

   UtColor GetColor(Outcome aOutcome);

   WsfSimulation* mSimulationPtr;
};

} // namespace cyber
} // namespace wsf

#endif
