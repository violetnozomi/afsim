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

// GenIO - A base class for generic input and output.

#ifndef GENIO_HPP
#define GENIO_HPP

#include "genio_export.h"

#include "GenI.hpp"
#include "GenO.hpp"

/// \file  GenIO.hpp
/// \brief GenIO
/// \class GenIO GenIO.hpp "GenIO.hpp"
/// \brief This class brings together the GenI and GenO interfaces.
///
/// It is meant to be derived by a transport class that sends and receives data.  The transport
/// class holds the code that actually moves the data.


class GENIO_EXPORT GenIO : public GenI, public GenO
{
public:
   GenIO(const char* aRecvBufName, const char* aSendBufName);
   ~GenIO() override;

private:
};

#endif
