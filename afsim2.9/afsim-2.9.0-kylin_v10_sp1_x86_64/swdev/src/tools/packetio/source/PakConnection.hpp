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

#ifndef PAKCONNECTION_HPP
#define PAKCONNECTION_HPP

#include "packetio_export.h"

// This is just an interface for classes that represent
// a connection to a computer.  Used to access the sender of a packet.
class PACKETIO_EXPORT PakConnection
{
public:
   virtual ~PakConnection();
};

#endif
