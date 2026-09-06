// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: WsfBMDisseminateC2.hpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name:	Radiance Technologies
*   Address:			350 Wynn Drive
*   					   Huntsville, AL 35805
*
*   Abstract: This is a core class implementation for a HELIOS-like C2 dissemination
*    model. It may be used or extended to meet individual IADS needs.
*
*******************************************************************************/

#ifndef WSF_BM_DISSEMINATE_C2_HPP
#define WSF_BM_DISSEMINATE_C2_HPP

namespace il
{
   class AssetManagerInterface;
   class DisseminateC2Interface;
}

class WsfComm;

#include <memory>
#include <WsfApplication.hpp>

#include <script/WsfScriptProcessor.hpp>
#include <script/WsfScriptContext.hpp>

#include <WsfC2ComponentRefInterface.hpp>
#include <WsfScriptOverridableProcessor.hpp>
#include "WsfBMCommon.hpp"
#include <script/WsfScriptProcessor.hpp>
#include <UtPlugin.hpp>
#include <WsfDefaultDisseminationImpl.hpp>
#include <WsfScriptDisseminateC2Class.hpp>

class WsfBMDisseminateC2 : public WsfScriptProcessor, public WsfC2ComponentContainer, public WsfScriptOverridableProcessor
{
   public:

      WsfBMDisseminateC2(WsfScenario&       aScenario,
                         const std::string& aPartClass = WsfScriptDisseminateC2Class::SCRIPT_CLASS_NAME,
                         const std::string& aPartVarName = "PROCESSOR",
                         const std::shared_ptr<il::DisseminateC2Interface>& dc2_component = std::make_shared<il::DisseminateC2Interface>());
      virtual ~WsfBMDisseminateC2();

      WsfProcessor* Clone() const override;

      // Plugin Interface
      static WSF_IADS_C2_LIB_EXPORT void RegisterScriptTypes(WsfApplication* aApplicationPtr);
      static WSF_IADS_C2_LIB_EXPORT WsfProcessor* Create(WsfScenario& aScenario);
      static WSF_IADS_C2_LIB_EXPORT std::unique_ptr<WsfBMDisseminateC2> CreateUnique(WsfScenario& aScenario);
      // End Plugin Interface

      bool ProcessInput(UtInput& aInput) override;

      const char* GetScriptClassName() const override;
      virtual const char* GetBaseClassName();

      bool Initialize(double aSimTime) override;
      bool Initialize2(double aSimTime) override;
      void Update(double aSimTime) override;
      void Run(const double aSimTime) override; // HELIOS C2 component interface for invoking processor periodic updates
      bool ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;
      bool ReceiveMessage(double aSimTime, const WsfMessage& aMessage) override;

      virtual bool SendMessage(double aSimTime, const WsfMessage& aMessage, const WsfPlatform* destination);

   protected:

      WsfBMDisseminateC2(const WsfBMDisseminateC2& from);

      virtual bool ProcessDisseminateC2Input(UtInput& aInput);
      virtual wsf::comm::Comm* SelectCommDevice(const WsfPlatform* aRecipientPtr, WsfStringId  aCommName);

   private:

      WsfBMDisseminateC2& operator=(const WsfBMDisseminateC2& rhs) = delete;

      WsfDefaultDisseminationImpl m_default_dissem_impl;
};

#endif
