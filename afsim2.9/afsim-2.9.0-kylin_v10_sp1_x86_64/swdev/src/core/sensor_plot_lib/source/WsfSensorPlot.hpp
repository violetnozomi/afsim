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

#ifndef WSFSENSORPLOT_HPP
#define WSFSENSORPLOT_HPP

#include "sensor_plot_lib_export.h"

#include <map>
#include <vector>

#include "Function.hpp"
#include "MapPlotVariables.hpp"
class UtInput;
#include "WsfScenarioExtension.hpp"
class WsfSensorPlotApplicationExtension;

class SENSOR_PLOT_LIB_EXPORT WsfSensorPlotExtension : public WsfScenarioExtension
{
public:
   static WsfSensorPlotExtension* Find(WsfScenario& aScenario);
   explicit WsfSensorPlotExtension(WsfSensorPlotApplicationExtension* aAppExtensionPtr);
   ~WsfSensorPlotExtension() override                    = default;
   WsfSensorPlotExtension(const WsfSensorPlotExtension&) = delete;
   WsfSensorPlotExtension& operator=(const WsfSensorPlotExtension&) = delete;

   void        AddedToScenario() override;
   bool        ProcessInput(UtInput& aInput) override;
   bool        Complete2() override;
   bool        ExecutePlots();
   static void EnableSensorPlotMode();
   static bool IsSensorPlotMode();

   template<class T>
   void RegisterFunction(const std::string& aCommandName)
   {
      auto fn                        = [](WsfScenario& aScenario) { return ut::make_unique<T>(aScenario); };
      mFunctionFactory[aCommandName] = std::move(fn);
   }

   //! Plugins or projects that register a new MapPlotVariable type must call this at application startup to register
   //! their new types.
   //!
   //! Once registered the types will be injected into ProcessInput() chain of MapPlotFunction derivatives.
   //!
   //! @param aVarName     The script name of the new variable type to be used by MapPlotFunction derived plotting
   //! classes.
   //! @param aVariablePtr When a plotting value of type `varName` is encountered this variable function will be invoked
   //! to process
   //!                     the data.
   void RegisterMapPlotVariable(const std::string& aVarName, std::shared_ptr<MapPlotVariable> aVariablePtr);

private:
   bool RunFunction(Function* aFunctionPtr);

   using FunctionPtr        = std::function<std::unique_ptr<Function>(WsfScenario&)>;
   using FunctionFactoryMap = std::map<std::string, FunctionPtr>;
   WsfSensorPlotApplicationExtension*     mAppExtensionPtr;
   std::vector<std::unique_ptr<Function>> mFunctionList;
   FunctionFactoryMap                     mFunctionFactory;

   //! Stores the variable extension registration data as std::map<varName, extension>.
   MapPlotVariables::MapPlotVariableMap mMapPlotVariableMap;
};

#endif
