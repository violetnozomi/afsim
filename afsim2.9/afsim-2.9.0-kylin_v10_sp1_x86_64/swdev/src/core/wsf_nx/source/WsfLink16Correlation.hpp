// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFLINK16CORRELATION_HPP
#define WSFLINK16CORRELATION_HPP

#include <map>
#include <set>

#include "UtEntity.hpp"
class UtInput;
#include "WsfCorrelationStrategy.hpp"
#include "WsfCovariance.hpp"
#include "WsfTrackId.hpp"

//! An implementation of WsfCorrelationStrategy for which the spatially closest track is
//! selected as being associated with the measurement under consideration.
class WsfLink16Correlation : public WsfCorrelationStrategy
{
public:
   WsfLink16Correlation();
   ~WsfLink16Correlation() override = default;

   WsfCorrelationStrategy* Clone() const override { return new WsfLink16Correlation(*this); }

   WsfLocalTrack* CorrelateImpl(double aSimTime, const WsfTrack& aNonLocalTrack, WsfLocalTrackList& aTrackList) override;

   bool ProcessInput(UtInput& aInput) override;

private:
   bool PositionalTest(unsigned int  aLocalTQ,
                       unsigned int  aRemoteTQ,
                       double        aDistance,
                       unsigned int  aMinTQ,
                       unsigned int  aMaxTQ,
                       const double* aTQ_ToDM_ConversionTable,
                       double        aScaleFactor = 1.0) const;

   bool CourseTest(const WsfTrack& aNewTrack, const WsfTrack& aLocalTrack) const;

   bool SpeedTest(const WsfTrack& aNewTrack, const WsfTrack& aLocalTrack) const;

   bool AltitudeTest(const WsfTrack& aTrack1, const WsfTrack& aTrack2) const;

   double GetDistanceBetweenTracks(const WsfTrack& aTrack1, const WsfTrack& aTrack2) const;

   bool RunCorrelationTest(double aSimTime, const WsfTrack& aNewTrack, const WsfLocalTrack& aLocalTrack);

   bool RunDecorrelationTest(double aSimTime, const WsfTrack& aNewTrack, const WsfLocalTrack& aLocalTrack);

   double GetCourse(const WsfTrack& aTrack) const;

   //#########################################################################
   //#Allows increasing or decreasing the standard TQ-based window
   //#sizes to be used by all IUs in an interface
   //#    a = 1.0(0.5 - 3.0, 0.1)
   double mWindowSizeMultiplier; // = 1.0;

   //#########################################################################
   //#A value applied to all calculated windows to insure that windows
   //#are not so small as to prevent valid correlations, e.g., due to
   //#minor errors introduced by extrapolation.
   //#    b = 0.5(0 - 2.0, 0.25) dm

   double mMinimumWindowSize; // = 0.5;

   //#########################################################################
   //#The minimum TQ to  be used in positional correlation calculations.
   //#Lower TQs shall be treated as if they were c.  This prevents correlation
   //#windows from being unrealistically large.  Note: The value used for c can
   //#never be less than or equal to the value used for e (Restriced_TQ).
   //#    c = 7(3 - 7, 1)

   unsigned mMinimumTQ; // = 7

   //#########################################################################
   //#The maximum TQ to be used in positional correlations calculations.
   //#Higher TQs shall be treated as if they were d. This prevents correlation
   //#windows from being unrealistically small.
   //#    d = 10(8 - 15, 1)

   unsigned mMaximumTQ; // = 10

   //#########################################################################
   //#The minimum value of Qpg to be used in positional correlation calculations.
   //#Lower values shall be treated as if they were j.  This prevents correlation
   //#windows used in testing correlation of PPLI positions from being
   //#unrealistically large.
   //#    j = 2(1 - 5, 1)

   unsigned mMinimumPPLI_TQ; // = 2

   //#########################################################################
   //# The maximum value of Qpg to be used in positional correlation calculations.
   //#Higher values shall be treated as if they were k.  This prevents correlation
   //#windows used in testing correlation of PLI positions from being
   //#unrealistically small.
   //# k = 11(1 - 15, 1)

   unsigned mMaximumPPLI_TQ; // = 11

   //#########################################################################
   //#Tracks with TQ less than or equal to e are not eligible for correlation
   //#    e = 4(2 - 6, 1)

   unsigned mRestrictedTQ; // = 4

   //#########################################################################
   //#The maximum difference between the reported course of the remote track
   //#and the calculated course of the local track allowable for correlation.
   //#If the speed of either track is less than 10 dmh, "course differential"
   //#shall not be applied in the correlation test.
   //#    f = 45(15 - 90, 15) degrees

   double mCourseDifferential; // = 45
   double mMinimumSpeed;       // = 10

   //#########################################################################
   //#The maximum percentage by which the speed of the faster track may differ
   //#from the speed of the slower track for correlation.
   //#    g = 40(10 - 100, 10) percent

   double mSpeedDifferential; // = 40
   double mSpeedDeltaAir;     // = 200
   double mSpeedDeltaSurface; // = 20

   //#########################################################################
   //#The maximum altitude difference between two air tracks allowable for
   //#correlation.  (Not applicable to surface track correlation.)
   //#    h = 10(5 - 50, 5) thousand feet (kft)
   double mAltitudeDifferential; // = 10

   double mMinDistance;

   mutable UtEntity mConverter;
};

#endif
