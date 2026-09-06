// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WSFCHAFFPARCELINTERFACE_HPP
#define WSFCHAFFPARCELINTERFACE_HPP

class WsfPlatform;

// An abstract base class that serves as the interface to a chaff parcel for a chaff cloud.
// This interface removes the circular dependency between chaff clouds and parcels.
class WsfChaffParcelInterface
{
public:
   virtual ~WsfChaffParcelInterface() = default;

   virtual WsfPlatform* GetPlatform() const                                  = 0;
   virtual void         GetDropLocationWCS(double aDropLocationWCS[3]) const = 0;
   virtual double       GetRadialSize() const                                = 0;
};

#endif
