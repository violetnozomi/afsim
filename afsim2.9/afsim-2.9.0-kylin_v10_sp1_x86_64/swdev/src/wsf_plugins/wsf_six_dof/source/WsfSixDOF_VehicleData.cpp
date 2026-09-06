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

#include "WsfSixDOF_VehicleData.hpp"

bool wsf::six_dof::Control::IsBankToTurn(Method aMethod)
{
   return (aMethod == Method::BankToTurnNoYaw || aMethod == Method::BankToTurnWithYaw);
}

bool wsf::six_dof::Control::IsYawToTurn(Method aMethod)
{
   return (aMethod == Method::YawToTurnNoRoll || aMethod == Method::YawToTurnRollRate ||
           aMethod == Method::YawToTurnZeroBank);
}
