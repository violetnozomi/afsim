// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFLASER_XMTRCOMPONENT_HPP
#define WSFLASER_XMTRCOMPONENT_HPP

#include "WsfEM_Xmtr.hpp"
#include "WsfMilComponentRoles.hpp"

//! A LASER transmitter component.
//! This class simply serves to provide LASER, or narrowband optical,
//! configuration data to be used with WsfEM_Xmtr.
class WsfLASER_XmtrComponent : public WsfEM_XmtrComponent
{
public:
   WsfLASER_XmtrComponent();
   WsfLASER_XmtrComponent(const WsfLASER_XmtrComponent& aSrc) = default;
   WsfLASER_XmtrComponent& operator=(const WsfLASER_XmtrComponent&) = delete;

   //! @name WsfComponent required methods.
   //@{
   WsfStringId   GetComponentName() const override;
   WsfComponent* CloneComponent() const override;
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   //@}

   //! @name Framework methods.
   //@{
   void ComponentParentChanged(WsfEM_Xmtr* aXmtr) override;
   bool Initialize(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;
   //@}

   //! Return the receiver's aperture diameter.
   double GetApertureDiameter() const { return mApertureDiameter; }

   double GetApertureArea() const;

   //! Return the transmit "antenna gain" (see reference guide),
   //! not including losses for optics transmission and wavefront error.
   double GetTransmitAntennaGain() const { return mTransmitAntennaGain; }

   //! Return the "antenna gain" associated with this transmitter (see reference guide),
   //! including losses for optics transmission and wavefront error.
   double GetAntennaGain() const { return mAntennaGain; }

   //! Return the angular width of the transmitted beam.
   double GetBeamwidth() const { return mBeamwidth; }

   //! Return the fraction of the transmitted laser light that passes through the transmitter optics.
   double GetOpticsTransmissionFactor() const { return mOpticsTransmissionFactor; }

   //! Return the fraction of laser light that passes through a the transmitter optical path as a function of wavefront error.
   double GetWavefrontTransmissionFactor() const { return mWavefrontTransmissionFactor; }

private:
   double mApertureDiameter;
   double mOpticsTransmissionFactor;
   double mBeamwidth;
   double mWavefrontError;
   double mWavefrontTransmissionFactor;
   double mTransmitAntennaGain;
   double mAntennaGain;
   double mObscurationTruncationFactor;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfLASER_XmtrComponent, cWSF_COMPONENT_LASER_XMTR)

#endif
