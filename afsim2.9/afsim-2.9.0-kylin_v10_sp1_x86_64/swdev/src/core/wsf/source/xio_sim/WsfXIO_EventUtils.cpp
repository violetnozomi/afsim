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

#include <ostream>

#include "GenUniqueId.hpp"

namespace wsf
{
namespace event
{

namespace utils
{
void Write(std::ostream& aStream, const GenUniqueId& aApplicationId)
{
   aStream << aApplicationId.GetData(0) << '-' << aApplicationId.GetData(1) << '-' << aApplicationId.GetData(2);
}
} // namespace utils

namespace utilsCSV
{
void Write(std::ostream& aStream, const GenUniqueId& aApplicationId)
{
   aStream << aApplicationId.GetData(0) << '-' << aApplicationId.GetData(1) << '-' << aApplicationId.GetData(2);
}
} // namespace utilsCSV

} // namespace event
} // namespace wsf
