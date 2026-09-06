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


#ifndef GENIO_MSG_STATUS_HPP
#define GENIO_MSG_STATUS_HPP

#include "genio_export.h"

#include <string>

#include "GenIOMsg.hpp"

class GenI;
class GenO;

class GENIO_EXPORT GenIOMsgStatus : public GenIOMsg
{
public:
   GenIOMsgStatus();
   GenIOMsgStatus(double timeA);
   GenIOMsgStatus(const GenIOMsg& genIOMsgA, GenI& genIA);
   GenIOMsgStatus(GenI& genIA);

   ~GenIOMsgStatus() override;

   // Accessors

   // Mutators

protected:
   // Input/output
   int  GetGenIOMsgClassLength() const override;
   void GetGenIOMsgClassData(GenI& genIA) override;
   void PutGenIOMsgClassData(GenO& genOA) const override;

private:
};

// Accessors

// Mutators

#endif
