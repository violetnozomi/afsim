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

//
// This class inherits from GenIOMsg
//


#include "GenIOMsgStatus.hpp"

#include "GenI.hpp"
#include "GenO.hpp"


GenIOMsgStatus::GenIOMsgStatus()
   : GenIOMsg(TypeStatus)
{
}

GenIOMsgStatus::GenIOMsgStatus(double timeA)
   : GenIOMsg(timeA, TypeStatus)
{
}

GenIOMsgStatus::GenIOMsgStatus(const GenIOMsg& genIOMsgA, GenI& genIA)
   : GenIOMsg(genIOMsgA)
{
   GetGenIOMsgClassData(genIA);
   CheckMismatchedSize(genIA);
}

GenIOMsgStatus::GenIOMsgStatus(GenI& genIA)
   : GenIOMsg(genIA)
{
   GetGenIOMsgClassData(genIA);
   CheckMismatchedSize(genIA);
}

GenIOMsgStatus::~GenIOMsgStatus() {}

// Virtual
int GenIOMsgStatus::GetGenIOMsgClassLength() const
{
   // Nothing here for now
   return 0;
}

// Virtual
void GenIOMsgStatus::GetGenIOMsgClassData(GenI& /* genIA */) {}

// Virtual
void GenIOMsgStatus::PutGenIOMsgClassData(GenO& /* genOA */) const {}
