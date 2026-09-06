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

#include "DisVariableParameterRecord.hpp"

#include "GenI.hpp"
#include "GenO.hpp"

DisVariableParameterRecord::DisVariableParameterRecord(const DisVariableParameterRecord& aSrc)
   : mTypeDesignator(aSrc.mTypeDesignator)
{
}

DisVariableParameterRecord::DisVariableParameterRecord(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisVariableParameterRecord* DisVariableParameterRecord::Clone() const
{
   return nullptr;
}

bool DisVariableParameterRecord::IsValid() const
{
   return mTypeDesignator >= 0 && mTypeDesignator <= 4;
}

std::string DisVariableParameterRecord::ToString() const
{
   std::string returnString = "TypeDesignator    = \n" + std::to_string(mTypeDesignator) + "\n";
   return returnString;
}

void DisVariableParameterRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

void DisVariableParameterRecord::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisUint16 DisVariableParameterRecord::GetLength()
{
   return cMIN_LENGTH_OCTETS;
}

void DisVariableParameterRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mTypeDesignator;
}

void DisVariableParameterRecord::Put(GenO& aGenO) const
{
   aGenO << mTypeDesignator;
}

bool DisVariableParameterRecord::operator==(const DisVariableParameterRecord& aRhs)
{
   return mTypeDesignator == aRhs.mTypeDesignator;
}
