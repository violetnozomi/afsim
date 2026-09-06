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

#ifndef GEN_UMPIO_SERVER_PRV_HPP
#define GEN_UMPIO_SERVER_PRV_HPP

#include <string>
#include <vector>

#include "GenExport.hpp"

// This class servers as a base class for a GenUmpIOServer template.  It is
// primarily needed to hold the HandleList vector and really not much more


class GEN_EXPORT GenUmpIOServerPrv
{
public:
   // Currently only a maximum of 32 objects of this class should be
   // instantiated.  Most sims will instantiate only one, so this limit
   // should not be a big deal..
   GenUmpIOServerPrv(const char* aCallingClassName, float aUpdateCallingPeriodSec);
   virtual ~GenUmpIOServerPrv();

   const std::string& GetCallingClassName() const;
   float              GetUpdatePeriodSec() const;

protected:
   static std::vector<GenUmpIOServerPrv*> sHandleList;

   std::string mCallingClassName;
   int         mHandleIndex;
   float       mUpdatePeriodSec;

   // A TCP server socket.  It is used to accept connections and can't
   // receive messages through it.
   int mSocketRecv;


private:
   // Disallow copy and assignment
   GenUmpIOServerPrv(const GenUmpIOServerPrv& aSrc);
   GenUmpIOServerPrv& operator=(const GenUmpIOServerPrv& aRhs);
};


inline const std::string& GenUmpIOServerPrv::GetCallingClassName() const
{
   return mCallingClassName;
}

inline float GenUmpIOServerPrv::GetUpdatePeriodSec() const
{
   return mUpdatePeriodSec;
}


#endif
