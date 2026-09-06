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

#ifndef GENSOCKETCONNECTION_HPP
#define GENSOCKETCONNECTION_HPP

#include "genio_export.h"

#include <memory>
#include <string>

#include "GenSocket.hpp"

//! Base class for a socket connection.  Not much more than a GenSocket.
class GENIO_EXPORT GenSocketConnection
{
public:
   GenSocketConnection(GenSockets::GenSocket* aSocket);

   virtual ~GenSocketConnection() = default;

   virtual int GetLocalPort() const;

   bool IsConnected() const;

   virtual bool IsNonBlocking() const;

   virtual void SetIsNonBlocking(bool aTF);

   std::unique_ptr<GenSockets::GenSocket> mSocket;
};

#endif
