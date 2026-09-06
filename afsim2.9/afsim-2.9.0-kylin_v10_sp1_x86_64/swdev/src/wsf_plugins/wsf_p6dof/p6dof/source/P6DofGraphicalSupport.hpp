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

#ifndef P6DOFGRAPHICALSUPPORT_H
#define P6DOFGRAPHICALSUPPORT_H

#include "p6dof_export.h"

#include "P6DofAtmosphere.hpp"
#include "P6DofKinematicState.hpp"
#include "P6DofWind.hpp"

// P6DofGraphicalSupport provides support to define the current graphics
// representation for a P6DofVehicle in graphical support tools.
class P6DOF_EXPORT P6DofGraphicalSupport
{
public:
   P6DofGraphicalSupport();

   P6DofGraphicalSupport* Clone() const;

   unsigned short GetCurrentObjectGraphicalType() const { return mCurrentObjectGraphicalType; }

   void SetCurrentObjectGraphicalType(int aAltType);

   unsigned short GetObjectGraphicalType() const { return mObjectGraphicalType; }
   unsigned short GetObjectGraphicalAltType1() const { return mObjectGraphicalAltType1; }
   unsigned short GetObjectGraphicalAltType2() const { return mObjectGraphicalAltType2; }
   unsigned short GetObjectGraphicalAltType3() const { return mObjectGraphicalAltType3; }

   void SetObjectGraphicalType(int aTypeId);
   void SetObjectGraphicalAltType1(int aTypeId);
   void SetObjectGraphicalAltType2(int aTypeId);
   void SetObjectGraphicalAltType3(int aTypeId);

protected:
   // Graphical support
   unsigned short mObjectGraphicalType;
   unsigned short mObjectGraphicalAltType1;
   unsigned short mObjectGraphicalAltType2;
   unsigned short mObjectGraphicalAltType3;
   unsigned short mCurrentObjectGraphicalType;
};

#endif
