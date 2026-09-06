// *****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// *****************************************************************************

#ifndef DISVARIABLEPARAMETERRECORDENUMS_HPP
#define DISVARIABLEPARAMETERRECORDENUMS_HPP

#include <string>

#include "DisTypes.hpp"

class DisVariableParameterRecord;

//! Defines the types of variable parameter records
enum class DisVariableParameterRecordTypes
{
   cARTICULATED        = 0,
   cATTACHED           = 1,
   cSEPARATION         = 2,
   cENTITY_TYPE        = 3,
   cENTITY_ASSOCIATION = 4
};
#endif
