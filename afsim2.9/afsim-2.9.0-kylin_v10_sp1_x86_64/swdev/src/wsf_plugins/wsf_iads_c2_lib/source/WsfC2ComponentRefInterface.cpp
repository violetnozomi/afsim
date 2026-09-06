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
*   File: WsfC2ComponentRefInterface.cpp
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

#include <WsfC2ComponentRefInterface.hpp>
#include <iadsLib/referencesProcessor.hpp>

#include <logger.hpp>

WsfC2ComponentContainer::WsfC2ComponentContainer(const std::shared_ptr<il::BattleManagerInterface>& component, const std::shared_ptr<il::moeLoggingInterface>& logger)
   : m_bm_component(component)
   , m_moe_logger(logger)
   , m_is_bm_class(true)
   , m_is_am_class(false)
   , m_is_dc2_class(false)
   , m_is_wm_class(false)
   , m_is_sm_class(false)
{ }

WsfC2ComponentContainer::WsfC2ComponentContainer(const std::shared_ptr<il::AssetManagerInterface>& component, const std::shared_ptr<il::moeLoggingInterface>& logger)
   : m_am_component(component)
   , m_moe_logger(logger)
   , m_is_bm_class(false)
   , m_is_am_class(true)
   , m_is_dc2_class(false)
   , m_is_wm_class(false)
   , m_is_sm_class(false)
{ }

WsfC2ComponentContainer::WsfC2ComponentContainer(const std::shared_ptr<il::DisseminateC2Interface>& component, const std::shared_ptr<il::moeLoggingInterface>& logger)
   : m_dc2_component(component)
   , m_moe_logger(logger)
   , m_is_bm_class(false)
   , m_is_am_class(false)
   , m_is_dc2_class(true)
   , m_is_wm_class(false)
   , m_is_sm_class(false)
{ }

WsfC2ComponentContainer::WsfC2ComponentContainer(const std::shared_ptr<il::WeaponsManagerInterface>& component, const std::shared_ptr<il::moeLoggingInterface>& logger)
   : m_wm_component(component)
   , m_moe_logger(logger)
   , m_is_bm_class(false)
   , m_is_am_class(false)
   , m_is_dc2_class(false)
   , m_is_wm_class(true)
   , m_is_sm_class(false)
{ }

WsfC2ComponentContainer::WsfC2ComponentContainer(const std::shared_ptr<il::SensorsManagerInterface>& component, const std::shared_ptr<il::moeLoggingInterface>& logger)
   : m_sm_component(component)
   , m_moe_logger(logger)
   , m_is_bm_class(false)
   , m_is_am_class(false)
   , m_is_dc2_class(false)
   , m_is_wm_class(false)
   , m_is_sm_class(true)
{ }

bool WsfC2ComponentContainer::SetComponent(const std::shared_ptr<il::BattleManagerInterface>& component)
{
   if (m_is_bm_class)
   {
      // attempt to redefine underlying type
      HCL_FATAL_LOGGER(component->getAssetManagerRef()->getGlobalLogger()) << "WsfC2ComponentContainer attempted to reset underlying battle manager.";
      throw std::runtime_error("WsfC2ComponentContainer attempted to reset underlying battle manager.");
   }
   else
   {
      m_bm_component = component;
   }

   // set up all cross-dependent references in a semi-generic manner
   auto TrySetupBMRef = [](il::ReferencesBattleManager* container, const std::shared_ptr<il::BattleManagerInterface>& component)
   {
      if (container != nullptr)
      {
         container->SetBattleManager(component);
      }
   };

   TrySetupBMRef(dynamic_cast<il::ReferencesBattleManager*>(m_am_component.get()), component);
   TrySetupBMRef(dynamic_cast<il::ReferencesBattleManager*>(m_dc2_component.get()), component);
   TrySetupBMRef(dynamic_cast<il::ReferencesBattleManager*>(m_wm_component.get()), component);
   TrySetupBMRef(dynamic_cast<il::ReferencesBattleManager*>(m_sm_component.get()), component);

   return HasBMComponent();
}

bool WsfC2ComponentContainer::SetComponent(const std::shared_ptr<il::AssetManagerInterface>& component)
{
   if (m_is_am_class)
   {
      // attempt to redefine underlying type
      HCL_FATAL_LOGGER(component->getGlobalLogger()) << "WsfC2ComponentContainer attempted to reset underlying asset manager.";
      throw std::runtime_error("WsfC2ComponentContainer attempted to reset underlying asset manager.");
   }
   else
   {
      m_am_component = component;
   }

   // set up all cross-dependent references in a generic manner
   auto TrySetupAMRef = [](il::ReferencesAssetManager* container, const std::shared_ptr<il::AssetManagerInterface>& component)
   {
      if (container != nullptr)
      {
         container->SetAssetManager(component);
      }
   };

   TrySetupAMRef(dynamic_cast<il::ReferencesAssetManager*>(m_bm_component.get()), component);
   TrySetupAMRef(dynamic_cast<il::ReferencesAssetManager*>(m_dc2_component.get()), component);
   TrySetupAMRef(dynamic_cast<il::ReferencesAssetManager*>(m_wm_component.get()), component);
   TrySetupAMRef(dynamic_cast<il::ReferencesAssetManager*>(m_sm_component.get()), component);

   return HasAMComponent();
}

bool WsfC2ComponentContainer::SetComponent(const std::shared_ptr<il::DisseminateC2Interface>& component)
{
   if (m_is_dc2_class)
   {
      // attempt to redefine underlying type
      HCL_FATAL_LOGGER(component->GetAssetManager()->getGlobalLogger()) << "WsfC2ComponentContainer attempted to reset underlying disseminate C2 processor.";
      throw std::runtime_error("WsfC2ComponentContainer attempted to reset underlying disseminate C2 processor.");
   }
   else
   {
      m_dc2_component = component;
   }

   // set up all cross-dependent references in a generic manner
   auto TrySetupDC2Ref = [](il::ReferencesC2Dissemination* container, const std::shared_ptr<il::DisseminateC2Interface>& component)
   {
      if (container != nullptr)
      {
         container->SetC2Dissemination(component);
      }
   };

   TrySetupDC2Ref(dynamic_cast<il::ReferencesC2Dissemination*>(m_bm_component.get()), component);
   TrySetupDC2Ref(dynamic_cast<il::ReferencesC2Dissemination*>(m_am_component.get()), component);
   TrySetupDC2Ref(dynamic_cast<il::ReferencesC2Dissemination*>(m_wm_component.get()), component);
   TrySetupDC2Ref(dynamic_cast<il::ReferencesC2Dissemination*>(m_sm_component.get()), component);

   return HasDC2Component();
}

bool WsfC2ComponentContainer::SetComponent(const std::shared_ptr<il::WeaponsManagerInterface>& component)
{
   if (m_is_wm_class)
   {
      // attempt to redefine underlying type
      HCL_FATAL_LOGGER(component->GetAssetManager()->getGlobalLogger()) << "WsfC2ComponentContainer attempted to reset underlying weapons manager processor.";
      throw std::runtime_error("WsfC2ComponentContainer attempted to reset underlying weapons manager processor.");
   }
   else
   {
      m_wm_component = component;
   }

   // set up all cross-dependent references in a generic manner
   auto TrySetupWMRef = [](il::ReferencesWeaponsManager* container, const std::shared_ptr<il::WeaponsManagerInterface>& component)
   {
      if (container != nullptr)
      {
         container->SetWeaponsManager(component);
      }
   };

   TrySetupWMRef(dynamic_cast<il::ReferencesWeaponsManager*>(m_bm_component.get()), component);
   TrySetupWMRef(dynamic_cast<il::ReferencesWeaponsManager*>(m_am_component.get()), component);
   TrySetupWMRef(dynamic_cast<il::ReferencesWeaponsManager*>(m_dc2_component.get()), component);
   TrySetupWMRef(dynamic_cast<il::ReferencesWeaponsManager*>(m_sm_component.get()), component);

   return HasWMComponent();
}

bool WsfC2ComponentContainer::SetComponent(const std::shared_ptr<il::SensorsManagerInterface>& component)
{
   if (m_is_sm_class)
   {
      // attempt to redefine underlying type
      HCL_FATAL_LOGGER(component->GetAssetManager()->getGlobalLogger()) << "WsfC2ComponentContainer attempted to reset underlying sensors manager processor.";
      throw std::runtime_error("WsfC2ComponentContainer attempted to reset underlying sensors manager processor.");
   }
   else
   {
      m_sm_component = component;
   }

   // set up all cross-dependent references in a generic manner
   auto TrySetupSMRef = [](il::ReferencesSensorsManager* container, const std::shared_ptr<il::SensorsManagerInterface>& component)
   {
      if (container != nullptr)
      {
         container->SetSensorsManager(component);
      }
   };

   TrySetupSMRef(dynamic_cast<il::ReferencesSensorsManager*>(m_bm_component.get()), component);
   TrySetupSMRef(dynamic_cast<il::ReferencesSensorsManager*>(m_am_component.get()), component);
   TrySetupSMRef(dynamic_cast<il::ReferencesSensorsManager*>(m_dc2_component.get()), component);
   TrySetupSMRef(dynamic_cast<il::ReferencesSensorsManager*>(m_wm_component.get()), component);

   return HasSMComponent();
}
