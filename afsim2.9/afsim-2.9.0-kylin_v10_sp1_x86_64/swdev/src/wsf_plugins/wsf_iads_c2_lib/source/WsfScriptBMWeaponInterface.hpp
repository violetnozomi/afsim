// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#ifndef WSF_SCRIPT_BM_WEAPON_INTERFACE_HPP
#define WSF_SCRIPT_BM_WEAPON_INTERFACE_HPP

#include <UtInput.hpp>
#include <UtScriptBasicTypes.hpp>
#include "WsfBMCommon.hpp"
#include <iadsLib/enum.hpp>
#include <UtMemory.hpp>

class WsfBMWeaponInterface;
class WsfSimulation;

class WSF_IADS_C2_LIB_EXPORT WsfScriptBMWeaponInterface : public UtScriptClass
{
   public:
      WsfScriptBMWeaponInterface(const std::string& aClassName, UtScriptTypes*     aScriptTypesPtr);
      virtual ~WsfScriptBMWeaponInterface();

      void* Create(const UtScriptContext& aInstance) override;
      void* Clone(void* aObjectPtr) override;
      void Destroy(void* aObjectPtr) override;

      UT_DECLARE_SCRIPT_METHOD(SetSAMInfo);
      UT_DECLARE_SCRIPT_METHOD(SetAIInfo);
      UT_DECLARE_SCRIPT_METHOD(GetNumMunitions);
      UT_DECLARE_SCRIPT_METHOD(GetMaxEngagements);
      UT_DECLARE_SCRIPT_METHOD(GetInterceptSpeed);
      UT_DECLARE_SCRIPT_METHOD(GetWsysID);
      UT_DECLARE_SCRIPT_METHOD(AddMez);
      UT_DECLARE_SCRIPT_METHOD(AddFez);
      UT_DECLARE_SCRIPT_METHOD(AddJez);

      static void AttachWeaponZone(WsfBMWeaponInterface* aObjectPtr,
                                   void* wsf_zone_obj,
                                   il::tZonePriority priority,
                                   il::eZoneType type,
                                   WsfSimulation* sim_ptr);
};

#endif
