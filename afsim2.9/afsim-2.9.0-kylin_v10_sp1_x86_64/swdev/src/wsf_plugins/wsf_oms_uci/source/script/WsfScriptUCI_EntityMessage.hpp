// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UCIENTITYMESSAGECLASS_HPP
#define UCIENTITYMESSAGECLASS_HPP

#include "wsf_oms_uci_export.h"

class UtScriptTypes;

#include "WsfScriptUCI_Message.hpp"

namespace wsf
{

   namespace script
   {

      //! Define script methods for EntityMessage.
      class WSF_OMS_UCI_EXPORT UCI_EntityMessageClass : public UCI_MessageClass
      {
         public:
            UCI_EntityMessageClass(const std::string& aClassName,
                                   UtScriptTypes*     aTypesPtr);

      };

   }

}

#endif
