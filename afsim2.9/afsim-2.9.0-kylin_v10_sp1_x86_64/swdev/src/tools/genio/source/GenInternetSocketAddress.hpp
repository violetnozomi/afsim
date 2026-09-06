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

#ifndef GENINTERNETSOCKETADDRESS_HPP
#define GENINTERNETSOCKETADDRESS_HPP

#include <string>
struct sockaddr_in;

#include "genio_export.h"

#include "GenInternetAddress.hpp"

namespace GenSockets
{
/** A SocketAddress and port number combined -- everything needed to
    specify a message destination.
*/
class GENIO_EXPORT GenInternetSocketAddress
{
public:
   friend class GenSocket;

   GenInternetSocketAddress(const std::string& aAddrStr);
   GenInternetSocketAddress(const GenInternetAddress& aAddr, int aPort);
   GenInternetSocketAddress(sockaddr_in& aAddr);
   GenInternetSocketAddress(const GenInternetSocketAddress& aSrc);
   GenInternetSocketAddress();
   ~GenInternetSocketAddress();
   GenInternetSocketAddress& operator=(const GenInternetSocketAddress& aSrc);
   bool                      operator==(const GenInternetSocketAddress& aRhs) const;
   bool                      operator<(const GenInternetSocketAddress& aRhs) const;
   int                       GetPort() const;
   void                      SetPort(int aPort);
   GenInternetAddress        GetAddress() const;
   bool                      IsValid() const;
                             operator std::string() const;

protected:
   sockaddr_in* GetSockAddr() { return mSockAddr; }

   sockaddr_in* mSockAddr;
};
} // namespace GenSockets

#endif
