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
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#ifndef WSFTABULARRATEFUEL_HPP
#define WSFTABULARRATEFUEL_HPP

#include "wsf_export.h"

#include <map>

#include "UtInput.hpp"
class UtInputBlock;
#include "WsfFuel.hpp"
class WsfFuelFlow;
#include "UtAtmosphere.hpp"

//!   Fuel consumption rate model.
//!
//!   This class defines fuel consumption behavior, at either constant rates, or at
//!   variable rates based upon one to three independent variable (the choices are
//!   platform altitude, weight, and speed).  By default, if fuel Mode is ignored,
//!   only one table is accepted, however, multiple tables may be defined for different
//!   Modes if desired.  The active table is switched by setting the fuel Mode, unless
//!   there is no table for that Mode, in which case the active table will not be
//!   changed.  (This model differs from the similar WsfVariableRateFuel in that the
//!   tables must be rectangular, and this class allows for 3-D tables versus only
//!   2-D for WsfVariableRateFuel.)
class WSF_EXPORT WsfTabularRateFuel : public WsfFuel
{
public:
   //! Enumeration to identify the variable for a fuel flow table.
   enum VarType
   {
      cNO_VARTYPE = 0, //!< Not yet set.
      cSPEED,          //!< Function of speed.
      cALTITUDE,       //!< Function of altitude.
      cMASS,           //!< Function of mass.
      cRATES           //!< Fuel Flow Rate (dependent variable)
   };

   //! Enumeration to select the fuel consumption rate table lookup type.
   enum TableType
   {
      cCONSTANT, //!< Constant (No table lookup required).
      cONE_D,    //!< A function of one independent variable.
      cTWO_D,    //!< A function of two independent variables.
      cTHREE_D   //!< A function of three independent variables.
   };

   struct Array
   {
      Array()
         : Variable(cNO_VARTYPE)
         , ConvFactor(1.0)
         , Values()
      {
      }

      VarType             Variable;
      double              ConvFactor;
      std::vector<double> Values;
      void                Check(UtInput& aInput);
   };

   struct InputData
   {
      InputData()
         : Mode()
         , Table(cCONSTANT)
         , FirstIV()
         , SecondIV()
         , ThirdIV()
         , Data()
      {
      }

      // This structure is used to capture fuel flow table input
      // before the full type is known (constant, 1D, 2D, 3D table).
      WsfStringId Mode;
      TableType   Table;
      Array       FirstIV;
      Array       SecondIV;
      Array       ThirdIV;
      Array       Data;
   };

   WsfTabularRateFuel(WsfScenario& aScenario);
   WsfTabularRateFuel& operator=(const WsfTabularRateFuel&) = delete;
   ~WsfTabularRateFuel() override;

   WsfFuel* Clone() const override;

   //! @name Common infrastructure methods.
   //@{
   bool Initialize(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;
   //@}

   void SetMode(WsfStringId aMode) override;

   using RateTableMap = std::map<WsfStringId, WsfFuelFlow*>;
   //! @name External Services access.
   //! These methods support the External Services and are not part of the WSF public interface.
   //@{
   virtual const RateTableMap& GetRateTableMap() { return mTables; }
   //@}

protected:
   void ProcessTableBlock(UtInputBlock& aInputBlock);
   void ProcessVarBlock(UtInputBlock& aInputBlock, UtInput::ValueType aType, Array& aArray);
   void ProcessMachBlock(UtInputBlock& aInputBlock, Array& aArray);
   using RateTablePair = std::pair<WsfStringId, WsfFuelFlow*>;
   // typedef std::pair<int, WsfFuelFlow*> RateTablePair;
   using RateTableConstIter = RateTableMap::const_iterator;
   using RateTableIter      = RateTableMap::iterator;

   //! Copy constructor (for Clone()).
   WsfTabularRateFuel(const WsfTabularRateFuel& aSrc);

   //! Calculates the consumption rate
   double CalcConsumptionRate() override;
   double CalcConsumptionRateAltitudeSpeed(const double aAltitude, const double aSpeed) override;

   WsfFuelFlow* GetCurrentTable();

private:
   bool         mSpeedInMach;
   RateTableMap mTables;
   UtAtmosphere mAtmosphere;
};

#endif
