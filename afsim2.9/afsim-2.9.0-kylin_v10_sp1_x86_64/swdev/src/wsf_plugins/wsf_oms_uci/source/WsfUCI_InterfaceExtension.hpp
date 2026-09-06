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

#ifndef WSFUCI_INTERFACEEXTENSION_HPP
#define WSFUCI_INTERFACEEXTENSION_HPP

#include <map>
#include <memory>
#include <string>

#include "wsf_oms_uci_export.h"

#include "WsfSimulation.hpp"
#include "WsfSimulationExtension.hpp"

namespace wsf
{
   class UCI_Component;
   class UCI_Interface;

   //! The OMS / UCI Interface Object to the UCI middleware.
   class WSF_OMS_UCI_EXPORT UCI_InterfaceExtension : public WsfSimulationExtension
   {
      public:

         explicit UCI_InterfaceExtension() = default;
         UCI_InterfaceExtension(const UCI_InterfaceExtension& aSrc) = default;
         virtual ~UCI_InterfaceExtension() = default;
         UCI_InterfaceExtension& operator=(const UCI_InterfaceExtension& aRhs) = delete;

         void AddedToSimulation() override;
         static const char* GetExtensionName() { return "uci_component"; }
         static const UCI_InterfaceExtension* Get(WsfSimulation& aSimulation);
         UCI_Interface* GetInterface() const;

      private:

         std::shared_ptr<UCI_Interface> mInterfacePtr { nullptr };
         mutable bool                   mInitialized  { false };
         std::string                    mSystemName   { "default" };
         std::string                    mDescriptor   { "uci"};
   };
}

#endif
