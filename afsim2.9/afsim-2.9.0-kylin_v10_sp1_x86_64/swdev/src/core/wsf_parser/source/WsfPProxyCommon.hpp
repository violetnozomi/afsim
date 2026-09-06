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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFPPROXYCOMMON_HPP
#define WSFPPROXYCOMMON_HPP

#include "wsf_parser_export.h"

class WsfPProxyType;
class WsfPProxyBasicType;

namespace WsfProxy
{
enum Flags
{
   cPOD         = 1, // Plain old Data
   cPOLYMORPHIC = 2, // Is this a struct with derived types
   cVALUE_FLAG  = 4  // This is a value type
};

enum ValueFlags
{
   cVALUE_INHERITED = 1,
   cVALUE_UNSET     = 2
};

enum ValueKind
{
   cSTRUCT            = 0,
   cLIST              = 1,
   cOBJECT_MAP        = 2,
   cPOINTER           = 3,
   cENUMERATION_VALUE = 4,
   cBOOL_VALUE        = 5,
   cINT_VALUE         = 6,
   cSTRING_VALUE      = 7,
   cDOUBLE_VALUE      = 8,
   cLENGTH_VALUE,
   cTIME_VALUE,
   cSPEED_VALUE,
   cDATASIZE_VALUE,
   cPOWER_VALUE,
   cPOWERDB_VALUE,
   cSOLIDANGLE_VALUE,
   cMASS_VALUE,
   cFORCE_VALUE,
   cTORQUE_VALUE,
   cAREA_VALUE,
   cAREADB_VALUE,
   cVOLUME_VALUE,
   cTIME2_VALUE,
   cACCELERATION_VALUE,
   cFREQUENCY_VALUE,
   cANGLE_VALUE,
   cANGULARRATE_VALUE,
   cANGULARACCELERATION_VALUE,
   cDATARATE_VALUE,
   cMASSDENSITY_VALUE,
   cMASSTRANSFER_VALUE,
   cENERGY_VALUE,
   cFLUENCE_VALUE,
   cIRRADIANCE_VALUE,
   cRATIO_VALUE,
   cNOISEPRESSURE_VALUE,
   cPRESSURE_VALUE,
   cTEMPERATURE_VALUE,
   cSPECIFICRANGE_VALUE,
   cANGULARINERTIA_VALUE,
   cLATITUDE_VALUE,
   cLONGITUDE_VALUE,
   cPOSITION_VALUE,
   cLENGTH2_VALUE,
   cLENGTH3_VALUE,
   cQUOTABLE_STRING_VALUE,
   cTYPE_COUNT
};

enum JSON_OutputOptions
{
   cJO_HIDE_BASIC_VALUES     = 1,
   cJO_HIDE_UNSET_VALUES     = 2,
   cJO_HIDE_INHERITED_VALUES = 4,
   cJO_SHOW_ADDRESSES        = 8,
   cJO_SHOW_INHERITANCE      = 16,

   cJO_LEAN_OUTPUT = cJO_HIDE_BASIC_VALUES | cJO_HIDE_UNSET_VALUES | cJO_HIDE_INHERITED_VALUES
};

enum CopyFlags
{
   cCOPY_BASE = 1,

   cCOPY_ALL       = cCOPY_BASE, // "or" more things on as necessary
   cCOPY_INHERITED = 0           // "or" more things on as necessary
};
} // namespace WsfProxy

namespace wsf
{
namespace proxy
{

//! Converts a WsfProxy::ValueKind to a std::string
//! @param aKind is the WsfProxy::ValueKind to convert
//! @return the WsfProxy::ValueKind as a std::string
static inline std::string ValueKindToString(WsfProxy::ValueKind aKind) noexcept
{
   switch (aKind)
   {
   case WsfProxy::ValueKind::cSTRUCT:
      return "Struct";
   case WsfProxy::ValueKind::cLIST:
      return "List";
   case WsfProxy::ValueKind::cOBJECT_MAP:
      return "ObjectMap";
   case WsfProxy::ValueKind::cPOINTER:
      return "Pointer";
   case WsfProxy::ValueKind::cENUMERATION_VALUE:
      return "Enumeration";
   case WsfProxy::ValueKind::cBOOL_VALUE:
      return "Bool";
   case WsfProxy::ValueKind::cINT_VALUE:
      return "Int";
   case WsfProxy::ValueKind::cSTRING_VALUE:
      return "String";
   case WsfProxy::ValueKind::cDOUBLE_VALUE:
      return "Double";
   case WsfProxy::ValueKind::cLENGTH_VALUE:
      return "Length";
   case WsfProxy::ValueKind::cTIME_VALUE:
      return "Time";
   case WsfProxy::ValueKind::cSPEED_VALUE:
      return "Speed";
   case WsfProxy::ValueKind::cDATASIZE_VALUE:
      return "DataSize";
   case WsfProxy::ValueKind::cPOWER_VALUE:
      return "Power";
   case WsfProxy::ValueKind::cPOWERDB_VALUE:
      return "PowerDB";
   case WsfProxy::ValueKind::cSOLIDANGLE_VALUE:
      return "SolidAngle";
   case WsfProxy::ValueKind::cMASS_VALUE:
      return "Mass";
   case WsfProxy::ValueKind::cFORCE_VALUE:
      return "Force";
   case WsfProxy::ValueKind::cTORQUE_VALUE:
      return "Torque";
   case WsfProxy::ValueKind::cAREA_VALUE:
      return "Area";
   case WsfProxy::ValueKind::cAREADB_VALUE:
      return "AreaDB";
   case WsfProxy::ValueKind::cVOLUME_VALUE:
      return "Volume";
   case WsfProxy::ValueKind::cTIME2_VALUE:
      return "Time2";
   case WsfProxy::ValueKind::cACCELERATION_VALUE:
      return "Acceleration";
   case WsfProxy::ValueKind::cFREQUENCY_VALUE:
      return "Frequency";
   case WsfProxy::ValueKind::cANGLE_VALUE:
      return "Angle";
   case WsfProxy::ValueKind::cANGULARRATE_VALUE:
      return "AngularRate";
   case WsfProxy::ValueKind::cANGULARACCELERATION_VALUE:
      return "AngularAcceleration";
   case WsfProxy::ValueKind::cDATARATE_VALUE:
      return "DataRate";
   case WsfProxy::ValueKind::cMASSDENSITY_VALUE:
      return "MassDensity";
   case WsfProxy::ValueKind::cMASSTRANSFER_VALUE:
      return "MassTransfer";
   case WsfProxy::ValueKind::cENERGY_VALUE:
      return "Energy";
   case WsfProxy::ValueKind::cFLUENCE_VALUE:
      return "Fluence";
   case WsfProxy::ValueKind::cIRRADIANCE_VALUE:
      return "Irradiance";
   case WsfProxy::ValueKind::cRATIO_VALUE:
      return "Ratio";
   case WsfProxy::ValueKind::cNOISEPRESSURE_VALUE:
      return "NoisePressure";
   case WsfProxy::ValueKind::cPRESSURE_VALUE:
      return "Pressure";
   case WsfProxy::ValueKind::cTEMPERATURE_VALUE:
      return "Temperature";
   case WsfProxy::ValueKind::cSPECIFICRANGE_VALUE:
      return "SpecificRange";
   case WsfProxy::ValueKind::cANGULARINERTIA_VALUE:
      return "AngularInertia";
   case WsfProxy::ValueKind::cLATITUDE_VALUE:
      return "Latitude";
   case WsfProxy::ValueKind::cLONGITUDE_VALUE:
      return "Longitude";
   case WsfProxy::ValueKind::cPOSITION_VALUE:
      return "Position";
   case WsfProxy::ValueKind::cLENGTH2_VALUE:
      return "Length2";
   case WsfProxy::ValueKind::cLENGTH3_VALUE:
      return "Length3";
   case WsfProxy::ValueKind::cQUOTABLE_STRING_VALUE:
      return "QuotableString";
   default:
      return "";
   }
}

} // namespace proxy
} // namespace wsf

#endif
