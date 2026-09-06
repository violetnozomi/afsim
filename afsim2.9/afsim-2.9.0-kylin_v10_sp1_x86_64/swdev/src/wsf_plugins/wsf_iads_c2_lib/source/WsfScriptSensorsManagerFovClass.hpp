// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WSF_SCRIPT_SENSORS_MANAGER_FOV_CLASS_HPP
#define WSF_SCRIPT_SENSORS_MANAGER_FOV_CLASS_HPP

#include "WsfBMCommon.hpp"
#include "script/WsfScriptProcessorClass.hpp"

class UtScriptContext;
class UtScriptTypes;

class WsfScriptSensorsManagerFovClass : public WsfScriptProcessorClass
{
   public:
      static WSF_IADS_C2_LIB_EXPORT const char* GetScriptClassName();
      static WSF_IADS_C2_LIB_EXPORT const char* GetBaseClassName();

      WsfScriptSensorsManagerFovClass(UtScriptTypes* scriptTypes);

      void* Create(const UtScriptContext& context) override;
      void* Clone(void* obj) override;
      void Destroy(void* obj) override;

   private:
      WsfScriptSensorsManagerFovClass() = delete;
      WsfScriptSensorsManagerFovClass(const WsfScriptSensorsManagerFovClass&) = delete;
      WsfScriptSensorsManagerFovClass& operator=(const WsfScriptSensorsManagerFovClass&) = delete;
};

#endif
