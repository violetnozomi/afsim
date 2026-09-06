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

#include "DisGeneralAppearanceRecordEnums.hpp"

#include <sstream>
#include <utility>
#include <vector>

#include "DisEnumConversion.hpp"
namespace DisEnum
{
namespace GeneralAppearanceRecord
{
namespace ObjectDamage
{
const std::vector<std::pair<Enum, std::string>> data = {{cNO_DAMAGE, "No Damage"},
                                                        {cDAMAGED, "Damaged"},
                                                        {cDESTROYED, "Destroyed"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int in)
{
   return in >= cNO_DAMAGE && in <= cDESTROYED;
}

const std::string& ToString(Enum in)
{
   return conv.ToString(in);
}

const Enum ToEnum(const std::string& in)
{
   return conv.ToEnum(in);
}
} // namespace ObjectDamage

namespace Predistributed
{
const std::vector<std::pair<Enum, std::string>> data = {{cOBJECT_CREATED_DURING_THE_EXERCISE,
                                                         "Object Created During the Exercise"},
                                                        {cOBJECT_PREDISTRIBUTED_PRIOR_TO_EXERCISE_START,
                                                         "Object Predistributed Prior to Exercise Start"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int in)
{
   return in >= cOBJECT_CREATED_DURING_THE_EXERCISE && in <= cOBJECT_PREDISTRIBUTED_PRIOR_TO_EXERCISE_START;
}

const std::string& ToString(Enum in)
{
   return conv.ToString(in);
}

const Enum ToEnum(const std::string& in)
{
   return conv.ToEnum(in);
}
} // namespace Predistributed

namespace ObjectState
{
const std::vector<std::pair<Enum, std::string>> data = {{cACTIVE, "Active"}, {cDEACTIVATED, "Deactivated"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int in)
{
   return in >= cACTIVE && in <= cDEACTIVATED;
}

const std::string& ToString(Enum in)
{
   return conv.ToString(in);
}

const Enum ToEnum(const std::string& in)
{
   return conv.ToEnum(in);
}
} // namespace ObjectState

namespace IEDPresent
{
const std::vector<std::pair<Enum, std::string>> data = {{cNONE, "None"},
                                                        {cVISIBLE, "Visible"},
                                                        {cPARTIALLY_HIDDEN, "Partially Hidden"},
                                                        {cCOMPLETELY_HIDDEN, "Completely Hidden"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int in)
{
   return in >= cNONE && in <= cCOMPLETELY_HIDDEN;
}
const std::string& ToString(Enum in)
{
   return conv.ToString(in);
}

const Enum ToEnum(const std::string& in)
{
   return conv.ToEnum(in);
}
} // namespace IEDPresent
} // namespace GeneralAppearanceRecord
} // namespace DisEnum
