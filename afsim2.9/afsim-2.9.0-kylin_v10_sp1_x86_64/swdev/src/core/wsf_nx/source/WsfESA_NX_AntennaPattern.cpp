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

#include "WsfESA_NX_AntennaPattern.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfSimulation.hpp"

WsfESA_NX_AntennaPattern::WsfESA_NX_AntennaPattern()
   : WsfESA_AntennaPattern(new ESA_NX_Data())
   , mSimulationPtr(nullptr)
   , mAntennaBeamCount(0)
{
}

WsfESA_NX_AntennaPattern::WsfESA_NX_AntennaPattern(const WsfESA_NX_AntennaPattern& aSrc)
   : WsfESA_AntennaPattern(aSrc)
   , mSimulationPtr(aSrc.mSimulationPtr)
   , mAntennaBeamCount(0)
{
}

//! Factory method for WsfESA_NX_AntennaPatternTypes.
//! This is called by WsfESA__NX_AntennaPatternTypes::LoadType to determine if a pattern
//! represented by this class is being requested.
// static
WsfAntennaPattern* WsfESA_NX_AntennaPattern::ObjectFactory(const std::string& aTypeName)
{
   WsfESA_AntennaPattern* patternPtr = nullptr;
   if ((aTypeName == "complex_esa_pattern") || (aTypeName == "steered_array_pattern"))
   {
      patternPtr = new WsfESA_NX_AntennaPattern();
   }
   return patternPtr;
}

// virtual
WsfAntennaPattern* WsfESA_NX_AntennaPattern::Clone() const
{
   return new WsfESA_NX_AntennaPattern(*this);
}

//! Initialize the antenna pattern.
//! This is called after all the input for an antenna pattern has been processed.
//! @returns true if successful or false if not.
// virtual
bool WsfESA_NX_AntennaPattern::Initialize(WsfSimulation* aSimulationPtr)
{
   mSimulationPtr = aSimulationPtr;
   bool ok        = WsfAntennaPattern::Initialize(
      aSimulationPtr); // just call the base class instead of the WsfESA_AntennaPattern::Initialize()

   if (ok && aSimulationPtr)
   {
      AdjustApertureElements(aSimulationPtr->GetRandom());

      if (NX_Data()->mDebug)
      {
         auto logger = ut::log::debug() << "Aperture Efficiency:";
         logger.AddNote() << "X: " << mApertureEffX;
         logger.AddNote() << "Y: " << mApertureEffY;
         logger.AddNote() << "Total: " << mApertureEff;
      }
   }

   return ok;
}

void WsfESA_NX_AntennaPattern::SetAntennaBeamCount(unsigned int aBeamCount)
{
   if (mAntennaBeamCount != aBeamCount)
   {
      mAntennaBeamCount = aBeamCount;
      if (mSimulationPtr != nullptr)
      {
         AdjustApertureElements(mSimulationPtr->GetRandom());
      }
   }
}

// protected virtual
void WsfESA_NX_AntennaPattern::ResizeArray()
{
   double splitRatios[2];
   NX_Data()->GetArraySplitRatios(mAntennaBeamCount, splitRatios[0], splitRatios[1]);
   mLengthX = NX_Data()->mLengthX / splitRatios[0];
   mLengthY = NX_Data()->mLengthY / splitRatios[1];

   mNX = NX_Data()->mNX;
   if (mNX != 1)
   {
      mNX = static_cast<int>(mLengthX / NX_Data()->mdX);
   }

   mNY = NX_Data()->mNY;
   if (mNY != 1)
   {
      mNY = static_cast<int>(mLengthY / NX_Data()->mdY);
   }
}

// protected
void WsfESA_NX_AntennaPattern::AdjustElementLocations()
{
   // Calculate and set element locations
   double alphaA = NX_Data()->mEdgeAngleY - NX_Data()->mEdgeAngleX; // Array inner trapezoidal angle
   double x;
   double y;

   double xOffset = 0.0;
   if (NX_Data()->mLatticeType == cLT_TRIANGLULAR)
   {
      xOffset = NX_Data()->mdX / 4.0;
   }

   for (int j = 0; j < mNY; ++j)
   {
      xOffset = -xOffset;
      for (int i = 0; i < mNX; ++i)
      {
         Element element;
         y = ((2.0 * (j + 1) - (mNY + 1)) / 2.0) * NX_Data()->mdY * sin(alphaA);
         x = ((2.0 * (i + 1) - (mNX + 1)) / 2.0) * NX_Data()->mdX + y * (cos(alphaA) / sin(alphaA)) + xOffset;
         element.mLocation[0] = x * cos(NX_Data()->mEdgeAngleX) - y * sin(NX_Data()->mEdgeAngleX);
         element.mLocation[1] = x * sin(NX_Data()->mEdgeAngleX) + y * cos(NX_Data()->mEdgeAngleX);
         element.mLocation[2] = 0.0;
         mElements.push_back(element);
      }
   }
}

// This method includes the efficiency in retrospect to the ComputeElementFactor(..) method.
// It is expected to be used when NOT being used in conjunction with the ComputeArrayFactor(...) method.
// private
double WsfESA_NX_AntennaPattern::ComputeDirectivityGain(double aFrequency, double aEBS_Az, double aEBS_El) const
{
   // The full equation for directivity gain is as follows:
   // double directivityGain = (4.0 * UtMath::cPI * sin(mEdgeAngleY - mEdgeAngleX) * (mNX * mdX) * (mNY * mdY)) /
   // pow(lambda, 2.0); It is divided up here to account for different conditions and array configurations
   return WsfESA_AntennaPattern::ComputeDirectivityGain(aFrequency, aEBS_Az, aEBS_El) *
          sin(NX_Data()->mEdgeAngleY - NX_Data()->mEdgeAngleX);
}

// =================================================================================================
// Nested class 'ESA_NX_Data'.
// =================================================================================================
WsfESA_NX_AntennaPattern::ESA_NX_Data::ESA_NX_Data()
   : ESA_Data()
   , mEdgeAngleX(0.0)
   , mEdgeAngleY(UtMath::cPI_OVER_2)
   , mArraySplitRatios()
{
   mArraySplitRatios[0].X_Ratio = 1.; // Set the default value
   mArraySplitRatios[0].Y_Ratio = 1.; // Set the default value
}

// virtual
bool WsfESA_NX_AntennaPattern::ESA_NX_Data::ProcessInput(WsfAntennaPattern& aAntennaPattern, UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "array_subdivision_table")
   {
      mArraySplitRatios.clear();
      mArraySplitRatios[0].X_Ratio = 1; // Set the default value since it was cleared
      mArraySplitRatios[0].Y_Ratio = 1; // Set the default value since it was cleared
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (command == "beam_count")
         {
            unsigned int beamCount;
            double       x_ratio;
            double       y_ratio;
            aInput.ReadValue(beamCount);
            aInput.ValueInClosedRange(static_cast<double>(beamCount),
                                      static_cast<double>(mArraySplitRatios.size()),
                                      static_cast<double>(mArraySplitRatios.size()));

            aInput.ReadValue(x_ratio);
            aInput.ValueGreater(x_ratio, 0.0);
            mArraySplitRatios[beamCount].X_Ratio = x_ratio;

            aInput.ReadValue(y_ratio);
            aInput.ValueGreater(y_ratio, 0.0);
            mArraySplitRatios[beamCount].Y_Ratio = y_ratio;
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (command == "edge_angle_x")
   {
      aInput.ReadValueOfType(mEdgeAngleX, UtInput::cANGLE);
      aInput.ValueInClosedRange(mEdgeAngleX, -180.0, 180.0);
   }
   else if (command == "edge_angle_y")
   {
      aInput.ReadValueOfType(mEdgeAngleY, UtInput::cANGLE);
      aInput.ValueInClosedRange(mEdgeAngleY, -180.0, 180.0);
   }
   else
   {
      myCommand = WsfESA_AntennaPattern::ESA_Data::ProcessInput(aAntennaPattern, aInput);
   }
   return myCommand;
}

// virtual
bool WsfESA_NX_AntennaPattern::ESA_NX_Data::Initialize(WsfAntennaPattern& aAntennaPattern)
{
   return WsfESA_AntennaPattern::ESA_Data::Initialize(aAntennaPattern);
}

void WsfESA_NX_AntennaPattern::ESA_NX_Data::GetArraySplitRatios(unsigned int aBeamCount,
                                                                double&      aX_SplitRatio,
                                                                double&      aY_SplitRatio)
{
   aX_SplitRatio         = mArraySplitRatios[0].X_Ratio;
   aY_SplitRatio         = mArraySplitRatios[0].Y_Ratio;
   unsigned int maxIndex = static_cast<unsigned int>(mArraySplitRatios.size()) - 1;
   if (aBeamCount < maxIndex)
   {
      aX_SplitRatio = mArraySplitRatios[aBeamCount].X_Ratio;
      aY_SplitRatio = mArraySplitRatios[aBeamCount].Y_Ratio;
   }
   else if (aBeamCount > maxIndex)
   {
      aX_SplitRatio = mArraySplitRatios[maxIndex].X_Ratio;
      aY_SplitRatio = mArraySplitRatios[maxIndex].Y_Ratio;
   }
}
