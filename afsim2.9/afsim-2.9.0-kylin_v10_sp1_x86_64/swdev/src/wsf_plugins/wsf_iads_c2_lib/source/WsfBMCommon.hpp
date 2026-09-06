// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#ifndef WSFBMCOMMON_HPP
#define WSFBMCOMMON_HPP

#include "wsf_iads_c2_lib_export.h"

// Now using dynamic libraries so imported and exported classes need specific markup.

// Exporting:
//      - class: class WSF_IADS_C2_LIB_EXPORT <class-name> (: <parent-class> ...)
//      - func decl: WSF_IADS_C2_LIB_EXPORT (<flag> ...) <ret-type> <function-name> (<param> ... ) (<flag>)
//      - func def: extern "C" WSF_IADS_C2_LIB_EXPORT (<flag> ...) <ret-type> <function-name> (<param> ... ) (<flag>)
//      - var decl: WSF_IADS_C2_LIB_EXPORT <type> <name>

// Importing
//      - class: class WSF_IADS_C2_LIB_EXPORT <class-name>

typedef unsigned int tPlatformRef;
typedef int tWeaponSysID;
typedef int tSensorSysID;

#define WSF_INVALID_PLAT_ID -1
#define WSF_INVALID_WSYS_ID -1

#define WSF_IADS_C2_XCEPT_SPEED_AUX_DATA_NAME "WSF_IADS_C2_WSYS_INTERCEPT_SPEED"
#define WSF_IADS_C2_MIN_RANGE_AUX_DATA_NAME "minRng"
#define WSF_IADS_C2_MAX_RANGE_AUX_DATA_NAME "maxRng"
#define WSF_IADS_C2_AVG_SPEED_AUX_DATA_NAME "avgSpd"
#define WSF_IADS_C2_EST_PK_AUX_DATA_NAME "estPK"

#endif
