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
// This class inherits from GenMsg


#include "GenIOMsg.hpp"

#include "GenI.hpp"
#include "GenO.hpp"


const int   GenIOMsg::GenIOMsgBaseLengthS = 0;
const char* GenIOMsg::classNameS[]        = {"Unknown", "GenIOMsgConnect", "GenIOMsgStatus", "UnknownMax"};


GenIOMsg::GenIOMsg(GenIOMsgType typeA)
   : GenMsg(TypeGenIO, typeA, 2)
{
}

GenIOMsg::GenIOMsg(GenI& aGenI)
   : GenMsg(aGenI)
{
   GenIOMsg::GetGenIOMsgData(aGenI);
}

GenIOMsg::GenIOMsg(const GenMsg& msgA, GenI& aGenI)
   : GenMsg(msgA)
{
   GenIOMsg::GetGenIOMsgData(aGenI);
}

GenIOMsg::GenIOMsg(double timeA, GenIOMsgType typeA)
   : GenMsg(timeA, TypeGenIO, typeA, 2)
{
}

GenIOMsg::~GenIOMsg() {}

// Static
const char* GenIOMsg::GetGenIOMsgClassName(GenIOMsg::GenIOMsgType typeA)
{
   if (typeA < TypeUnknown)
   {
      typeA = TypeUnknown;
   }
   else if (typeA >= TypeUnknownMax)
   {
      typeA = TypeUnknown;
   }

   return classNameS[typeA];
}

// Virtual
int GenIOMsg::DoIt()
{
   return 0;
}

//
// Private
//

void GenIOMsg::GetGenIOMsgData(GenI& /* genIA */)
{
   // This function is a place holder for now
}

// Virtual
int GenIOMsg::GetClassLength() const
{
   return GenIOMsgBaseLengthS + GetGenIOMsgClassLength();
}

// Virtual
void GenIOMsg::GetClassData(GenI& genIA)
{
   GetGenIOMsgData(genIA);
   GetGenIOMsgClassData(genIA);
}

// Virtual
void GenIOMsg::PutClassData(GenO& genOA) const
{
   // If there were any variables in this class, they would be written
   // out here
   PutGenIOMsgClassData(genOA);
}

// Virtual
// Derived classes will typically override this function
int GenIOMsg::GetGenIOMsgClassLength() const
{
   return 0;
}

// Virtual
// Derived classes will typically override this function
void GenIOMsg::GetGenIOMsgClassData(GenI& /* genIA */) {}

// Virtual
// Derived classes will typically override this function
void GenIOMsg::PutGenIOMsgClassData(GenO& /* genOA */) const {}
