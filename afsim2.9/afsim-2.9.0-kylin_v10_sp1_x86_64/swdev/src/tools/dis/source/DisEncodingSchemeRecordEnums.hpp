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

#ifndef DISENCODINGSCHEMERECORDENUMS_HPP
#define DISENCODINGSCHEMERECORDENUMS_HPP

#include "dis_export.h"

#include <string>

#include "DisTypes.hpp"

//! For the DisUint16 bit values that each of these
//! enums apply to, see DisEncodingSchemeRecord.cpp.

namespace DisEnum
{
namespace EncodingSchemeRecord
{
namespace EncodingClass
{

enum Enum
{
   cENCODED_AUDIO             = 0,
   cRAW_BINARY_DATA           = 1,
   cAPPLICATION_SPECIFIC_DATA = 2,
   cDATABASE_INDEX            = 3
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace EncodingClass

namespace EncodingType
{

enum Enum
{
   cEIGHT_BIT_MU_LAW                                     = 1,
   cCVSD                                                 = 2,
   cADPCM                                                = 3,
   cSIXTEEN_BIT_LINEAR_PCM_TWOS_COMPLEMENT_BIG_ENDIAN    = 4,
   cEIGHT_BIT_LINEAR_PCM_UNSIGNED                        = 5,
   cGSM_FULL_RATE                                        = 8,
   cGSM_HALF_RATE                                        = 9,
   cSPEEX_NARROW_BAND                                    = 10,
   cSIXTEEN_BIT_LINEAR_PCM_TWOS_COMPLEMENT_LITTLE_ENDIAN = 100
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace EncodingType
} // end namespace EncodingSchemeRecord
} // namespace DisEnum

#endif
