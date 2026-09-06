// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFECLIPSEREPORT_HPP
#define WSFECLIPSEREPORT_HPP

#include "wsf_space_export.h"

#include <list>
#include <map>
#include <string>
#include <vector>

#include "UtCalendar.hpp"
#include "UtCallbackHolder.hpp"
#include "WsfEvent.hpp"
#include "WsfScenarioExtension.hpp"
#include "WsfSimulationExtension.hpp"
class WsfSpaceMoverBase;
#include "WsfStringId.hpp"

class UtInput;
class WsfDateTime;
class WsfOrbitalEvent;
class WsfPlatform;
class WsfSimuluation;

//! The data portion of the Eclipse Report class.  It is configured in the scenario extension and
//! then passed into the simulation extension's constructor and populated during runtime.
class WsfEclipseReportData
{
public:
   WsfEclipseReportData();

   struct EclipseInterval
   {
      double mStart;
      double mEnd;
      double mDuration;

      EclipseInterval()
         : mStart(0.0)
         , mEnd(0.0)
         , mDuration(0.0)
      {
      }
   };

   using EclipseIntervalList = std::list<EclipseInterval>;

   using EclipseMap = std::map<WsfStringId, EclipseIntervalList>;

   bool        mReportAll;
   EclipseMap  mEclipseMap;
   std::string mFileName;
   std::string mFormat;
};

//! @brief A utility that produces Eclipse reports.
//! Eclipse Reports provide start time, end time, and the duration the platform is in Earth's shadow.
//! Results do not take into account Earth's oblateness or change in sun's position over one orbital period.
class WSF_SPACE_EXPORT WsfEclipseReport : public WsfSimulationExtension, protected WsfEclipseReportData
{
public:
   explicit WsfEclipseReport(const WsfEclipseReportData& aData);

   bool Initialize() override;

protected:
   void PlatformInitializedHandler(double aSimTime, WsfPlatform* aPlatformPtr);
   void SimulationCompleteHandler(double aSimTime);
   friend class EclipseEvent;

   class EclipseEvent : public WsfEvent
   {
   public:
      enum Type
      {
         cENTRY,
         cEXIT,
         cEVALUATE
      };

      EclipseEvent(Type aType, size_t aId, WsfEclipseReport& aReport, WsfSpaceMoverBase* aSpaceMoverPtr);

      void                       SetType(Type aType) { mType = aType; }
      WsfEvent::EventDisposition Execute() override;

   private:
      Type               mType;
      size_t             mId;
      WsfEclipseReport&  mReport;
      WsfSpaceMoverBase* mSpaceMoverPtr;
      size_t             mPlatformIndex;
   };

private:
   std::string FormatDateTime(const double& aSimTime);

   void InitiateEclipseEvent(double aSimTime, size_t aId, WsfSpaceMoverBase& aSpaceMover);

   void OrbitalManeuverCompleted(double aSimTime, WsfSpaceMoverBase* aSpaceMoverPtr, const WsfOrbitalEvent& aManeuver);

   UtCallbackHolder              mCallbacks;
   UtCalendar                    mSimulationCalendar;
   std::map<WsfStringId, size_t> mPlatformToCurrentEventIdMap;
};

//! The scenario extension used to configure the Eclipse reports object.
class WsfConfigureEclipseReport : public WsfScenarioExtension, public WsfEclipseReportData
{
public:
   bool ProcessInput(UtInput& aInput) override;
   void SimulationCreated(WsfSimulation& aSimulation) override;
};
#endif
