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

#ifndef GEN_NETINFO_HPP
#define GEN_NETINFO_HPP

#include "genio_export.h"

#include <string>
#include <vector>

//! GenNetInfo is a utility class that performs discovery of available
//! network interfaces, including address, netmask, enabled status,
//! and capabilities.
class GENIO_EXPORT GenNetInfo
{
   struct InfoImpl;

public:
   class GENIO_EXPORT Interface
   {
   public:
      Interface();
      ~Interface();

      bool HasBroadcast() const;
      bool HasMulticast() const;
      bool IsUp() const;
      bool IsLoopback() const;

      const std::string& GetAddress() const;
      const std::string& GetBroadcastAddr() const;
      const std::string& GetNetmask() const;

   private:
      friend class GenNetInfo;
      InfoImpl* mInfoPtr;
   };

public:
   ~GenNetInfo();

   //! Methods for iterating over available interfaces.
   static unsigned         GetInterfaceCount();
   static const Interface* GetInterfaceEntry(unsigned aEntry);

   //! Methods for retrieving specific broadcast or multicast interfaces.
   static const Interface* GetBroadcastInterface(unsigned aInterfaceNumber = 0);
   static const Interface* GetMulticastInterface(unsigned aInterfaceNumber = 0);

   static void Print();

private:
   GenNetInfo();

   static GenNetInfo& GetInstance();

   std::vector<Interface*> mInterfaces;
};

#endif
