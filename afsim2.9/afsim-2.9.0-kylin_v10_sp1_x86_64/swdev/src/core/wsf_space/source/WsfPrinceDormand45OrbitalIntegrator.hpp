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

#ifndef WSFPRINCEDORMAND45ORBITALINTEGRATOR_HPP
#define WSFPRINCEDORMAND45ORBITALINTEGRATOR_HPP

#include "WsfRungeKuttaOrbitalIntegrator.hpp"

/*

This integrator implements the RK5(4)7S scheme described in

A family of embedded Runge-Kutta formulae, J.R. Dormand and P.J. Prince, Journal of Computational
and Applied Mathematics, Vol. 6, no 1, 1980.

*/

class WsfPrinceDormand45OrbitalIntegrator : public WsfRungeKuttaOrbitalIntegrator<5, 7, WsfPrinceDormand45OrbitalIntegrator>
{
public:
   WsfPrinceDormand45OrbitalIntegrator()                                                  = default;
   WsfPrinceDormand45OrbitalIntegrator(const WsfPrinceDormand45OrbitalIntegrator& aOther) = default;
   ~WsfPrinceDormand45OrbitalIntegrator() override                                        = default;

   WsfPrinceDormand45OrbitalIntegrator* Clone() const override
   {
      return new WsfPrinceDormand45OrbitalIntegrator{*this};
   }

   const char* GetScriptClassName() const override { return "WsfPrinceDormand45Integrator"; }

   std::string GetIntegratorType() const override { return cTYPE; }

   static constexpr const char* cTYPE = "prince_dormand_45";

   static constexpr double cCVALUES[cSTEPCOUNT] = {0.0, 2.0 / 9.0, 1.0 / 3.0, 5.0 / 9.0, 2.0 / 3.0, 1.0, 1.0};

   static constexpr double cBVALUES[cSTEPCOUNT] = {19.0 / 200.0, 0.0, 3.0 / 5.0, -243.0 / 400.0, 33.0 / 40.0, 7.0 / 80.0, 0.0};

   static constexpr double cERRORVALUES[cSTEPCOUNT] = {cBVALUES[0] - 431.0 / 5000.0,
                                                       cBVALUES[1] - 0.0,
                                                       cBVALUES[2] - 333.0 / 500.0,
                                                       cBVALUES[3] + 7857.0 / 10000.0,
                                                       cBVALUES[4] - 957.0 / 1000.0,
                                                       cBVALUES[5] - 193.0 / 2000.0,
                                                       cBVALUES[6] + 1.0 / 50.0};

   static constexpr double cAVALUES[cSTEPCOUNT][cSTEPCOUNT - 1] = {
      {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
      {2.0 / 9.0, 0.0, 0.0, 0.0, 0.0, 0.0},
      {1.0 / 12.0, 1.0 / 4.0, 0.0, 0.0, 0.0, 0.0},
      {55.0 / 324.0, -25.0 / 108.0, 50.0 / 81.0, 0.0, 0.0, 0.0},
      {83.0 / 330.0, -13.0 / 22.0, 61.0 / 66.0, 9.0 / 110.0, 0.0, 0.0},
      {-19.0 / 28.0, 9.0 / 4.0, 1.0 / 7.0, -27.0 / 7.0, 22.0 / 7.0, 0.0},
      {19.0 / 200.0, 0.0, 3.0 / 5.0, -243.0 / 400.0, 33.0 / 40.0, 7.0 / 80.0}};
};

#endif // WSFPRINCEDORMAND45ORBITALINTEGRATOR_HPP
