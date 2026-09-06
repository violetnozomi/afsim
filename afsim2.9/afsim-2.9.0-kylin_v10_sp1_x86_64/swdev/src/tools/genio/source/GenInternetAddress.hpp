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

#ifndef GENINTERNETADDRESS_HPP
#define GENINTERNETADDRESS_HPP

#include <string>
struct in_addr;

#include "genio_export.h"

namespace GenSockets
{
class GenIP;
class GenHostName;

/** Represents an address.
 */
class GENIO_EXPORT GenInternetAddress
{
public:
   GenInternetAddress(const GenIP& aIP);
   GenInternetAddress(const GenHostName& aHostName);
   GenInternetAddress(const std::string& aAddrStr);
   GenInternetAddress(const GenInternetAddress& aSrc);
   GenInternetAddress& operator=(const GenInternetAddress& aAddr);
   bool                operator<(const GenInternetAddress& aRhs) const;
   bool                operator==(const GenInternetAddress& aAddr) const;
   //! Sets address = INADDR_ANY
   GenInternetAddress();
   ~GenInternetAddress();
                  operator GenHostName() const;
                  operator GenIP() const;
   const in_addr* GetInAddr() const { return mInAddr; }
   bool           IsValid();

protected:
   in_addr* mInAddr;
};
} // namespace GenSockets
#endif
