// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFFIELDOFVIEW_HPP
#define WSFFIELDOFVIEW_HPP

#include "wsf_export.h"

class UtInput;
#include "UtScriptAccessible.hpp"
class WsfArticulatedPart;
class WsfPlatform;
class WsfEM_Antenna;
class WsfScenario;
#include "WsfStringId.hpp"

//! A class that represents a field-of-view / field-of-regard methods.
//! This class is abstract in that the developer is required to implement
//! the WithinFieldOfView(double aThisToTgtAz, double aThisToTgtEl)
//! and Clone(WsfEM_Antenna& aAntenna) methods.
//! Implementations are provided for rectangular, circular, and polygonal
//! fields of view.  The WsfAntenna class aggregates the field of view.
//! By default a rectangular field of view is assumed, as this was the
//! only option available prior to AFSIM 2.1.0  Legacy methods for
//! GetAzimuthFieldOfView(), etc. are retained, as some sensors may
//! assume the rectangular FOV.
class WSF_EXPORT WsfFieldOfView : public UtScriptAccessible
{
public:
   virtual bool ProcessInput(UtInput& aInput);
   virtual bool Initialize(const WsfArticulatedPart& aArticulatedPart);
   const char*  GetScriptClassName() const override { return "WsfFieldOfView"; }

   //! Make a copy of this object.
   //! @return The copy of "this" object.
   virtual WsfFieldOfView* Clone() const = 0;

   //! @name Field-of-regard methods.
   //@{

   //! Is the target within the field-of-view?
   //!
   //! @param aThisToTgtAz [input] Azimuth of the target with respect to the device (radians).
   //! @param aThisToTgtEl [input] Elevation of the target with respect to the device (radians).
   //! @returns 'true' if within the field of view or 'false' if not.
   virtual bool WithinFieldOfView(double aThisToTgtAz, double aThisToTgtEl) = 0;

   virtual bool WithinFieldOfView(WsfArticulatedPart* aArticulatedPart,
                                  double              aTgtLocWCS[3],
                                  double              aEarthRadiusMultiplier,
                                  double&             aTrueThisToTgtAz,
                                  double&             aTrueThisToTgtEl,
                                  double              aApparentThisToTgtLocWCS[3],
                                  double&             aApparentThisToTgtAz,
                                  double&             aApparentThisToTgtEl,
                                  double              aApparentTgtToThisLocWCS[3]);

   virtual bool WithinFieldOfView(WsfEM_Antenna* aAntennaPtr,
                                  double         aTgtLocWCS[3],
                                  double         aEarthRadiusMultiplier,
                                  double&        aTrueThisToTgtAz,
                                  double&        aTrueThisToTgtEl,
                                  double         aApparentThisToTgtLocWCS[3],
                                  double&        aApparentThisToTgtAz,
                                  double&        aApparentThisToTgtEl,
                                  double         aApparentTgtToThisLocWCS[3]);
   //@}

   //! Return the azimuth field of view limits, if applicable.
   //! @param aMinAzFOV The minimum azimuth field-of-view limit.
   //! @param aMaxAzFOV The maximum azimuth field-of-view limit.
   //! @return True if the limits apply to the field-of-view type;
   //!   false otherwise.
   virtual bool GetAzimuthFieldOfView(double& aMinAzFOV, double& aMaxAzFOV) const { return false; }

   //! Return the elevation field of view limits, if applicable.
   //! @param aMinElFOV The minimum elevation field-of-view limit.
   //! @param aMaxElFOV The maximum elevation field-of-view limit.
   //! @return True if the limits apply to the field-of-view type;
   //!   false otherwise.
   virtual bool GetElevationFieldOfView(double& aMinElFOV, double& aMaxElFOV) const { return false; }

   //! Set the azimuth field of view limits, if applicable.
   //! @param aMinAzFOV The minimum azimuth field-of-view limit.
   //! @param aMaxAzFOV The maximum azimuth field-of-view limit.
   //! @return True if the limits apply to the field-of-view type;
   //!   false otherwise.
   virtual bool SetAzimuthFieldOfView(double aMinAzFOV, double aMaxAzFOV) { return false; }

   //! Set the elevation field of view limits, if applicable.
   //! @param aMinElFOV The minimum elevation field-of-view limit.
   //! @param aMaxElFOV The maximum elevation field-of-view limit.
   //! @return True if the limits apply to the field-of-view type;
   //!   false otherwise.
   virtual bool SetElevationFieldOfView(double aMinElFOV, double aMaxElFOV) { return false; }

   //! Return the type of field of view (i.e., "rectangular",
   //! "circular", "polygonal", etc.)
   //! @return A string corresponding with the field-of-view type.
   const WsfStringId& GetType() const { return mType; }

protected:
   //! Set the type corresponding with the type of derived class.
   void SetType(const WsfStringId& aType) { mType = aType; }

   const WsfArticulatedPart* GetArticulatedPart() const { return mArticulatedPartPtr; }

private:
   WsfStringId               mType;
   const WsfArticulatedPart* mArticulatedPartPtr = nullptr;
};

#endif
