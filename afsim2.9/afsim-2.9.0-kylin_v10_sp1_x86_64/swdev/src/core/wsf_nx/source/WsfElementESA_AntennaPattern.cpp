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

#include "WsfElementESA_AntennaPattern.hpp"

#include <algorithm>
#include <cfloat> // for DBL_MAX
#include <cmath>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"
#include "WsfSimulation.hpp"
#include "WsfStandardAntennaPattern.hpp"

WsfElementESA_AntennaPattern::WsfElementESA_AntennaPattern()
   : WsfESA_AntennaPattern(new EESA_Data)
   , mExplicitApertureEff(false)
   , mExplicitApertureEffXY(false)
{
}

//! Factory method for WsfAntennaPatternTypes.
//! This is called by WsfAntennaPatternTypes::LoadType to determine if a pattern
//! represented by this class is being requested.
// static
WsfAntennaPattern* WsfElementESA_AntennaPattern::ObjectFactory(const std::string& aTypeName)
{
   WsfAntennaPattern* patternPtr = nullptr;
   if (aTypeName == "element_esa_pattern")
   {
      patternPtr = new WsfElementESA_AntennaPattern();
   }
   return patternPtr;
}

// virtual
WsfAntennaPattern* WsfElementESA_AntennaPattern::Clone() const
{
   return new WsfElementESA_AntennaPattern(*this);
}

//! Initialize the antenna pattern.
//! This is called after all the input for an antenna pattern has been processed.
//! @param aSimulationPtr A pointer to the simulation if this antenna pattern is going to be used in a simulation
//!                       may be null to perform quick checks after inputs are read
//! @returns true if successful or false if not.
// virtual
bool WsfElementESA_AntennaPattern::Initialize(WsfSimulation* aSimulationPtr)
{
   bool ok = WsfAntennaPattern::Initialize(aSimulationPtr);

   if (ok && aSimulationPtr != nullptr)
   {
      mLengthX = GetEESA_Data()->mLengthX;
      mLengthY = GetEESA_Data()->mLengthY;
      mNX      = GetEESA_Data()->mNX;
      mNY      = GetEESA_Data()->mNY;

      // Order is important in the calling of these methods currently
      // TODO - put checks in each to call the ones required.
      ComputeFailedModulesWeights(aSimulationPtr->GetRandom());
      ComputeApertureEfficiency();

      if (GetEESA_Data()->mDebug)
      {
         auto logger = ut::log::debug() << "Aperture Efficiency:";
         logger.AddNote() << "X: " << mApertureEffX;
         logger.AddNote() << "Y: " << mApertureEffY;
         logger.AddNote() << "Total: " << mApertureEff;
      }
   }

   return ok;
}

// virtual
bool WsfElementESA_AntennaPattern::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "element_locations")
   {
      Element element;
      mElements.clear();
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         aInput.PushBack(command);
         double xyzLoc[3];
         double normal[2];
         double elementWeight;
         aInput.ReadValueOfType(xyzLoc[0], UtInput::cLENGTH);
         aInput.ReadValueOfType(xyzLoc[1], UtInput::cLENGTH);
         aInput.ReadValueOfType(xyzLoc[2], UtInput::cLENGTH);
         element.mLocation.Set(xyzLoc);

         aInput.ReadValueOfType(normal[0], UtInput::cANGLE);
         aInput.ReadValueOfType(normal[1], UtInput::cANGLE);
         element.mNormal.Set(normal);

         aInput.ReadValue(elementWeight);
         element.mWeight = elementWeight;

         mElements.push_back(element);
      }
   }
   else if (command == "aperture_efficiencies")
   {
      aInput.ReadValue(mApertureEffX);
      aInput.ValueGreater(mApertureEffX, 0.0);
      aInput.ReadValue(mApertureEffY);
      aInput.ValueGreater(mApertureEffY, 0.0);
      mExplicitApertureEffXY = true;
   }
   else if (command == "aperture_efficiency")
   {
      aInput.ReadValue(mApertureEff);
      aInput.ValueGreater(mApertureEff, 0.0);
      mExplicitApertureEff = true;
   }
   else
   {
      myCommand = WsfESA_AntennaPattern::ProcessInput(aInput);
   }
   return myCommand;
}

WsfElementESA_AntennaPattern::WsfElementESA_AntennaPattern(const WsfElementESA_AntennaPattern& aSrc)
   : WsfESA_AntennaPattern(aSrc)
{
   // Base class constructor does not copy the element information
   // so it is copied here or it is lost
   mElements = aSrc.mElements;
}

double WsfElementESA_AntennaPattern::ComputeElementGain(double aFrequency, double aAzAngle, double aElAngle) const
{
   double elementGain(0.0);
   for (const auto& elem : mElements)
   {
      elementGain += mSharedDataPtr->GetGain(aFrequency, aAzAngle - elem.mNormal[0], aElAngle - elem.mNormal[1], 0.0, 0.0);
   }
   elementGain /= (mNX * mNY);

   return elementGain;
}

// This method includes the efficiency in retrospect to the ComputeElementFactor(..) method.
// It is expected to be used when NOT being used in conjunction with the ComputeArrayFactor(...) method.
// private
double WsfElementESA_AntennaPattern::ComputeDirectivityGain(double aFrequency, double aEBS_Az, double aEBS_El) const
{
   // The full equation for directivity gain is as follows:
   // double directivityGain = (4.0 * UtMath::cPI * (mNX * mdX) * (mNY * mdY)) / pow(lambda, 2.0);
   // It is divided up here to account for different conditions and array configurations

   // assume lambda/2 spacing and matched frequency
   double directivityGain = UtMath::cPI * mNX * mNY;
   if (aFrequency > 0.0)
   {
      double lambda = UtMath::cLIGHT_SPEED / aFrequency;
      if (mNX != 1)
      {
         // multiply in the x direction directivity gain part
         directivityGain *= (2.0 * GetEESA_Data()->mdX / lambda);
      }

      if (mNY != 1)
      {
         // multiply in the x direction directivity gain part
         directivityGain *= (2.0 * GetEESA_Data()->mdY / lambda);
      }
   }

   // NOTE: The Array gain is being applied here consists of only the Gain portion according to
   // "Skolnik "Radar Handbook", 2nd ed. Chapter 7, Phased Array Radar Antennas, the steering loss is applied in
   // WsfEM_Antenna::ComputeBeamSteeringLoss outside this class. This allows it to be extended to other
   // antenna pattern classes. An example of how this would be applied is below.
   // directivityGain *= pow(cos(aEBS_Az), loss_exponent) * pow(cos(aEBS_El), loss_exponent);

   return directivityGain * mApertureEff;
}

// virtual
double WsfElementESA_AntennaPattern::ComputeApertureEfficiency()
{
   if (mExplicitApertureEffXY && (!mExplicitApertureEff))
   {
      mApertureEff = mApertureEffX * mApertureEffY;
   }
   else if ((!mExplicitApertureEffXY) && mExplicitApertureEff)
   {
      mApertureEffX = mApertureEff + (mLengthX / (mLengthX + mLengthY)) * (1.0 - mApertureEff);
      mApertureEffY = mApertureEff + (mLengthY / (mLengthX + mLengthY)) * (1.0 - mApertureEff);
   }
   else if ((!mExplicitApertureEffXY) && (!mExplicitApertureEff))
   {
      // Efficiency
      double Amn  = 0.0;
      double AmnS = 0.0;
      size_t numElements(mElements.size());
      for (size_t i = 0; i < numElements; ++i)
      {
         Amn += sqrt(mElements[i].mWeight * mElements[i].mWeight);
         AmnS += mElements[i].mWeight * mElements[i].mWeight;
      }
      mApertureEff = pow(Amn, 2.0) / (static_cast<double>(numElements) * AmnS);

      mApertureEffX = mApertureEff + (mLengthX / (mLengthX + mLengthY)) * (1.0 - mApertureEff);
      mApertureEffY = mApertureEff + (mLengthY / (mLengthX + mLengthY)) * (1.0 - mApertureEff);
   }

   return mApertureEff;
}

// =================================================================================================
// Nested class 'ESA_Data'.
// =================================================================================================
WsfElementESA_AntennaPattern::EESA_Data::EESA_Data()
   : ESA_Data()
{
}

// virtual
bool WsfElementESA_AntennaPattern::EESA_Data::ProcessInput(WsfAntennaPattern& aPattern, UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "average_element_spacing_x")
   {
      aInput.ReadValueOfType(mdX, UtInput::cLENGTH);
      aInput.ValueGreater(mdX, 0.0);
   }
   else if (command == "average_element_spacing_y")
   {
      aInput.ReadValueOfType(mdY, UtInput::cLENGTH);
      aInput.ValueGreater(mdY, 0.0);
   }
   else if (command == "distribution_type")
   {
      throw UtInput::BadValue(aInput, "Distribution type not used for Element-Wise ESA pattern types.");
   }
   else
   {
      myCommand = WsfESA_AntennaPattern::ESA_Data::ProcessInput(aPattern, aInput);
   }
   return myCommand;
}
