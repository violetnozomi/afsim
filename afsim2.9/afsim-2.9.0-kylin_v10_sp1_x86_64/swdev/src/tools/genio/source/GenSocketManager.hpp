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

#ifndef GENSOCKETMANAGER_HPP
#define GENSOCKETMANAGER_HPP

#include "genio_export.h"

#include <string>
#include <vector>

#include "GenIP.hpp"

namespace GenSockets
{
class GENIO_EXPORT GenSocketManager
{
public:
   static void        Ready();
   static std::string GetLastError();
   static void        PrintLastError(const std::string& aHeader);
   static int         GetIP_Count();
   static GenIP       GetIP(int aIndex);
   static GenIP       GetInterfaceLocalIP(const std::string& aInterfaceIP);
   ~GenSocketManager();

protected:
   GenSocketManager();

   void InsertIP(const GenIP& aIP);

   void Startup();

   void Shutdown();

   static GenSocketManager& GetInstance();

   void CreateIP_List();

   bool               mIsInitialized;
   std::vector<GenIP> mIP_List;
};
} // namespace GenSockets

#endif
