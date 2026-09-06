// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFEM_ATTENUATION_HPP
#define WSFEM_ATTENUATION_HPP

#include "wsf_export.h"

class UtInput;
#include "WsfEM_Interaction.hpp"
class WsfEM_Xmtr;
class WsfEM_XmtrRcvr;
class WsfEnvironment;
#include "WsfObject.hpp"
class WsfScenario;

//! Base Class definition for the Atmospheric Attenuation (Absorption) Model.
//! Returns a factor that indicates no attenuation by default.
class WSF_EXPORT WsfEM_Attenuation : public WsfObject
{
public:
   WsfEM_Attenuation();
   ~WsfEM_Attenuation() override;

   WsfEM_Attenuation* Clone() const override = 0;

   virtual bool Initialize(WsfEM_XmtrRcvr* aXmtrRcvrPtr);

   virtual bool Initialize(WsfEM_Xmtr* aXmtrPtr);

   bool ProcessInput(UtInput& aInput) override;

   const char* GetScriptClassName() const override { return "WsfEM_Attenuation"; }

   //! Is 'debug' enabled?
   bool DebugEnabled() const { return mDebugEnabled; }

   //! Is the object a 'null' (no-effect) clutter model?
   //! No operational model needs to worry about this method.
   //! @note See the code in WsfEM_Attenuation as to why this is present.
   virtual bool IsNullModel() const { return false; }

   //! Does the accept inline block input?
   //!
   //! This is called by WsfEM_Attenuation::LoadReference when attempting to load a
   //! reference to an attenuation model from within another object (i.e.: a 'transmitter').
   //!
   //! When originally developed none of the models required additional input, and the
   //! command stream was simply 'attenuation_model \<x\>'. Some of the newer models however
   //! have optional inputs that may be provided and thus require the 'block' input
   //! (i.e.: they require a 'end_attenuation_model' terminator. Models that have optional
   //! inputs should return 'true'.
   virtual bool AcceptsInlineBlockInput() const { return false; }

   virtual double ComputeAttenuationFactor(WsfEM_Interaction&          aInteraction,
                                           WsfEnvironment&             aEnvironment,
                                           WsfEM_Interaction::Geometry aGeometry);

protected:
   WsfEM_Attenuation(const WsfEM_Attenuation& aSrc);

   virtual double ComputeAttenuationFactorP(double aRange, double aElevation, double aAltitude, double aFrequency);

   void GetAltitudesAndGroundRange(WsfEM_Interaction&          aInteraction,
                                   WsfEM_Interaction::Geometry aGeometry,
                                   double&                     aAltitude1,
                                   double&                     aAltitude2,
                                   double&                     aGroundRange);

   void GetRangeElevationAltitude(WsfEM_Interaction&          aInteraction,
                                  WsfEM_Interaction::Geometry aGeometry,
                                  double&                     aRange,
                                  double&                     aElevation,
                                  double&                     aAltitude);

   bool mDebugEnabled;

   //! True if the end points of the path should be sorted so the path goes from the lowest to highest point.
   //! The default in the base class is 'true' and provides no means for the user to change it.
   //! Derived classes may change the default and may provide means for the user to change it.
   bool mSortEndPoints;

private:
   //! Assignment operator declared but not defined to prevent use.
   WsfEM_Attenuation& operator=(const WsfEM_Attenuation& aRhs) = delete;

   double GetGroundRange(const double aLocWCS_1[3], const double aLocWCS_2[3]) const;

   void GetRangeElevationAltitude(WsfPlatform*                           aSrcPlatformPtr,
                                  WsfPlatform*                           aTgtPlatformPtr,
                                  const WsfEM_Interaction::LocationData& aSrcLoc,
                                  const WsfEM_Interaction::LocationData& aTgtLoc,
                                  const WsfEM_Interaction::RelativeData& aSrcToTgt,
                                  const WsfEM_Interaction::RelativeData& aTgtToSrc,
                                  double&                                aRange,
                                  double&                                aElevation,
                                  double&                                aAltitude);

   static WsfEM_Attenuation* CreateInstance(const std::string& aTypeName);
};

#endif
