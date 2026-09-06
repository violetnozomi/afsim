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
#ifndef GENSOCKETSELECTOR_HPP
#define GENSOCKETSELECTOR_HPP

#include "genio_export.h"

#include "GenSocketSet.hpp"

namespace GenSockets
{
class GenSocket;
/**  GenSocketSelector allows select() to be called on a set of sockets
 */
class GENIO_EXPORT GenSocketSelector : public GenSocketSet
{
public:
   enum WaitEvent
   {
      cREAD      = 1,
      cWRITE     = 2,
      cEXCEPTION = 4,
      cALL       = cREAD | cWRITE | cEXCEPTION
   };

   enum SelectResult
   {
      //! Wait time has expired without any events
      cTIMEOUT,
      //! One or more socket has been signaled.
      cREADY,
      //! An error has occurred
      cERROR
   };

   static const float cBLOCK_FOREVER;
   GenSocketSelector();
   ~GenSocketSelector();
   SelectResult Select(GenSocketSet& aSignalledSocketSet, float aWaitTime = cBLOCK_FOREVER, int aSignalledEvent = cREAD);
};

} // namespace GenSockets
#endif
