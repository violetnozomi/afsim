// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfHDD_DataContainer.hpp"

HDD::DataContainer::DataContainer(const HDD::DataContainer& aSrc)
   : mHddData(aSrc.mHddData)
{
}

void HDD::DataContainer::SetHDDData(const std::map<QString, HDD_Data>& aHddData)
{
   mHddData = aHddData;
}
