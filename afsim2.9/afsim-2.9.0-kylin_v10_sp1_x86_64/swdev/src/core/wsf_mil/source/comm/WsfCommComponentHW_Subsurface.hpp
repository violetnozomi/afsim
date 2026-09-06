// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOMMCOMPONENTHWSUBSURFACE_HPP
#define WSFCOMMCOMPONENTHWSUBSURFACE_HPP

#include "wsf_mil_export.h"

#include <memory>

#include "WsfCommComponentHW.hpp"
#include "WsfMilComponentRoles.hpp"

// Forward declarations
class WsfCommResult;
class WsfGeoPoint;

namespace wsf
{
namespace comm
{

//! The hardware component that can be attached to all comm systems.
class WSF_MIL_EXPORT ComponentHW_Subsurface : public ComponentHW
{
public:
   static ComponentHW_Subsurface* Find(const wsf::comm::Comm& aParent);
   static ComponentHW_Subsurface* FindOrCreate(wsf::comm::Comm& aParent);

   ComponentHW_Subsurface();
   ~ComponentHW_Subsurface() override = default;

   //! @name Required interface from WsfComponent.
   //@{
   WsfComponent* CloneComponent() const override;
   WsfStringId   GetComponentName() const override;
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   //@}

   //! @name Interface from WsfCommComponent.
   //@{
   bool ProcessInput(UtInput& aInput) override;

   //@}

   bool CanSendTo(double aSimTime, wsf::comm::Comm* aRcvrPtr, wsf::comm::Result& aResult) override;

protected:
   ComponentHW_Subsurface(const ComponentHW_Subsurface& aSrc);
   ComponentHW_Subsurface& operator=(const ComponentHW_Subsurface& aRhs) = delete;

   unsigned int SetSubmarineRadioValues(double             aSimTime,
                                        WsfGeoPoint&       aLowPoint1,
                                        WsfGeoPoint&       aHighPoint2,
                                        double&            aSlantrange,
                                        double&            aWaterRange,
                                        double&            aAirRange,
                                        double&            aWaterLatDeg,
                                        double&            aWaterLongDeg,
                                        double&            aGrazingAngleRadians,
                                        wsf::comm::Result& aResult);

private:
   // We are putting the mWaterAttenuation attribute on the radio here for our initial
   // implementation in order to simplify the release and to check that this parameter
   // will be truly useful. Other WSF attenuation methods are performed through the
   // WsfEM_Attenuation class models which reside on the WsfEM_Xmtr class.  These models do not
   // assume the air-water boundary needed for this communication model.  Overriding the transmitter
   // type and the associated WsfEM_Attenuation methods and models can be a future development if this
   // feature is needed for other radios.  But if only for this radio, then we see extra work as an
   // unnecessary complication.
   double mWaterAttenuation; // unit is ratio using dB/meter

   // Permits users to exclude comm signals based on angle to water.
   // Angle is from horizon up to line for signal path--similar to a grazing angle.
   // This angle is applied to the airborne side of the path.
   // Later consider modeling to include better understanding of refraction and effects of
   // reflection and absorption effects at watter boundary.
   double mMinimumHorizonAngle;

   // Defaults to false; Set true for modeled VLF.  Later consider VLf behavior as separate class.
   bool mIsVLFcomm;

   // This is an error tolerance used to simplify horizon checks in the initial filtering process.
   // Water transmission will generally be less than a few hundred meters. So we will set sub altitude
   // to zero and won't constrain initial horizon/LOS checks when ranges fail by this small value.
   double mAllowedWaterPathRange;

   // Specifies the maximum depth at which a submerged platform is permitted to exchange communication
   // events. In order to reduce user input issues, the absolute value of the numeric input is used,
   // and the value is converted to be a negative altitude to represent the depth below the surface
   // for the submerged platform.
   double mMaxCommunicationDepth;
};

} // namespace comm
} // namespace wsf

WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::comm::ComponentHW_Subsurface, cWSF_COMPONENT_COMM_HW_SUBSURFACE)

#endif
