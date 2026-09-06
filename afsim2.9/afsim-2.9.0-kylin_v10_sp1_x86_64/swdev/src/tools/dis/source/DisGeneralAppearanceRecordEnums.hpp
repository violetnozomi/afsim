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

#ifndef DISGENERALAPPEARANCERECORDENUMS_HPP
#define DISGENERALAPPEARANCERECORDENUMS_HPP

#include "dis_export.h"

#include <string>

#include "DisTypes.hpp"

/**!
 * For the DisUint16 bit values that each of these
 * enums apply to, see DisGeneralAppearanceRecord.cpp.
 */
namespace DisEnum
{
namespace GeneralAppearanceRecord
{
namespace ObjectDamage
{

enum Enum
{
   cNO_DAMAGE = 0,
   cDAMAGED   = 1,
   cDESTROYED = 2
};

DIS_EXPORT const bool IsValid(int in);
DIS_EXPORT const std::string& ToString(Enum in);
DIS_EXPORT const Enum         ToEnum(const std::string& in);
} // namespace ObjectDamage

namespace Predistributed
{

enum Enum
{
   cOBJECT_CREATED_DURING_THE_EXERCISE            = 0,
   cOBJECT_PREDISTRIBUTED_PRIOR_TO_EXERCISE_START = 1
};

DIS_EXPORT const bool IsValid(int in);
DIS_EXPORT const std::string& ToString(Enum in);
DIS_EXPORT const Enum         ToEnum(const std::string& in);
} // namespace Predistributed

namespace ObjectState
{

enum Enum
{
   cACTIVE      = 0,
   cDEACTIVATED = 1
};

DIS_EXPORT const bool IsValid(int in);
DIS_EXPORT const std::string& ToString(Enum in);
DIS_EXPORT const Enum         ToEnum(const std::string& in);
} // namespace ObjectState

namespace IEDPresent
{

enum Enum
{
   cNONE              = 0,
   cVISIBLE           = 1,
   cPARTIALLY_HIDDEN  = 2,
   cCOMPLETELY_HIDDEN = 3
};

DIS_EXPORT const bool IsValid(int in);
DIS_EXPORT const std::string& ToString(Enum in);
DIS_EXPORT const Enum         ToEnum(const std::string& in);
} // namespace IEDPresent
} // end namespace GeneralAppearanceRecord
} // namespace DisEnum

#endif
