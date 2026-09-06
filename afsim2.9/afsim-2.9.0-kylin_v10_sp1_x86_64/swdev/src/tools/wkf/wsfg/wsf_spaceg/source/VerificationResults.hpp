// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef VERIFICATIONRESULTS_HPP
#define VERIFICATIONRESULTS_HPP

#include <string>

#include "UtColor.hpp"
#include "UtOrbitalElements.hpp"
#include "WsfOrbitalEvent.hpp"
#include "WsfSpaceVerifyTypes.hpp"

namespace wsfg
{

namespace spaceg
{

class VerificationResults
{
public:
   VerificationResults();

   //! Clear the verification results.
   void Clear();

   //! Return the verification status of the associated event.
   wsf::space::verify::Status GetStatus() const { return mStatus; }

   //! Returns true if verification for the associated event was passed.
   bool Passed() const { return mStatus == wsf::space::verify::Status::cPASS; }

   //! Returns true if verification for the associated event failed.
   bool Failed() const { return mStatus == wsf::space::verify::Status::cFAIL; }

   //! Returns true if the associated event is unverifiable.
   bool Unverifiable() const { return mStatus == wsf::space::verify::Status::cUNVERIFIABLE; }

   //! Return if the verification has been attempted.
   bool VerificationAttempted() const { return mStatus != wsf::space::verify::Status::cNONE; }

   //! Set the verification status.
   void SetStatus(wsf::space::verify::Status aStatus) { mStatus = aStatus; }

   //! Get the delta v available before the event in m/s.
   double GetDeltaV_AvailBefore() const { return mDeltaV_AvailBefore; }

   //! Get the delta v available after the event in m/s.
   double GetDeltaV_AvailAfter() const { return mDeltaV_AvailAfter; }

   //! Get the orbital elements before the event.
   const UtOrbitalElements& GetElementsBefore() const { return mElementsBefore; }

   //! Get the orbital elements after the event.
   const UtOrbitalElements& GetElementsAfter() const { return mElementsAfter; }

   //! Get the output log produced during verification of the event.
   const std::string& GetOutputLog() const { return mOutputLog; }

   //! Get the date and time before the event.
   UtCalendar GetTimeBefore() const { return mTimeBefore; }

   //! Get the date and time after the event.
   UtCalendar GetTimeAfter() const { return mTimeAfter; }

   //! Get the sim time before the event.
   double GetSecondsBefore() const { return mSecondsBefore; }

   //! Get the sim time after the event.
   double GetSecondsAfter() const { return mSecondsAfter; }

   //! Get the date and time of the event.
   UtCalendar GetEventTime() const { return mTimeAfter; }

   //! Get the sim time of the event.
   double GetEventSeconds() const { return mSecondsAfter; }

   //! Get the delta V cost of the event.
   double GetDeltaV() const { return mDeltaV_AvailBefore - mDeltaV_AvailAfter; }

   //! Get the delta v budget remaining after the event.
   double GetDeltaV_Budget() const { return mDeltaV_AvailAfter; }

   //! Get the event's preview color.
   UtColor GetPreviewColor() const { return mPreviewColor; }

   //! Set the delta v available before the event.
   void SetDeltaV_AvailBefore(double aDeltaV) { mDeltaV_AvailBefore = aDeltaV; }

   //! Set the delta v available after the event.
   void SetDeltaV_AvailAfter(double aDeltaV) { mDeltaV_AvailAfter = aDeltaV; }

   //! Set the orbital elements before the event.
   void SetElementsBefore(const UtOrbitalElements& aElements) { mElementsBefore = aElements; }

   //! Set the orbital elements after the event.
   void SetElementsAfter(const UtOrbitalElements& aElements) { mElementsAfter = aElements; }

   //! Set the date and time before the event.
   void SetTimeBefore(const UtCalendar& aTime) { mTimeBefore = aTime; }

   //! Set the date and time after the event.
   void SetTimeAfter(const UtCalendar& aTime) { mTimeAfter = aTime; }

   //! Set the sim time before the event.
   void SetSecondsBefore(double aSeconds) { mSecondsBefore = aSeconds; }

   //! Set the sim time after the event.
   void SetSecondsAfter(double aSeconds) { mSecondsAfter = aSeconds; }

   //! Set the output log for the output produced by the verification of the event.
   void SetOutputLog(const std::string& aLog) { mOutputLog = aLog; }

   //! Set the event's preview color.
   void SetPreviewColor(const UtColor& aColor) { mPreviewColor = aColor; }

private:
   wsf::space::verify::Status mStatus;
   UtCalendar                 mTimeBefore;
   UtCalendar                 mTimeAfter;
   double                     mSecondsBefore;
   double                     mSecondsAfter;
   UtOrbitalElements          mElementsBefore;
   UtOrbitalElements          mElementsAfter;
   double                     mDeltaV_AvailBefore;
   double                     mDeltaV_AvailAfter;
   std::string                mOutputLog;
   UtColor                    mPreviewColor;
};

std::string VerificationStatusDescriptor(wsf::space::verify::Status aStatus);

} // namespace spaceg

} // namespace wsfg

#endif // VERIFICATIONRESULTS_HPP
