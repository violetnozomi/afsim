// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#if !defined(UTOMAPPROJECTION_HPP)
#define UTOMAPPROJECTION_HPP

#include "utilosg_export.h"

#include "UtoTypes.hpp"

class UtEntity;

class UTILOSG_EXPORT UtoMapProjection
{
public:
   enum Projection
   {
      Geocentric      = 0,
      Equirectangular = 1
   };

   UtoMapProjection();

   void ConvertFromLLA(const double aLLA[3], double aXYZ[3]) const;
   void ConvertFromLLA(const double aLat, const double aLon, const double aAlt, double aXYZ[3]) const;
   void ConvertFromECEF(const double aXYZin[3], double aXYZout[3]) const;
   void ConvertFromECEF(double aXYZinout[3]) const;
   void ConvertToLLA(const double aXYZ[3], double aLLA[3]) const;

   void        Convert(UtEntity& aEntity, double aXYZ[3]) const;
   void        ConvertNoOffset(UtEntity& aEntity, double aXYZ[3]) const;
   void        Convert(UtEntity& aEntity, double aXYZ[3], double aRPY[3]) const;
   void        ConvertNoOffset(UtEntity& aEntity, double aXYZ[3], double aRPY[3]) const;
   void        SetOffset(UtoPosition Offset) { m_Offset = Offset; }
   UtoPosition Offset() const { return m_Offset; }
   bool        operator==(const UtoMapProjection& aRHS) const;
   bool        operator!=(const UtoMapProjection& aRHS) const;

   Projection          projection;
   double              longitudeOffset;
   UtoPosition         m_Offset;
   const static double msEqrFactor;
};

#endif
