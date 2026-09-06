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
#ifndef WSFCHAFFRADARSIGNATURE_HPP
#define WSFCHAFFRADARSIGNATURE_HPP

#include "wsf_nx_export.h"

#include "WsfChaffParcel.hpp"
#include "WsfRadarSignature.hpp"

class WSF_NX_EXPORT WsfChaffRadarSignature : public WsfRadarSignature
{
public:
   WsfChaffRadarSignature();

   WsfRadarSignature* Clone() const override;

   float GetSignature(WsfStringId               aStateId,
                      WsfEM_Types::Polarization aPolarization,
                      double                    aFrequency,
                      double                    aTgtToXmtrAz,
                      double                    aTgtToXmtrEl,
                      double                    aTgtToRcvrAz,
                      double                    aTgtToRcvrEl,
                      WsfEM_Xmtr*               aXmtrPtr = nullptr,
                      WsfEM_Rcvr*               aRcvrPtr = nullptr) override;

   std::pair<float, float> GetSignatureLimits(WsfStringId aStateId, WsfEM_Types::Polarization aPolarization) const override;

   bool Initialize(double aSimTime, WsfPlatform* aPlatformPtr) override;

private:
   WsfChaffParcel* mParcelPtr; // Pointer to chaff parcel component of platform
};
#endif
