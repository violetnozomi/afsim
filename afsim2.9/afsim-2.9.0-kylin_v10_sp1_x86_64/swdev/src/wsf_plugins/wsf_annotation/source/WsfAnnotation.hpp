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

#ifndef WSFANNOTATION_HPP
#define WSFANNOTATION_HPP

#include "wsf_export.h"

#include "UtColor.hpp"
#include "UtInput.hpp"
#include "UtUnitTypes.hpp"
#include "WsfAnnotationDataTypes.hpp"
#include "WsfScenarioExtension.hpp"
#include "WsfSimulationExtension.hpp"

class WsfAnnotationInput
{
public:
   bool                           ProcessAnnotationInput(UtInput& aInput);
   void                           ProcessDecorationInput(UtInput& aInput);
   void                           ProcessPoiInput(UtInput& aInput);
   void                           ProcessRangeRingInput(UtInput& aInput);
   AnnotationData::AnnotationInfo GetAnnotationInfo() const { return mAnnotationInfo; }

protected:
   AnnotationData::AnnotationInfo mAnnotationInfo;
};

class WsfAnnotationExtension : public WsfScenarioExtension, public WsfAnnotationInput
{
public:
   WsfAnnotationExtension() {}
   void AddedToScenario() override;
   bool ProcessInput(UtInput& aInput) override;
   void SimulationCreated(WsfSimulation& aSimulation) override;
};

// Simulation Extension unnecessary since the scenario will never be directly modified; even on restart the annotations
// will remain the same

#endif
