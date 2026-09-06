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

#include "DisModulationType.hpp"

#include <iostream>

#include "GenI.hpp"
#include "GenO.hpp"

DisModulationType::DisModulationType()
   : mSpreadSpectrum(0)
   , mMajor(0)
   , mDetail(0)
   , mSystem(0)
{
}

DisModulationType::~DisModulationType() {}

void DisModulationType::Get(GenI& aGenI)
{
   aGenI >> mSpreadSpectrum;
   aGenI >> mMajor;
   aGenI >> mDetail;
   aGenI >> mSystem;
}

void DisModulationType::Put(GenO& aGenO) const
{
   aGenO << mSpreadSpectrum;
   aGenO << mMajor;
   aGenO << mDetail;
   aGenO << mSystem;
}
void DisModulationType::Stream(std::ostream& aStream) const
{
   aStream << ToString() << std::endl;
}
std::string DisModulationType::ToString() const
{
   std::string returnString;
   returnString += "Spread Spectrum: " + std::to_string(mSpreadSpectrum) + "\n";
   returnString += "major:           " + std::to_string(mMajor) + "\n";
   returnString += "mDetail:         " + std::to_string(mDetail) + "\n";
   returnString += "mSystem:         " + std::to_string(mSystem) + "\n";
   return returnString;
}
bool DisModulationType::IsValid() const
{
   return true;
}
