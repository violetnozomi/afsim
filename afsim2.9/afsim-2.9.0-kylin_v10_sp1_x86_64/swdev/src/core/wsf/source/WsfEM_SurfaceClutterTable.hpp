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

#ifndef WSFEM_SURFACECLUTTERTABLE_HPP
#define WSFEM_SURFACECLUTTERTABLE_HPP

#include "wsf_export.h"

#include <memory>
#include <vector>

#include "TblLookup.hpp"
#include "UtEntity.hpp"
class UtInput;
#include "WsfEM_Clutter.hpp"
class WsfEM_Interaction;
class WsfEM_Environment;

//! This class defines a clutter table as a function of altitude and range
//!
//! Clutter tables are used by receivers of electromagnetic radiation to determine
//! how much noise is added to the received signal due to ground (or sea) clutter.
//!
//! A given clutter table object will be shared amongst all objects that utilize the table.
class WSF_EXPORT WsfEM_SurfaceClutterTable : public WsfEM_Clutter
{
public:
   enum Type
   {
      cUNDEFINED = 0, //<! The table type is not defined.
      cCONSTANT,      //<! A "dummy" table filled with a predefined constant value.
      cGENERIC,       //<! A range-elevation-clutter table with that is independent of bearing angle.
      cSITE_SPECIFIC  //<! A table that is dependent on range, bearing, and elevation.
   };

   WsfEM_SurfaceClutterTable();
   ~WsfEM_SurfaceClutterTable() override;

   static WsfEM_Clutter* ObjectFactory(const std::string& aTypeName);

   WsfEM_Clutter* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;

   //! Return the table type.
   Type GetType() const { return mType; }

   double ComputeClutterPower(WsfEM_Interaction& aInteraction, WsfEnvironment& aEnvironment, double aProcessingFactor) override;

protected:
   WsfEM_SurfaceClutterTable(const WsfEM_SurfaceClutterTable& aSrc);
   WsfEM_SurfaceClutterTable& operator=(const WsfEM_SurfaceClutterTable& aRhs);

private:
   //! Convenience method for creating a clutter table when only a single value is provided.
   void CreateConstantTable(double value);

   //! Add an entry to the clutter table.
   void AddAltitudeEntry(double               aAltitude,
                         std::vector<double>& aRanges,
                         std::vector<double>& aBearings,
                         std::vector<double>& aClutters);

   //! Add an entry to the clutter table.
   void AddAltitudeEntry(double aAltitude, std::vector<double>& aRanges, std::vector<double>& aClutters);

   bool Load(UtInput& aInput);

   struct AltitudeEntry
   {
      double             mAltitude; //<! Altitude independent value
      TblIndVarU<double> mRanges;   //<! Range independent values
      TblIndVarU<double> mBearings; //<! The set of bearings for which the clutter is measured
      //<! (not used if table type is GENERIC).
      TblDepVar1<double>  mClutters1; //<! the clutter table f(altitude, range)
      TblDepVar2<double>  mClutters2; //<! the clutter table f(altitude, range, bearing)
      TblLookupLU<double> mRangeLookup;
      TblLookupLU<double> mBearingLookup;
   };

   //! clutter table
   class ClutterTable : public std::vector<AltitudeEntry*>
   {
   public:
      virtual ~ClutterTable();
   };

   std::shared_ptr<ClutterTable> mAltitudesPtr;

   ClutterTable& mAltitudes;

   Type   mType;
   double mConstantClutter;

   // The following are used to compute accurate ground ranges between the target and receiver.
   UtEntity mReceiver;
   UtEntity mTarget;
};

#endif
