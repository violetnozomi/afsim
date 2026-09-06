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
#include "FieldEnumDefines.hpp"
#include "Message12_6.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J12_6
{
IMPLEMENT_ENUM_MAP(StatusField)
{
   ADD_ENUM8(cNO_STATEMENT,
             cENGAGING,
             cINVESTIGATING,
             cWEAPON_RELEASE,
             cNEW_SENSOR_TARGET_REPORT,
             cCANCEL_SENSOR_TARGET,
             cTRACK_TARGET_DESTROYED,
             cDISENGAGING);
   ADD_ENUM4(cHEADS_UP, cLOCK_ON, cINVESTIGATE, cENGAGE);
   ADD_ENUM(cDISENGAGE);
}
} // namespace J12_6
} // namespace Messages
} // namespace WsfL16
