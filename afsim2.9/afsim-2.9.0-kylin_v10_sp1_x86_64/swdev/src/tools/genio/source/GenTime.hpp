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

/// \file  GenTime.hpp
/// \brief GenTime
/// \class GenTime GenTime.hpp
/// \brief Provides a base class to compute current wall clock time.


#ifndef GEN_TIME_HPP
#define GEN_TIME_HPP

#include "genio_export.h"

#include <string>

class GENIO_EXPORT GenTime
{
public:
   GenTime();
   ~GenTime();

   static std::string GetWallClockTime();

private:
};


#endif
