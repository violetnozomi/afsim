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

#ifndef DIS_DIRECTED_ENERGY_AIMPOINT_HPP
#define DIS_DIRECTED_ENERGY_AIMPOINT_HPP

#include "dis_export.h"

class DisPrecisionBeamAimpoint;
#include "DisTypes.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

class DIS_EXPORT DisDirectedEnergyAimpoint
{
public:
   DisDirectedEnergyAimpoint();
   virtual ~DisDirectedEnergyAimpoint();

   // Accessors
   DisEnum32                       GetAimpointType() const { return mAimpointType; }
   const DisPrecisionBeamAimpoint* GetPrecisionBeamAimpoint() const { return mPrecisionBeamAimpointPtr; }

   // Input/Output
   void      Get(GenI& aGenI);
   void      Put(GenO& aGenO);
   DisUint16 GetLength();

   // Mutators
   void SetPrecisionAimpoint(DisPrecisionBeamAimpoint* aPrecisionBeamAimpointPtr);

protected:
private:
   DisEnum32 mAimpointType;
   union
   {
      DisPrecisionBeamAimpoint* mPrecisionBeamAimpointPtr;
      // DisDE_AreaAimpoint*  mDE_AreaAimpointPtr;  // (todo)
   };
};

#endif
