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

#include "WsfPrinceDormand45OrbitalIntegrator.hpp"

constexpr const char* WsfPrinceDormand45OrbitalIntegrator::cTYPE;
constexpr double      WsfPrinceDormand45OrbitalIntegrator::cCVALUES[WsfPrinceDormand45OrbitalIntegrator::cSTEPCOUNT];
constexpr double      WsfPrinceDormand45OrbitalIntegrator::cBVALUES[WsfPrinceDormand45OrbitalIntegrator::cSTEPCOUNT];
constexpr double WsfPrinceDormand45OrbitalIntegrator::cERRORVALUES[WsfPrinceDormand45OrbitalIntegrator::cSTEPCOUNT];
constexpr double WsfPrinceDormand45OrbitalIntegrator::cAVALUES[WsfPrinceDormand45OrbitalIntegrator::cSTEPCOUNT]
                                                              [WsfPrinceDormand45OrbitalIntegrator::cSTEPCOUNT - 1];
