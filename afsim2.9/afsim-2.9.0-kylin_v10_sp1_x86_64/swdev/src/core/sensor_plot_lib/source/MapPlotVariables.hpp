// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef MAPVARIABLES_HPP
#define MAPVARIABLES_HPP

#include "sensor_plot_lib_export.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

class UtInput;

#include "MapPlotVariable.hpp"
class WsfSensor;
class WsfSensorResult;
class WsfSimulation;

//! A class that defines plot variables that are common between horizontal and vertical maps.
//!
//! The horizontal_map and vertical_map functions have many plot variables in common.
//! Rather than replicated the code, this provides the processing that can be shared.
class SENSOR_PLOT_LIB_EXPORT MapPlotVariables : public MapPlotVariable::SharedData
{
public:
   using MapPlotVariableMap = std::map<std::string, std::shared_ptr<MapPlotVariable>>;

   MapPlotVariables();
   MapPlotVariables(const MapPlotVariables&) = default;
   MapPlotVariables& operator=(const MapPlotVariables&) = default;
   ~MapPlotVariables()                                  = default;

   //! Check if UtInput contains a script block type defined by this extension and if so processes the data.
   //!
   //! @param aInput Check and process this block of script.
   //! @return True if the script block was processed by this extension, False if the extension ignored the script
   //! block.
   bool ProcessInput(UtInput& aInput);

   //! Add a user defined script variable to the master list (MapPlotVariableMap) and add its name to mVarLabels
   //!
   //! @param aVarName        The name of the script variable.
   //! @param aVarInitValue   The initial value for the script variable.
   //! @return                True if the script is added to the list, False otherwise.
   bool ProcessScriptVariableInput(const std::string& aVarName, double aVarInitValue);

   //! Return the number of variables defined.
   //!
   //! @return The length of the vector of strings mVarLabels
   size_t Count() const { return mVarLabels.size(); }

   //! Calls the MapPlotVariable::Evaluate in each instance of MapPlotVariable derived classes.
   //!
   //! @param aSimTime     Current simulation time, internal.
   //! @param aSensorPtr   A pointer to the sensor object undergoing evaluation.
   //! @param aResult      Resultant value of previous sensor process.
   //! @param aVarValues   Initial value for a given evaluation.
   void Evaluate(double aSimTime, WsfSensor* aSensorPtr, WsfSensorResult& aResult, std::vector<double>& aVarValues);

   //! Initializes member variables in MapPlotVariable derived classes.
   //!
   //! @param  aSimulation The currently active simulation.
   //! @return True by default, False if failed initialization in a derived class.
   bool Initialize(WsfSimulation& aSimulation);

   //! Initializes dynamic member values in MapPlotVariable derived classes.
   //!
   //! @param aSimulation     The currently active simulation.
   //! @param aVarValues      A vector containing variable values for each derived class.
   //! @param aUserInitValues Initialization values.
   void InitializeValues(WsfSimulation&             aSimulation,
                         std::vector<double>&       aVarValues,
                         const std::vector<double>& aUserInitValues);

   //! Adds a new map plot variable ID to mVarLabels. ID must already be present in mVariableMap.
   //!
   //! @param aVarLabel the ID of the new map plot variable, added to the end of mVarLabels.
   void AddVariable(const std::string& aVarLabel);

   //! Get the MapPlotVariable ID (label) at a given index.
   //!
   //! @param  aIndex   Index of the desired ID.
   //! @return          The MapPlotVariable ID (label) at the given index.
   const std::string& GetLabel(size_t aIndex) const { return mVarLabels.at(aIndex); }

   //! Get the MapPlotVariable Index at a given ID (label)
   //!
   //! @param  aLabel   Label of the desired index.
   //! @return          Index of the input label.
   size_t GetLabelPosition(const std::string& aLabel) const;

   //! Status checker function.
   //!
   //! @param  aResult Subject result to be evaluated
   //! @return         True if aResult reports that its status has been checked & reports a valid status, False otherwise.
   static bool CheckSucceeded(WsfSensorResult& aResult);

   //! Registration of new Map Plot Variables.
   //!
   //! @param aVarName     The name of the variable Label
   //! @param aVariablePtr Pointer to MapPlotVariable derived class instance mapped by variable label
   void RegisterMapPlotVariable(const std::string& aVarName, std::shared_ptr<MapPlotVariable> aVariablePtr);

private:
   WsfSimulation*           mSimulationPtr{nullptr};
   std::vector<std::string> mVarLabels;
   MapPlotVariableMap       mVariableMap; //! Map Plot Variable Master List (std::map<varName, extension>)
};

#endif
