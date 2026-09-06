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
*   File: WsfC2ComponentRefInterface.hpp
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
*   Abstract:
*
*******************************************************************************/

#ifndef WSF_C2_COMPONENT_REF_INTERFACE_HPP
#define WSF_C2_COMPONENT_REF_INTERFACE_HPP

#include <exception>
#include <memory>

#include <WsfBMCommon.hpp>

#include <iadsLib/assetManagerInterface.hpp>
#include <iadsLib/BattleManagerIface.hpp>
#include <iadsLib/disseminateC2Interface.hpp>
#include <iadsLib/weaponsManagerInterface.hpp>
#include <iadsLib/sensorsManagerInterface.hpp>
#include <iadsLib/moeLoggingInterface.hpp>

class WsfC2ComponentContainer
{
   public:
      explicit WsfC2ComponentContainer(const std::shared_ptr<il::BattleManagerInterface>& component, const std::shared_ptr<il::moeLoggingInterface>& logger = nullptr);
      explicit WsfC2ComponentContainer(const std::shared_ptr<il::AssetManagerInterface>& component, const std::shared_ptr<il::moeLoggingInterface>& logger = nullptr);
      explicit WsfC2ComponentContainer(const std::shared_ptr<il::DisseminateC2Interface>& component, const std::shared_ptr<il::moeLoggingInterface>& logger = nullptr);
      explicit WsfC2ComponentContainer(const std::shared_ptr<il::WeaponsManagerInterface>& component, const std::shared_ptr<il::moeLoggingInterface>& logger = nullptr);
      explicit WsfC2ComponentContainer(const std::shared_ptr<il::SensorsManagerInterface>& component, const std::shared_ptr<il::moeLoggingInterface>& logger = nullptr);

      virtual WSF_IADS_C2_LIB_EXPORT bool SetComponent(const std::shared_ptr<il::BattleManagerInterface>& component);
      virtual WSF_IADS_C2_LIB_EXPORT bool SetComponent(const std::shared_ptr<il::AssetManagerInterface>& component);
      virtual WSF_IADS_C2_LIB_EXPORT bool SetComponent(const std::shared_ptr<il::DisseminateC2Interface>& component);
      virtual WSF_IADS_C2_LIB_EXPORT bool SetComponent(const std::shared_ptr<il::WeaponsManagerInterface>& component);
      virtual WSF_IADS_C2_LIB_EXPORT bool SetComponent(const std::shared_ptr<il::SensorsManagerInterface>& component);

      inline virtual std::shared_ptr<il::BattleManagerInterface> GetCoreBMRef() const { return m_bm_component; }
      inline virtual std::shared_ptr<il::AssetManagerInterface>  GetCoreAMRef() const { return m_am_component; }
      inline virtual std::shared_ptr<il::DisseminateC2Interface> GetCoreDC2Ref() const { return m_dc2_component; }
      inline virtual std::shared_ptr<il::SensorsManagerInterface> GetCoreSMRef() const { return m_sm_component; }
      inline virtual std::shared_ptr<il::WeaponsManagerInterface> GetCoreWMRef() const { return m_wm_component; }

      inline virtual bool HasBMComponent() const { return m_bm_component.get() != nullptr; }
      inline virtual bool HasAMComponent() const { return m_am_component.get() != nullptr; }
      inline virtual bool HasDC2Component() const { return m_dc2_component.get() != nullptr; }
      inline virtual bool HasWMComponent() const { return m_wm_component.get() != nullptr; }
      inline virtual bool HasSMComponent() const { return m_sm_component.get() != nullptr; }

   protected:
      std::shared_ptr<il::BattleManagerInterface> m_bm_component;
      std::shared_ptr<il::AssetManagerInterface>  m_am_component;
      std::shared_ptr<il::DisseminateC2Interface> m_dc2_component;
      std::shared_ptr<il::WeaponsManagerInterface> m_wm_component;
      std::shared_ptr<il::SensorsManagerInterface> m_sm_component;

      std::shared_ptr<il::moeLoggingInterface> m_moe_logger;

   private:
      const bool m_is_bm_class;
      const bool m_is_am_class;
      const bool m_is_dc2_class;
      const bool m_is_wm_class;
      const bool m_is_sm_class;
};

#endif
