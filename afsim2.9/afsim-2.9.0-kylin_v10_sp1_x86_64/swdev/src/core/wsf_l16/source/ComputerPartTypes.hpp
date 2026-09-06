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
#ifndef L16_COMPUTERPARTTYPES_HPP
#define L16_COMPUTERPARTTYPES_HPP

#include "ComputerPart.hpp"
#include "WsfObjectTypeList.hpp"

namespace WsfL16
{
class ComputerPartTypes : public WsfObjectTypeList<ComputerPart>
{
public:
   ComputerPartTypes(WsfScenario& aScenario);
};
} // namespace WsfL16

#endif
