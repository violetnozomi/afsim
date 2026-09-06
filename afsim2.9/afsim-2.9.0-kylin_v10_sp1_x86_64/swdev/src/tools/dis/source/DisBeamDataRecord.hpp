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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef DisBeamDataRecord_hpp
#define DisBeamDataRecord_hpp

#include "dis_export.h"

#include <iomanip>

#include "DisTypes.hpp"

class GenI;
class GenO;

//! Beam Data Record
/*!
 *   The specification of beam-specific data necessary to describe the scan
 *   volume of an emitter beam.
 */
class DIS_EXPORT DisBeamDataRecord
{
public:
   //! Default Constructor
   DisBeamDataRecord();
   //! Default Destructor
   ~DisBeamDataRecord();

   // Accessors

   //! Beam Azimuth Center (radians)
   /*!
    *  This field, in conjunction with the Azimuth Sweep, Elevation Center,
    *  and Elevation Sweep below, shall specify the beam azimuth and
    *  elevation centers and the corresponding half-angles of the beam
    *  azimuth and elevation sweep required to describe the scan volume
    *  covered by the emitter beam main lobe.  Sweep half-angles describe
    *  half of the full sweep, that is, the angle from the center to either
    *  edge of the beam sweep, including 3 dB half-power beam width.  This
    *  scan volume does not include energy emitted in antenna side lobes.
    *
    *  All angles shall be measured in relation to the emitter coordinate
    *  system.  The azimuth center for 2PI radians (360 deg) scan systems shall
    *  be 0, and the azimuth sweep shall be PI radians (180 deg).  It should be
    *  noted that the scan volume described does not take into account masking
    *  by the entity.  Masking determinations are a part of the regeneration
    *  process and require appropriate database information be processed by
    *  the receiving entity.
    *
    *  \note  If this beam is inactive, values of 0 radians will be reported
    */
   DisFloat32 GetAzimuthCenter() const;
   //! Beam Azimuth Sweep (Half Anggle, radians)
   /*!
    *  See GetAzimuthCenter() above.
    */
   DisFloat32 GetAzimuthSweep() const;
   //! Beam Elevation Center (radians)
   /*!
    *  See GetAzimuthCenter() above.
    */
   DisFloat32 GetElevationCenter() const;
   //! Beam Elevation Sweep (Half Angle, radians)
   /*!
    *  See GetAzimuthCenter() above.
    */
   DisFloat32 GetElevationSweep() const;
   //! Beam Sweep Sync (Percentage Range [0.0, 100.0) )
   /*!
    *  This field is provided to allow a receiver to synchronize its
    *  regeneration scan pattern to that of the emitter.  When employed, this
    *  feild shall specify, in the semi-open range [0.0, 100.0), the
    *  percentage of the pattern scanned from its origin.  Note that this
    *  semi-open range includes the start value but not the end value.
    *
    *  \note  If this beam is inactive, a value of 0.0 will be reported
    */
   DisFloat32 GetSweepSync() const;

   // Mutators

   void SetAzimuthCenter(DisFloat32 aAzimuthCenter);
   void SetAzimuthSweep(DisFloat32 aAzimuthSweep);
   void SetElevationCenter(DisFloat32 aElevationCenter);
   void SetElevationSweep(DisFloat32 aElevationSweep);
   void SetSweepSync(DisFloat32 aSweepSync);

   // Input/Output

   void        Get(GenI& aGenI);
   void        Put(GenO& aGenO) const;
   void        Stream(std::ostream& aStream) const;
   std::string ToString() const;

   // Data Validation
   bool IsValid() const;

private:
   /*!
    * Specifies the center of the center
    */
   DisFloat32 mAzimuthCenter;
   /*!
    * Specifies the sweep
    */
   DisFloat32 mAzimuthSweep;
   /*!
    * Specifies the elevation center
    */
   DisFloat32 mElevationCenter;
   /*!
    * Specifies the elevation sweep
    */
   DisFloat32 mElevationSweep;
   /*!
    * Specifies the sweep sync
    */
   DisFloat32 mSweepSync;
};

// Accessors

inline DisFloat32 DisBeamDataRecord::GetAzimuthCenter() const
{
   return mAzimuthCenter;
}
inline DisFloat32 DisBeamDataRecord::GetAzimuthSweep() const
{
   return mAzimuthSweep;
}
inline DisFloat32 DisBeamDataRecord::GetElevationCenter() const
{
   return mElevationCenter;
}
inline DisFloat32 DisBeamDataRecord::GetElevationSweep() const
{
   return mElevationSweep;
}
inline DisFloat32 DisBeamDataRecord::GetSweepSync() const
{
   return mSweepSync;
}

// Mutators

inline void DisBeamDataRecord::SetAzimuthCenter(DisFloat32 aAzimuthCenter)
{
   mAzimuthCenter = aAzimuthCenter;
}
inline void DisBeamDataRecord::SetAzimuthSweep(DisFloat32 aAzimuthSweep)
{
   mAzimuthSweep = aAzimuthSweep;
}
inline void DisBeamDataRecord::SetElevationCenter(DisFloat32 aElevationCenter)
{
   mElevationCenter = aElevationCenter;
}
inline void DisBeamDataRecord::SetElevationSweep(DisFloat32 aElevationSweep)
{
   mElevationSweep = aElevationSweep;
}
inline void DisBeamDataRecord::SetSweepSync(DisFloat32 aSweepSync)
{
   mSweepSync = aSweepSync;
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, const DisBeamDataRecord& aRecord)
{
   aRecord.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisBeamDataRecord& aRecord)
{
   aRecord.Get(aGenI);
   return aGenI;
}

#endif // DisBeamDataRecord_hpp
