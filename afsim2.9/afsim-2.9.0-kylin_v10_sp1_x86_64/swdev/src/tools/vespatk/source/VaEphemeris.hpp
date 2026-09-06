// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef VaEphemeris_hpp
#define VaEphemeris_hpp

#include "vespatk_export.h"

#include <map>

#include "UtCalendar.hpp"
#include "UtoEphemerisNode.hpp"
#include "VaCallbackHolder.hpp"

class UtoMapProjection;

namespace vespa
{
class VaViewer;

class VESPATK_EXPORT VaEphemeris
{
public:
   static void Initialize();

   static void Draw();

   static void              SetStartTime(const UtCalendar& aCalendar);
   static const UtCalendar& GetStartTime() { return mStartTime; }
   static const UtCalendar& GetSimTime() { return mSimTime; }

   static void ViewerInitializedCB(VaViewer* aViewerPtr);
   static void ViewerDestroyedCB(VaViewer* aViewerPtr);

protected:
   VaEphemeris();
   virtual ~VaEphemeris();

private:
   static void TimeUpdatedCB(double aTime, double aRate, double aMax, double aMin);

   static UtCalendar                                                            mStartTime;
   static UtCalendar                                                            mSimTime;
   static std::map<int, std::pair<osg::ref_ptr<Uto::EphemerisNode>, VaViewer*>> mNodeMap;
   static std::map<int, osg::ref_ptr<Uto::EphemerisMoon>>                       mMoonMap;
   static bool                                                                  mUseSimTime;
   static VaCallbackHolder                                                      mCallbacks;
   static bool                                                                  mTimeSetManually;
};

} // namespace vespa

#endif // VaEphemeris_hpp
