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
#ifndef WKFVAENVIRONMENT_HPP
#define WKFVAENVIRONMENT_HPP

#include "VaEnvironment.hpp"

class UtCalendar;
#include "wkf_export.h"

namespace vespa
{
class VaViewer;
class VaWidget;
} // namespace vespa

#include "WkfFactory.hpp"

#define vaEnv wkf::VtkEnvironment::Instance()

namespace wkf
{
class Scenario;
class Viewer;

class WKF_EXPORT VtkEnvironment : public vespa::VaEnvironment
{
public:
   //! Provides a global access point for the VESPA environment object.
   static VtkEnvironment& Instance();

   static void Create(Factory* aFactoryPtr, const std::string& aScenarioType = "wkf");

   const Factory* GetFactory() const { return static_cast<const Factory*>(vespa::VaEnvironment::GetFactory()); }

   void      SetStandardScenario(Scenario* aScenarioPtr);
   Scenario* GetStandardScenario() const { return mStandardScenarioPtr; }
   void      ResetStandardScenario();

   // this relates to the standard viewer (the one generated from a central map display)
   void         SetStandardViewer(wkf::Viewer* aViewerPtr);
   wkf::Viewer* GetStandardViewer() const { return mStandardViewerPtr; }

   void             SetStandardWidget(vespa::VaWidget* aWidgetPtr);
   vespa::VaWidget* GetStandardWidget() const { return mStandardWidgetPtr; }

   vespa::VaScenario* NewScenario(const std::string& aScenarioType) override;

   static void InitializeOSG_Path();

   void EnableBoxSelection();
   void DisableBoxSelection();

   void EnablePlatformsDraggable(bool aDraggable);

   void HandleSelectionBoxCB(vespa::VaViewer* aViewerPtr, const vespa::VaSelectionBox& aSelectionBox);

   bool DeletePlatformByIndex(unsigned int aIndex);

   void   SetStartTime(const UtCalendar& aCalendar);
   double GetDisplayedTime();

protected:
   VtkEnvironment(Factory* aFactoryPtr, const std::string& aScenarioType);

private:
   void HandleScenarioRemovedCB(vespa::VaScenario* aScenario);
   void HandleViewerDestroyedCB(vespa::VaViewer* aViewer);

   bool mEnableDraggableMovement;

   // These are managed by the base VaEnvironment class & VaViewerManager class,
   //  thus we do not delete these within this class.
   std::string      mScenarioType;
   Scenario*        mStandardScenarioPtr;
   Viewer*          mStandardViewerPtr;
   vespa::VaWidget* mStandardWidgetPtr{nullptr};
};
} // namespace wkf
#endif
