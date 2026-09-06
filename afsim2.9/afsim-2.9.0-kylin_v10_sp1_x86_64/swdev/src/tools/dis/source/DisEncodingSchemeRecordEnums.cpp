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

#include "DisEncodingSchemeRecordEnums.hpp"

#include <sstream>
#include <utility>
#include <vector>

#include "DisEnumConversion.hpp"

namespace DisEnum
{

namespace EncodingSchemeRecord
{

namespace EncodingClass
{

const std::vector<std::pair<Enum, std::string>> data = {{cENCODED_AUDIO, "Encoded audio"},
                                                        {cRAW_BINARY_DATA, "Raw Binary Data"},
                                                        {cAPPLICATION_SPECIFIC_DATA, "Application-Specific Data"},
                                                        {cDATABASE_INDEX, "Database index"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int in)
{
   return in >= cENCODED_AUDIO && in <= cDATABASE_INDEX;
}
const std::string& ToString(Enum in)
{
   return conv.ToString(in);
}

const Enum ToEnum(const std::string& in)
{
   return conv.ToEnum(in);
}
} // namespace EncodingClass

namespace EncodingType
{

const std::vector<std::pair<Enum, std::string>> data = {{cEIGHT_BIT_MU_LAW, "8-bit mu-law (ITU-T G.711)"},
                                                        {cCVSD, "CVSD (MIL-STD-188-113)"},
                                                        {cADPCM, "ADPCM (ITU-T G.726)"},
                                                        {cSIXTEEN_BIT_LINEAR_PCM_TWOS_COMPLEMENT_BIG_ENDIAN,
                                                         "16-bit Linear PCM 2’s complement, Big Endian"},
                                                        {cEIGHT_BIT_LINEAR_PCM_UNSIGNED, "8-bit Linear PCM, unsigned"},
                                                        {cGSM_FULL_RATE, "GSM Full-Rate (ETSI 06.10)"},
                                                        {cGSM_HALF_RATE, "GSM Half-Rate (ETSI 06.20)"},
                                                        {cSPEEX_NARROW_BAND, "Speex Narrow Band"},
                                                        {cSIXTEEN_BIT_LINEAR_PCM_TWOS_COMPLEMENT_LITTLE_ENDIAN,
                                                         "16-bit Linear PCM 2’s complement, Little Endian"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int in)
{
   return (in >= cEIGHT_BIT_MU_LAW && in <= cEIGHT_BIT_LINEAR_PCM_UNSIGNED) ||
          (in >= cGSM_FULL_RATE && in <= cSPEEX_NARROW_BAND) ||
          in == cSIXTEEN_BIT_LINEAR_PCM_TWOS_COMPLEMENT_LITTLE_ENDIAN;
}
const std::string& ToString(Enum in)
{
   return conv.ToString(in);
}

const Enum ToEnum(const std::string& in)
{
   return conv.ToEnum(in);
}
} // namespace EncodingType
} // namespace EncodingSchemeRecord
} // namespace DisEnum
