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
#include "Message3_4.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J3_4
{
IMPLEMENT_ENUM_MAP(DataReportTypeField)
{
   ADD_ENUM(cSUBSURFACE_TRACK);
   ADD_ENUM(cSURFACED_SUBMARINE);
   ADD_ENUM(cSNORKELING_SUBMARINE);
   ADD_ENUM(cDATUM);
}
} // namespace J3_4
} // namespace Messages
} // namespace WsfL16
