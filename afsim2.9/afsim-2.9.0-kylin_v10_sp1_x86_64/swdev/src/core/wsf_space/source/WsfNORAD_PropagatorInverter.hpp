// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFNORADPROPAGATORINVERTER_HPP
#define WSFNORADPROPAGATORINVERTER_HPP

#include "wsf_space_export.h"

#include <memory>

#include "UtCalendar.hpp"
#include "UtECI_Conversion.hpp"
#include "UtMatrix.hpp"
#include "UtNewtonRaphson.hpp"
#include "UtOrbitalState.hpp"
#include "UtTwoLineElement.hpp"
#include "UtVec3.hpp"

class WsfNORAD_OrbitalPropagator;

//! A class that will solve the inverse problem of the WsfNORAD_OrbitalPropagator.
class WSF_SPACE_EXPORT WsfNORAD_PropagatorInverter
{
public:
   //! Construct an inverter for the given target state.
   //! After construction, this object will be able to provide the TLE that can be used
   //! in the NORAD propagator to reach the given @p aPosition and @p aVelocity at the
   //! given @p aEpoch.
   //! \param aOsculatingInitialState - The initial osculating orbital state.
   //! \param aPropPtr     - The propagator to use to predict forward in time.
   WsfNORAD_PropagatorInverter(const ut::OrbitalState&                     aOsculatingInitialState,
                               std::unique_ptr<WsfNORAD_OrbitalPropagator> aPropPtr);

   //! Returns if a solution was found during construction.
   bool SolutionFound() const { return mSolutionFound; }

   //! Return the solution orbital state.
   ut::OrbitalState GetOrbitalState() const;

   //! Return the solution Two-line element
   UtTwoLineElement GetTwoLineElement() const;

private:
   //! A thin adaptor of a column matrix to represent states (position and velocity).
   class StateVector
   {
   public:
      StateVector(const ut::OrbitalState::Vector& aVector);
      explicit StateVector(const UtMatrixd& aMatrix);
      StateVector() = default;

      UtVec3d Position() const;
      UtVec3d Velocity() const;

      StateVector operator-(const StateVector& aOther) const;
      StateVector operator*(double aScalar) const;

      double Component(int aIndex) const { return mVec(aIndex); }

      const UtMatrixd& Vector() const { return mVec; }

      static const int cDIMENSION = 6;

   private:
      UtMatrixd mVec;
   };

   //! A thin adaptor of a column matrix to represent the search space (orbital elements).
   class SearchVector
   {
   public:
      explicit SearchVector(const UtTwoLineElement& aElements);
      explicit SearchVector(const UtMatrixd& aMatrix);
      explicit SearchVector(double aFillValue);

      UtTwoLineElement GetTLE(const UtCalendar& aEpoch, const UtTwoLineElement& aReferenceTLE) const;

      SearchVector& operator+=(const SearchVector& aOther);
      SearchVector& operator-=(const SearchVector& aOther);
      SearchVector  operator+(const SearchVector& aOther) const;
      SearchVector  operator*(double aScalar) const;

      double Component(int aIndex) const;

      static SearchVector UnitVector(int aIndex, double aValue = 1.0);

      static const int cDIMENSION = 6;

   private:
      UtMatrixd mVec;
   };

   //! A thin adaptor of a square matrix.
   // TODO finish the description here about the requirements of Q and X
   template<typename Q, typename X>
   class Jacobian
   {
   public:
      static_assert(Q::cDIMENSION == X::cDIMENSION, "Search and State space must have the same dimension");

      Jacobian()
         : mMat(Q::cDIMENSION, Q::cDIMENSION)
      {
      }

      void SetColumn(int aIndex, const X& aStateColumn)
      {
         for (int i = 0; i < cDIMENSION; ++i)
         {
            mMat(i, aIndex) = aStateColumn.Component(i);
         }
      }

      Q InverseProduct(const X& aState) const
      {
         UtMatrixd Jinv;
         Jinv.Invert(mMat);

         UtMatrixd retval;
         retval.Multiply(Jinv, aState.Vector());

         return Q{retval};
      }

      static const int cDIMENSION = Q::cDIMENSION;

   private:
      UtMatrixd mMat;
   };

   using FullStateSolver = ut::NewtonRaphson<SearchVector, StateVector, Jacobian<SearchVector, StateVector>>;

   StateVector  Propagate(const SearchVector& aElems) const;
   SearchVector InitialOsculatingElements(const StateVector& aState) const;

   std::unique_ptr<WsfNORAD_OrbitalPropagator> mPropPtr;
   UtCalendar                                  mEpoch;
   UtECI_Conversion                            mECI_TEME_Converter;
   StateVector                                 mTargetStateTEME;
   SearchVector                                mSearchSolution;
   bool                                        mSolutionFound;
   UtTwoLineElement                            mReferenceTLE;
};


#endif // WSFNORADPROPAGATORINVERTER_HPP
