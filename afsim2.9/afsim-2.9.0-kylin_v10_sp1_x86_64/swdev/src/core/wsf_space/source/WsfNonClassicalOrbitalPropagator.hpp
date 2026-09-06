// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFNONCLASSICALORBITALPROPAGATOR_HPP
#define WSFNONCLASSICALORBITALPROPAGATOR_HPP

#include "wsf_space_export.h"

#include "UtGoldenSectionSearch.hpp"
#include "UtMatrix.hpp"
#include "UtOrbitalState.hpp"
#include "WsfOrbitalPropagator.hpp"

class WSF_SPACE_EXPORT WsfNonClassicalOrbitalPropagator : public wsf::space::OrbitalPropagator
{
public:
   explicit WsfNonClassicalOrbitalPropagator(std::unique_ptr<ut::OrbitalState> aInitialOrbitalStatePtr);
   WsfNonClassicalOrbitalPropagator(const WsfNonClassicalOrbitalPropagator& aOther) = default;
   ~WsfNonClassicalOrbitalPropagator() override                                     = default;

   WsfNonClassicalOrbitalPropagator& operator=(const WsfNonClassicalOrbitalPropagator& aOther) = delete;

   std::unique_ptr<UtOrbitalPropagatorBase> GetManeuveringPropagator() const override;

   UtLambertProblem::Result SolveLambertProblem(const ut::CentralBody& aCentralBody,
                                                const UtVec3d&         aLocationECI_1,
                                                const UtVec3d&         aLocationECI_2,
                                                double                 aDt,
                                                bool                   aShortWay,
                                                bool                   aAllowHitsEarth,
                                                double                 aConvergenceTolerance = 1.0e-12) const override;

private:
   class StateVector
   {
   public:
      explicit StateVector(const UtVec3d& aPosition);
      explicit StateVector(const UtMatrixd& aMatrix);

      StateVector operator-(const StateVector& aOther) const;
      StateVector operator*(double aScalar) const;

      const UtMatrixd& Vector() const { return mVec; }

      double Component(int aIndex) const;

      static const int cDIMENSION = 3;

   private:
      UtMatrixd mVec;
   };

   class SearchVector
   {
   public:
      explicit SearchVector(const UtVec3d& aVelocity);
      explicit SearchVector(const UtMatrixd& aMatrix);
      explicit SearchVector(double aFillValue);

      SearchVector& operator+=(const SearchVector& aOther);
      SearchVector& operator-=(const SearchVector& aOther);
      SearchVector  operator+(const SearchVector& aOther) const;
      SearchVector  operator*(double aScalar) const;

      UtVec3d Velocity() const;

      double Component(int aIndex) const;

      static SearchVector UnitVector(int aIndex, double aValue = 1.0);

      static const int cDIMENSION = 3;

   private:
      UtMatrixd mVec;
   };

   class Jacobian
   {
   public:
      Jacobian();

      void         SetColumn(int aIndex, const StateVector& aColumn);
      SearchVector InverseProduct(const StateVector& aState) const;

      static const int cDIMENSION = 3;

   private:
      UtMatrixd mMat;
   };
};

#endif
