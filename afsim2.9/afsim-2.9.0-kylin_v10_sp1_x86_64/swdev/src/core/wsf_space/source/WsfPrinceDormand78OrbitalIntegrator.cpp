// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfPrinceDormand78OrbitalIntegrator.hpp"

constexpr const char* WsfPrinceDormand78OrbitalIntegrator::cTYPE;
constexpr double      WsfPrinceDormand78OrbitalIntegrator::cCVALUES[WsfPrinceDormand78OrbitalIntegrator::cSTEPCOUNT];
constexpr double      WsfPrinceDormand78OrbitalIntegrator::cBVALUES[WsfPrinceDormand78OrbitalIntegrator::cSTEPCOUNT];
constexpr double WsfPrinceDormand78OrbitalIntegrator::cERRORVALUES[WsfPrinceDormand78OrbitalIntegrator::cSTEPCOUNT];
constexpr double WsfPrinceDormand78OrbitalIntegrator::cAVALUES[WsfPrinceDormand78OrbitalIntegrator::cSTEPCOUNT]
                                                              [WsfPrinceDormand78OrbitalIntegrator::cSTEPCOUNT - 1];
