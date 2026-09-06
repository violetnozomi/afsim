// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WSF_ALT_LOC_SIMULATION_EXTENSION_HPP
#define WSF_ALT_LOC_SIMULATION_EXTENSION_HPP

#include "wsf_alternate_locations_export.h"

#include "UtCallback.hpp"
#include "WsfAltLocComponent.hpp"
class WsfPlatform;
#include "WsfSimulationExtension.hpp"

// --------- AlternateLocationsObserver
//! Here we add a new callback signature and exported method to the WsfObserver namespace. This will allow
//! us (and others) to subscribe to our new observer.
namespace WsfObserver
{
using EventCallback = UtCallbackListN<void(double, WsfPlatform*, wsf::altloc::Component*)>;

WSF_ALTERNATE_LOCATIONS_EXPORT EventCallback& Event(const WsfSimulation* aSimulationPtr);
} // namespace WsfObserver

namespace wsf
{
namespace altloc
{
struct InputData
{
   bool   mGlobalDebugEnabled{false};
   bool   mGlobalDrawEnabled{false};
   double mGlobalDraw{Component::cINVALID_DRAW};
};

class SimulationExtension : public WsfSimulationExtension
{
public:
   static constexpr const char* cEXTENSION = "wsf_alternate_locations";

   SimulationExtension(const InputData& aData)
      : mData(aData)
   {
   }

   void AddedToSimulation() override;

   static SimulationExtension& Get(const WsfSimulation& aSimulation);

   WsfObserver::EventCallback& GetObserver() { return mAltLocEventCallback; }

   bool   GetGlobalDebugEnabled() const { return mData.mGlobalDebugEnabled; }
   bool   GetGlobalDrawEnabled() const { return mData.mGlobalDrawEnabled; }
   double GetDraw();

private:
   WsfObserver::EventCallback mAltLocEventCallback;
   InputData                  mData;
};
} // namespace altloc
} // namespace wsf

#endif
