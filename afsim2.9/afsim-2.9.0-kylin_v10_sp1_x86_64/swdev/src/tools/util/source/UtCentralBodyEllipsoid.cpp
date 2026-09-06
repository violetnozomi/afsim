// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtCentralBodyEllipsoid.hpp"

#include "UtEllipsoidalCentralBody.hpp"

void ut::CentralBodyEllipsoid::ConvertLLA_ToWCS(double aLat, double aLon, double aAlt, UtVec3d& aWCS) const
{
   EllipsoidalCentralBody::ConvertLLAToECEF(aLat, aLon, aAlt, *this, aWCS.GetData());
}

void ut::CentralBodyEllipsoid::ConvertWCS_ToLLA(const UtVec3d& aWCS, double& aLat, double& aLon, double& aAlt) const
{
   EllipsoidalCentralBody::ConvertECEFToLLA(aWCS.GetData(), *this, aLat, aLon, aAlt);
}

void ut::CentralBodyEllipsoid::ConvertLocalToLLA(const UtVec3d& aRefWCS,
                                                 const double   aTransWCS[3][3],
                                                 const UtVec3d& aLocal,
                                                 double&        aLat,
                                                 double&        aLon,
                                                 double&        aAlt) const
{
   EllipsoidalCentralBody::ConvertLocalToLLA(aRefWCS.GetData(), aTransWCS, aLocal.GetData(), *this, aLat, aLon, aAlt);
}

void ut::CentralBodyEllipsoid::ConvertLLA_ToLocal(const UtVec3d& aRefWCS,
                                                  const double   aTransWCS[3][3],
                                                  double         aLat,
                                                  double         aLon,
                                                  double         aAlt,
                                                  UtVec3d&       aLocal) const
{
   EllipsoidalCentralBody::ConvertLLAToLocal(aRefWCS.GetData(), aTransWCS, aLat, aLon, aAlt, *this, aLocal.GetData());
}
