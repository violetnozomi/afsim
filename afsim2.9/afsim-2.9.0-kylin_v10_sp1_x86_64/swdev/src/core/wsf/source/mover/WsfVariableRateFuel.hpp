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

#ifndef WSFVARIABLERATEFUEL_HPP
#define WSFVARIABLERATEFUEL_HPP

#include "wsf_export.h"

#include <map>

#include "TblLookup.hpp"
#include "WsfFuel.hpp"

//!   Fuel consumption rate model.
//!
//!   This class defines fuel consumption behavior, at either constant rates, or at variable rates
//!   based upon one or two independent variables (the choices are platform altitude and speed).
//!   By default, one fuel table is accepted, independent of fuel mode.  However, multiple tables
//!   may be defined for different fuel modes, if supplied, and the active table will be switched
//!   by setting the fuel mode.  (This model differs from the similar WsfTabularRateFuel in that
//!   the table input format is more flexible, and need not be rectangular.)
class WSF_EXPORT WsfVariableRateFuel : public WsfFuel
{
public:
   WsfVariableRateFuel(WsfScenario& aScenario);
   WsfVariableRateFuel& operator=(const WsfVariableRateFuel&) = delete;
   ~WsfVariableRateFuel() override;

   WsfFuel* Clone() const override;

   //! @name Common infrastructure methods.
   //@{
   bool Initialize(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;
   //@}

   void SetMode(WsfStringId aMode) override;

   //! Enumeration to select the independent variable for a fuel flow table.
   enum IndependentVariable
   {
      cSPEED,    //!< Function of speed.
      cALTITUDE, //!< A function of one independent variable.
      cWEIGHT    //!< A function of two independent variables.
   };

   //! Enumeration to select the fuel consumption rate table lookup type.
   enum TableType
   {
      cCONSTANT, //!< Constant (No table lookup required).
      cONE_D,    //!< A function of one independent variable.
      cTWO_D,    //!< A function of two independent variables.
      cTHREE_D   //!< A function of three independent variables.
   };

   struct AltitudeEntry
   {
      double              mAltitude; //<! Altitude independent value
      TblIndVarU<double>  mSpeeds;   //<! Speed independent values
      TblDepVar1<double>  mRates;    //<! the rate table f(altitude, speed)
      TblLookupLU<double> mSpeedLookup;
   };

   //! Fuel rate table
   typedef std::vector<AltitudeEntry> Altitudes;

   class Table
   {
   public:
      Table()
         : mAltitudes()
         , mConstantRateUsed(false)
         , mRateTableUsed(false)
      {
      }

      Altitudes mAltitudes;
      bool      mConstantRateUsed; //! True if only a single rate is provided
      bool      mRateTableUsed;    //! True if rate table is provided
   };

protected:
   typedef std::map<WsfStringId, Table>  RateTables;
   typedef RateTables::iterator          RateTableIter;
   typedef RateTables::const_iterator    RateTableConstIter;
   typedef std::pair<WsfStringId, Table> RateTablePair;

   bool ProcessTablePairInput(UtInput& aInput, RateTablePair& aPair);

   //! Copy constructor (for Clone()).
   WsfVariableRateFuel(const WsfVariableRateFuel& aSrc);

   //! Returns the consumption rate
   double CalcConsumptionRate() override;
   double CalcConsumptionRateAltitudeSpeed(const double aAltitude, const double aSpeed) override;

private:
   Table* GetCurrentTable();

   //! Convenience method for creating a fuel table when only a rate is provided.
   void CreateConstantTable(double aValue, Table& aTable);

   //! Add an entry to the fuel table.
   void AddAltitudeEntry(double aAltitude, std::vector<double>& aSpeeds, std::vector<double>& aRates, Table& aTable);

   RateTables  mTables;
   WsfStringId mInputMode;
};

#endif
