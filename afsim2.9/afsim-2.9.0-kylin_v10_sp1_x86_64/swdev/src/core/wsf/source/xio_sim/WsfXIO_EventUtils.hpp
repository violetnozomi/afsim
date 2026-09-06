// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "wsf_export.h"

#include <ostream>

class GenUniqueId;

namespace wsf
{
namespace event
{

namespace utils
{
void WSF_EXPORT Write(std::ostream& aStream, const GenUniqueId& aApplicationId);
}

namespace utilsCSV
{
void WSF_EXPORT Write(std::ostream& aStream, const GenUniqueId& aApplicationId);
}

} // namespace event
} // namespace wsf
