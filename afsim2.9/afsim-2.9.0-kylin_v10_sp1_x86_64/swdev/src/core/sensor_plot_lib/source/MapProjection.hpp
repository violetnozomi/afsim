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

#ifndef MapProjection_hpp
#define MapProjection_hpp

class MapProjection
{
public:
   MapProjection();

   virtual ~MapProjection();

   virtual bool ConvertLatLonToXY(double aLat, double aLon, double& aX, double& aY) const = 0;

   virtual bool ConvertXYToLatLon(double aX, double aY, double& aLat, double& aLon) const = 0;

   virtual void SetCenter(double aLatDeg, double aLonDeg) = 0;

private:
};

#endif
