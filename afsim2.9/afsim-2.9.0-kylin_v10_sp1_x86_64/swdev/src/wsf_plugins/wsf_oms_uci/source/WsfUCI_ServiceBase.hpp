// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFUCI_SERVICEBASE_HPP
#define WSFUCI_SERVICEBASE_HPP

namespace wsf
{
   class UCI_ServiceBase
   {
      public:
         UCI_ServiceBase() = default;
         virtual ~UCI_ServiceBase() = default;
         UCI_ServiceBase(const UCI_ServiceBase& aSrc) = delete;
         const UCI_ServiceBase& operator=(const UCI_ServiceBase& aRhs) = delete;

         virtual void Shutdown() = 0;

      protected:

   };

}

#endif
