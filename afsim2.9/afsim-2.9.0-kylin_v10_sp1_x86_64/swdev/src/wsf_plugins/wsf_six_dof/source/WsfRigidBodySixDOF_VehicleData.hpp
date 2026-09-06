// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFRIGIDBODYSIXDOFVEHICLEDATA_HPP
#define WSFRIGIDBODYSIXDOFVEHICLEDATA_HPP

#include <WsfSixDOF_VehicleData.hpp>

namespace wsf
{
namespace six_dof
{
// This data structure is used to define a control surface position
struct ControlSurfaceValue
{
   std::string       controlSurfaceName;
   float             value    = 0.0;
   DataType::Angular typeData = DataType::Angular::AngleDeg;
};

// This data structure is used to define a control input and its value
struct FlightControlInputValue
{
   explicit FlightControlInputValue(const std::string& aName)
      : controlInputName(aName)
   {
   }

   FlightControlInputValue(const FlightControlInputValue& aSrc) = default;

   std::string      controlInputName;
   float            normalizedValue = 1.0;
   DataType::Simple typeData        = DataType::Simple::Float;
   bool             boolData        = false;
};
} // namespace six_dof
} // namespace wsf

#endif
