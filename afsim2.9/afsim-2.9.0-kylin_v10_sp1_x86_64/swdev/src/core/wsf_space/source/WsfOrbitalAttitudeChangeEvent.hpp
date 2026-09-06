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

#ifndef WSFORBITALATTITUDECHANGEEVENT_HPP
#define WSFORBITALATTITUDECHANGEEVENT_HPP

#include "wsf_space_export.h"

#include <memory>

class UtCalendar;
class UtInput;
#include "WsfOrbitalEvent.hpp"
#include "WsfSpaceOrientation.hpp"
#include "WsfStringId.hpp"

//! An orbital event to change the attitude of the spacecraft to one of the standard types
//! (see wsf::space::Orientation).
class WSF_SPACE_EXPORT WsfOrbitalAttitudeChangeEvent : public WsfOrbitalEvent
{
public:
   WsfOrbitalAttitudeChangeEvent(const WsfScenario& aScenario);
   WsfOrbitalAttitudeChangeEvent(const WsfOrbitalAttitudeChangeEvent& aSrc);
   ~WsfOrbitalAttitudeChangeEvent() override = default;

   WsfOrbitalEvent* Clone() const override;
   bool             ProcessInput(UtInput& aInput) override;
   bool             Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext) override;

   const char*        GetScriptClassName() const override { return "WsfChangeAttitude"; }
   static WsfStringId GetTypeId();

   void                     SetOrientation(std::unique_ptr<wsf::space::Orientation> aOrientationPtr);
   wsf::space::Orientation* GetOrientation() const;

   void Accept(WsfOrbitalEventVisitor* aVisitorPtr) override;

   bool ValidateParameterRanges(const std::string& aExecutingPlatformName) const override;

protected:
   bool ExecuteEvent(const UtCalendar& aEpoch, wsf::space::OrbitalMissionContext& aContext) override;

   std::unique_ptr<wsf::space::Orientation> mOrientationPtr{nullptr};
};

#endif
