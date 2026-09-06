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

#ifndef DISDELETEOBJ_HPP
#define DISDELETEOBJ_HPP

struct DisDeleteObj
{
   DisDeleteObj()
      : x(0)
   {
   }

   template<class T>
   void operator()(const T* ptr) const
   {
      delete ptr;
   }

   // Dummy variable just to keep Purify from reporting UMRs
   unsigned int x;
};

#endif
