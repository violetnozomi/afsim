// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef MAPPLOTVARIABLE_HPP
#define MAPPLOTVARIABLE_HPP

class WsfSensor;
class WsfSensorResult;
class WsfSimulation;
class UtInput;
class MapPlotVariables;

//! Pure abstract interface extension class to be used by MapPlotFunction types for invoking
//! runtime defined plotting computations.
//!
//! This interface type allows users to define custom plotting routines in horizontal_map,
//! vertical_map, and spherical_map.
class MapPlotVariable
{
public:
   MapPlotVariable(double aInitValue)
      : mInitValue(aInitValue)
   {
   }
   virtual ~MapPlotVariable() = default;

   class SharedData
   {
   public:
      //! Set accessor for private member data mCrossRange.
      void SetCrossRange(double aCrossRange) { mCrossRange = aCrossRange; }

      //! Set accessor for private member data mDownRange.
      void SetDownRange(double aDownRange) { mDownRange = aDownRange; }

      //! Set accessor for private member data mRangeScale.
      void SetRangeScale(double aRangeScale) { mRangeScale = aRangeScale; }

      //! Get accessor for private member data mCrossRange.
      double GetCrossRange() const { return mCrossRange; }

      //! Get accessor for private member data mDownRange.
      double GetDownRange() const { return mDownRange; }

      //! Get accessor for private member data mRangeScale.
      double GetRangeScale() const { return mRangeScale; }

   private:
      double mCrossRange{0.0};
      double mDownRange{0.0};
      double mRangeScale{1.0};
   };

   //! Get the default value for this computation type.
   //!
   //! @return Default value (in units known only to the derived type).
   double GetInitialValue() const { return mInitValue; };

   //! Process an input for the variable.
   //!
   //! @param aInput Check and process this block of script.
   //! @return True if the script block was processed by this extension, False if the extension ignored the script
   //! block.
   virtual bool ProcessInput(UtInput& aInput) { return false; }

   //! Initializes the map plot variable.
   //!
   //! @param  aSimulation    The currently active simulation.
   //! @param  aSharedData    The shared data across the variables for sharing common data set.
   //! @return True by default, False if failed initialization in a derived class.
   virtual bool Initialize(WsfSimulation& aSimulation, const SharedData& aSharedData)
   {
      mSharedDataPtr = &aSharedData;
      return true;
   }

   //! Performs the variable's evaluation logic between the sensor and target.
   //!
   //! @param aSimTime           The current time of the simulation in seconds since the beginning of the simulation.
   //! @param aSimulationPtr     The simulation currently being executed.
   //! @param aValue             The current or initial value of the variable.
   //! @param aSensorPtr         The sensor performing the detection attempt of the target.
   //! @param aResult            The electronic warfare result from the corresponding WsfEM_Interaction.
   //! @return                   The custom value computed by the extension.
   virtual double Evaluate(double           aSimTime,
                           WsfSimulation*   aSimulationPtr,
                           double           aValue,
                           WsfSensor*       aSensorPtr,
                           WsfSensorResult& aResult) const = 0;

protected:
   const SharedData* mSharedDataPtr{nullptr};

private:
   const double mInitValue{0.0};
};

#endif
