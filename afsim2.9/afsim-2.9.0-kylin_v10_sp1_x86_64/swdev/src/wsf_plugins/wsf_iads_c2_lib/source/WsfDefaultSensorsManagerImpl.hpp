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
*   File: WsfDefaultSensorsManagerImpl.hpp
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
*   Abstract: This abstract class provides default implementations in C++ for
*    what could be overridden in script for the Sensors Manager base class.
*    The intention was to provide the classical implementation of what was originally
*    done in script so default usage of the processor works without supplemental scripting.
*
*******************************************************************************/

#ifndef WSF_DEFAULT_SENSORS_MANAGER_IMPL_HPP
#define WSF_DEFAULT_SENSORS_MANAGER_IMPL_HPP

#include <memory>

#include <WsfBMCommon.hpp>
#include <WsfBMStatusMessage.hpp>
#include <WsfBMTrackMessage.hpp>

// forward declarations
class WsfPlatform;
class WsfAssetManager;
class WsfMessage;
class WsfBMSensorsManager;

class WSF_IADS_C2_LIB_EXPORT WsfDefaultSensorsManagerImpl
{
   public:
      WsfDefaultSensorsManagerImpl();

      virtual bool ProcessInput(UtInput& aInput);
      virtual void Initialize(WsfBMSensorsManager* sm);
      virtual void Initialize2(WsfBMSensorsManager* sm);

      virtual void on_update(const double sim_time_s, WsfPlatform* this_plat, WsfBMSensorsManager* sm);
      virtual bool on_message(const double sim_time_s, const WsfMessage& message, WsfBMSensorsManager* sm);

   private:
      WsfAssetManager* m_am_ref;
};

#endif
