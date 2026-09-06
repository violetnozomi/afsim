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

#ifndef OMS_UCI_EXTENSION_HPP
#define OMS_UCI_EXTENSION_HPP

#include <memory>

#include "wsf_oms_uci_export.h"

#include "WsfScenarioExtension.hpp"
class     WsfSimulation;
#include "WsfUCI_InterfaceExtension.hpp"

namespace wsf
{
   class UCI_ComponentTypes;
   class UCI_Interface;

   class WSF_OMS_UCI_EXPORT OMS_UCI_Extension : public WsfScenarioExtension
   {
      public:

         OMS_UCI_Extension() = default;
         ~OMS_UCI_Extension() override;
         void AddedToScenario() override;
         void SimulationCreated(WsfSimulation& aSimulation) override;
         static const char* GetExtensionName() { return "wsf_oms_uci"; }
         wsf::UCI_ComponentTypes& GetUCI_ComponentTypes() const { return *mUCI_ComponentTypesPtr; }
         bool ProcessInput(UtInput& aInput) override;
         std::shared_ptr<wsf::UCI_Interface> GetInterface();
      
         static OMS_UCI_Extension& Get(const WsfScenario& aScenario);

      private:

         std::shared_ptr<wsf::UCI_Interface>     mInterfacePtr          { nullptr };
         wsf::UCI_ComponentTypes*                mUCI_ComponentTypesPtr { nullptr };
         std::unique_ptr<UCI_InterfaceExtension> mInterfaceExtensionPtr {  ut::make_unique<UCI_InterfaceExtension>() };
   };
} // namespace wsf

#endif
