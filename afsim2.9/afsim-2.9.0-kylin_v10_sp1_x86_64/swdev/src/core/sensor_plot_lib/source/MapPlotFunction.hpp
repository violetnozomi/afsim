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

#ifndef MAPPLOTFUNCTION_HPP
#define MAPPLOTFUNCTION_HPP

#include "sensor_plot_lib_export.h"

#include <memory>
#include <string>

#include "Function.hpp"
#include "MapPlotVariable.hpp"
#include "MapPlotVariables.hpp"

class WsfScenario;

//! An abstract base class for sensor_plot functions using MapPlotVariables.
class SENSOR_PLOT_LIB_EXPORT MapPlotFunction : public Function
{
public:
   MapPlotFunction() = default;
   MapPlotFunction(const WsfScenario& aScenario, const std::string& aScriptClassName = "");
   virtual ~MapPlotFunction() = 0;

   bool ProcessInput(UtInput& aInput) override;

   //! Add runtime defined extension of variable types to the list of available variable computation types for plotting routines.
   //!
   //! @param aVarName     The name of the new variable type to be used by MapPlotFunction derived plotting classes.
   //! @param aVariablePtr When a plotting value of type 'aVarName' is encountered this extension will be invoked to process
   //!                     the data.
   void RegisterMapPlotVariable(const std::string& aVarName, std::shared_ptr<MapPlotVariable> aVariablePtr);

protected:
   MapPlotVariables mPlotVariables{};
};

#endif
