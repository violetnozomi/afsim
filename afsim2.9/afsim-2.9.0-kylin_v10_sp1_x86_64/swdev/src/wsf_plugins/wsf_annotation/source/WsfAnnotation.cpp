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

#include "WsfAnnotation.hpp"

#include "wsf_annotation_export.h"

#include "UtEllipsoidalEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "WsfAnnotationEventPipe.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfPlugin.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

namespace
{
const std::string cEXTENSION_NAME = "wsf_annotation";

class AnnotationApplicationExtension : public WsfApplicationExtension
{
public:
   void ScenarioCreated(WsfScenario& aScenario) override
   {
      aScenario.RegisterExtension(GetExtensionName(), ut::make_unique<WsfAnnotationExtension>());
   }
};

// Length and angle units can be converted prior to validation, so use custom validators instead of the UtInput
// built-ins for error message clarity
void ValidateDouble(UtInput& aInput, double aLength, double aNum, const std::string& aType, const std::string& aUnits)
{
   if (aLength < aNum)
   {
      throw UtInput::BadValue(aInput,
                              aType + " must have a value greater than " + std::to_string(aNum) + " " + aUnits + ".");
   }
}

template<class T>
void ValidateBounds(UtInput& aInput, T aNum, T aMin, T aMax, const std::string& aType, const std::string& aUnits = "")
{
   if (aNum < aMin || aNum > aMax)
   {
      throw UtInput::BadValue(aInput,
                              aType + " must be between " + std::to_string(aMin) + " and " + std::to_string(aMax) +
                                 (aUnits.empty() ? "" : (" " + aUnits)) + " inclusive.");
   }
}
} // namespace

void WsfAnnotationExtension::AddedToScenario()
{
   WsfEventPipeExtension* eventPipePtr = WsfEventPipeExtension::Find(GetScenario());
   if (eventPipePtr)
   {
      wsf::annotation::EventPipe::RegisterEvents(*eventPipePtr);
   }
}

bool WsfAnnotationExtension::ProcessInput(UtInput& aInput)
{
   return WsfAnnotationInput::ProcessAnnotationInput(aInput);
}

void WsfAnnotationExtension::SimulationCreated(WsfSimulation& aSimulation)
{
   aSimulation.RegisterExtension("wsf_annotation_eventpipe",
                                 ut::make_unique<wsf::annotation::EventPipeInterface>(mAnnotationInfo));
}

void WSF_ANNOTATION_EXPORT Register_wsf_annotation(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered(cEXTENSION_NAME))
   {
      aApplication.RegisterFeature("annotation", cEXTENSION_NAME);
      aApplication.RegisterExtension(cEXTENSION_NAME, ut::make_unique<AnnotationApplicationExtension>());
      aApplication.ExtensionDepends(cEXTENSION_NAME, "event_pipe", false);
   }
}

extern "C"
{
   UT_PLUGIN_EXPORT void WsfPluginVersion(UtPluginVersion& aVersion)
   {
      aVersion =
         UtPluginVersion(WSF_PLUGIN_API_MAJOR_VERSION, WSF_PLUGIN_API_MINOR_VERSION, WSF_PLUGIN_API_COMPILER_STRING);
   }

   UT_PLUGIN_EXPORT void WsfPluginSetup(WsfApplication& aApplication) { Register_wsf_annotation(aApplication); }
}

bool WsfAnnotationInput::ProcessAnnotationInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string cmd       = aInput.GetCommand();
   if (cmd == "visual_elements")
   {
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand())
      {
         std::string command(aInput.GetCommand());
         if (command == "decoration")
         {
            ProcessDecorationInput(aInput);
         }
         else if (command == "poi")
         {
            ProcessPoiInput(aInput);
         }
         else if (command == "range_ring")
         {
            ProcessRangeRingInput(aInput);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

void WsfAnnotationInput::ProcessDecorationInput(UtInput& aInput)
{
   AnnotationData::DecorationData decInfo;
   aInput.ReadValue(decInfo.mEntityName);
   auto duplicateIt = std::find_if(mAnnotationInfo.mDecorations.begin(),
                                   mAnnotationInfo.mDecorations.end(),
                                   [decInfo](const AnnotationData::DecorationData& aData)
                                   { return aData.mEntityName == decInfo.mEntityName; });
   if (duplicateIt != mAnnotationInfo.mDecorations.end())
   {
      throw UtInput::BadValue(aInput, "A decoration has already been defined on " + decInfo.mEntityName + ".");
   }
   UtInputBlock inputBlock(aInput);
   while (inputBlock.ReadCommand())
   {
      std::string command(aInput.GetCommand());
      if (command == "text")
      {
         decInfo.mType = AnnotationData::AnnotationType::cTEXT;
         aInput.ReadValueQuoted(decInfo.mInfo);
      }
      else if (command == "preset")
      {
         decInfo.mType = AnnotationData::AnnotationType::cPRESET;
         aInput.ReadValue(decInfo.mInfo);
      }
      else if (command == "image")
      {
         decInfo.mType = AnnotationData::AnnotationType::cIMAGE;
         aInput.ReadValueQuoted(decInfo.mInfo);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   mAnnotationInfo.mDecorations.push_back(decInfo);
}

void WsfAnnotationInput::ProcessPoiInput(UtInput& aInput)
{
   AnnotationData::PoiData poiInfo;
   aInput.ReadValue(poiInfo.mPoiName);
   // Cannot have identical poi names
   auto duplicateIt =
      std::find_if(mAnnotationInfo.mPois.begin(),
                   mAnnotationInfo.mPois.end(),
                   [poiInfo](const AnnotationData::PoiData& aData) { return aData.mPoiName == poiInfo.mPoiName; });
   if (duplicateIt != mAnnotationInfo.mPois.end())
   {
      throw UtInput::BadValue(aInput, poiInfo.mPoiName + " has already been defined in the scenario.");
   }
   UtInputBlock inputBlock(aInput);
   while (inputBlock.ReadCommand())
   {
      std::string command(aInput.GetCommand());
      if (command == "position")
      {
         aInput.ReadValueOfType(poiInfo.mLat, UtInput::cLATITUDE);
         aInput.ReadValueOfType(poiInfo.mLon, UtInput::cLONGITUDE);
      }
      else if (command == "mgrs_coordinate")
      {
         std::string mgrsValue;
         aInput.ReadValue(mgrsValue);
         if (!UtEllipsoidalEarth::ConvertMGRSToLL(mgrsValue, poiInfo.mLat, poiInfo.mLon))
         {
            throw UtInput::BadValue(aInput, "Unable to convert MGRS location: " + mgrsValue);
         }
      }
      else if (command == "altitude")
      {
         aInput.ReadValueOfType(poiInfo.mAlt, UtInput::cLENGTH);
      }
      else if (command == "bullseye")
      {
         poiInfo.mIsBullseye = true;
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   if (poiInfo.mIsBullseye && poiInfo.mAlt != 0)
   {
      throw UtInput::BadValue(aInput, "Bullseyes must have an altitude of 0 m.");
   }
   mAnnotationInfo.mPois.push_back(poiInfo);
}

void WsfAnnotationInput::ProcessRangeRingInput(UtInput& aInput)
{
   AnnotationData::RangeRingData ringInfo;
   aInput.ReadValue(ringInfo.mRingName);
   UtInputBlock inputBlock(aInput);
   while (inputBlock.ReadCommand())
   {
      std::string command(aInput.GetCommand());
      if (command == "entity")
      {
         aInput.ReadValue(ringInfo.mReferenceName);
         auto it = std::find_if(mAnnotationInfo.mRangeRings.begin(),
                                mAnnotationInfo.mRangeRings.end(),
                                [ringInfo](const AnnotationData::RangeRingData& aInfo)
                                { return ringInfo.mRingName == aInfo.mRingName; });
         if (it != mAnnotationInfo.mRangeRings.end())
         {
            throw UtInput::BadValue(aInput, "A range ring with name " + ringInfo.mRingName + " has already been defined.");
         }
      }
      else if (command == "show_range_labels")
      {
         ringInfo.mShowRangeLabels = true;
      }
      else if (command == "range_label_color")
      {
         aInput.ReadValue(ringInfo.mRangeLabelColor);
      }
      else if (command == "ring_color")
      {
         aInput.ReadValue(ringInfo.mRingColor);
      }
      else if (command == "ring_width")
      {
         aInput.ReadValue(ringInfo.mRingWidth);
         ValidateBounds<int>(aInput, ringInfo.mRingWidth, 1, 5, "ring_width");
      }
      else if (command == "center_radius")
      {
         aInput.ReadValueOfType(ringInfo.mCenterRadius, UtInput::cLENGTH);
         ValidateDouble(aInput, ringInfo.mCenterRadius, 1.0, "center_radius", "meters");
      }
      else if (command == "additional_rings")
      {
         aInput.ReadValue(ringInfo.mAdditionalRings);
         if (ringInfo.mAdditionalRings < 0 || ringInfo.mAdditionalRings > 20)
         {
            throw UtInput::BadValue(aInput, "Range rings can have a maximum of 20 additional rings and a minimum of 0.");
         }
      }
      else if (command == "additional_radii")
      {
         aInput.ReadValueOfType(ringInfo.mAdditionalRadii, UtInput::cLENGTH);
         ValidateDouble(aInput, ringInfo.mAdditionalRadii, 1.0, "additional_radii", "meters");
      }
      else if (command == "show_angle_labels")
      {
         ringInfo.mShowAngleLabels = true;
      }
      else if (command == "angle_label_color")
      {
         aInput.ReadValue(ringInfo.mAngleLabelColor);
      }
      else if (command == "radial_color")
      {
         aInput.ReadValue(ringInfo.mRadialColor);
      }
      else if (command == "radial_width")
      {
         aInput.ReadValue(ringInfo.mRadialWidth);
         ValidateBounds<int>(aInput, ringInfo.mRadialWidth, 1, 5, "radial_width");
      }
      else if (command == "center_angle")
      {
         aInput.ReadValueOfType(ringInfo.mCenterAngle, UtInput::cANGLE);
         ValidateBounds<double>(aInput, ringInfo.mCenterAngle, 0.0, UtMath::cTWO_PI, "center_angle", "radians");
      }
      else if (command == "angle_width")
      {
         aInput.ReadValueOfType(ringInfo.mAngleWidth, UtInput::cANGLE);
         ValidateBounds<double>(aInput, ringInfo.mAngleWidth, 0.0, UtMath::cTWO_PI, "angle_width", "radians");
      }
      else if (command == "radial_rate")
      {
         aInput.ReadValueOfType(ringInfo.mRadialRate, UtInput::cANGLE);
         ValidateBounds<double>(aInput, ringInfo.mRadialRate, 0.0, UtMath::cTWO_PI, "radial_rate", "radians");
      }
      else if (command == "align_north")
      {
         ringInfo.mAlignPlatform = false;
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   mAnnotationInfo.mRangeRings.push_back(ringInfo);
}
