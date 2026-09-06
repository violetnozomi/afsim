//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#ifndef IDISPDUPROCESS_HPP
#define IDISPDUPROCESS_HPP

#include "dis_export.h"

//! Wrapper class for a PDU processing logic block
class DIS_EXPORT DisPduProcessI
{
public:
   virtual ~DisPduProcessI() = default;

   //! Processes the PDU which contains this logic block
   virtual int Process() = 0;
};
#endif
