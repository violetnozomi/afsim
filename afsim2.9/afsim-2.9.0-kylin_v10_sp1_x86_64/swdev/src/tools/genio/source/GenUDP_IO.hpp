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

#ifndef GENUDP_IO_HPP
#define GENUDP_IO_HPP

#include "genio_export.h"

#include <string>
#include <vector>

#include "GenIP.hpp"
#include "GenNetIO.hpp"
#include "GenUDP_Connection.hpp"

namespace GenSockets
{
class GenInternetSocketAddress;
class GenSocket;
} // namespace GenSockets

/** Provides User Datagram Protocol communication with a GenIO interface

   Supports broadcast and multicast.
*/

class GENIO_EXPORT GenUDP_IO : public GenUDP_Connection, public GenNetIO
{
public:
   GenUDP_IO();

   ~GenUDP_IO() override;

   int Receive(int aWaitTimeInMicroSec) override;
   int Send() override;

   bool GetSenderId(unsigned int& aSrcInetAddr, unsigned short int& aSrcPort) const override
   {
      return GenUDP_Connection::GetSenderId(aSrcInetAddr, aSrcPort);
   }

   int GetLocalPort() const override { return GenUDP_Connection::GetLocalPort(); }
};
#endif
