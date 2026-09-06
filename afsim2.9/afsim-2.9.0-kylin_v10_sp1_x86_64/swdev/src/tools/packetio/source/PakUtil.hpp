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

#ifndef PAKUTIL_HPP
#define PAKUTIL_HPP

#include "packetio_export.h"

#include <cstring>

#include "PakO.hpp"
#include "PakSerialize.hpp"
class PakSerializable;

class PACKETIO_EXPORT PakUtil
{
public:
   template<typename PKT_A, typename PKT_B>
   static bool Equal(PKT_A& a, PKT_B& b)
   {
      bool      ok = false;
      GenBuffer mem1;
      PakO      writer1(&mem1);
      a.Serialize(writer1);
      GenBuffer mem2;
      PakO      writer2(&mem2);
      b.Serialize(writer2);
      int len1 = mem1.GetPutPos();
      int len2 = mem2.GetPutPos();
      if (len1 == len2)
      {
         ok = (0 == memcmp(mem1.GetBuffer(), mem2.GetBuffer(), len1));
      }
      return ok;
   }

protected:
};

#endif
