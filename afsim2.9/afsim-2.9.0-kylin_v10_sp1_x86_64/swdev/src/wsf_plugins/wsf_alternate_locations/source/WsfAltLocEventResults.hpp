// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WSF_ALT_LOC_EVENT_RESULTS_HPP
#define WSF_ALT_LOC_EVENT_RESULTS_HPP

#include "WsfAltLocComponent.hpp"
#include "WsfEventResult.hpp"
class WsfPlatform;

namespace wsf
{
namespace altloc
{
class ComponentInitialized : public wsf::event::Result
{
public:
   ComponentInitialized(double aSimTime, WsfPlatform* aPlatformPtr, Component* aComponentPtr, wsf::event::Settings& aSettings)
      : Result(aSimTime, aSettings)
      , mPlatformPtr(aPlatformPtr)
      , mComponentPtr(aComponentPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

private:
   std::string GetDrawType() const;

   WsfPlatform* mPlatformPtr;
   Component*   mComponentPtr;
};
} // namespace altloc
} // namespace wsf
#endif
