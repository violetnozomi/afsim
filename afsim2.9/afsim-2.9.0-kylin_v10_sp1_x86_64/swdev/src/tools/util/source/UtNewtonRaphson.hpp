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

#ifndef UTNEWTONRAPHSON_HPP
#define UTNEWTONRAPHSON_HPP

#include <utility>

namespace ut
{

//! A utility class to perform an iterative root finding using the Newton-Raphson method.
//! The function under consideration is a mapping from a search space into a state space.
//! Elements of these spaces are represented using the SearchVector and StateVector types,
//! respectively. The Jacobian type will contain the linear approximation to the system of
//! equations and will provide the utility of solving the linear system. This object will
//! assume that for the system in question it is impossible or time-consuming to produce
//! an analytic version of the linearized system. Instead, this will produce a finite
//! difference approximation to the Jacobian.
//!
//! The search and state space are distinct to allow for the StateVector and SearchVector
//! to wrap objects for which there is no obvious connection. For example, this is used
//! in WsfNORAD_PropagatorInverter, which has a search space of orbital elements and a
//! state space of pairs of position and velocity.
//!
//! SearchVector must provide:
//! * a compile-time constant integral value cDIMENSION giving the dimensionality of the space,
//! * a copy constructor,
//! * a -= operator,
//! * a + operator,
//! * a double Component(int) method that returns a component of the vector if used in the Solve
//!   method,
//! * a static SearchVector UnitVector(int, double) method that returns a SearchVector
//!   with all components equal to zero, except the component at the given index which
//!   has the given value.
//!
//! StateVector must provide:
//! * a compile-time constant integral value cDIMENSION giving the dimensionality of the space,
//! * a copy constructor,
//! * a - operator,
//! * a * operator taking a double that scales the vector by the given double,
//! * a double Component(int) method that returns a component of the vector if used in the
//!   SolveState method.
//!
//! Jacobian must provide:
//! * a compile-time constant integral value cDIMENSION giving the dimensionality of the space,
//! * a copy constructor,
//! * a method SearchVector InverseProduct(const StateVector& aState) that returns the product
//!   of the inverse of the Jacobian with the given StateVector,
//! * a method void SetColumn(int aIndex, const StateVector& aColumn) that sets the aIndex-th
//!   column of the Jacobian to the given value.
template<typename SearchVector, typename StateVector, typename Jacobian>
class NewtonRaphson
{
public:
   static_assert(SearchVector::cDIMENSION == StateVector::cDIMENSION, "Search and State space must have same dimension");
   static_assert(SearchVector::cDIMENSION == Jacobian::cDIMENSION,
                 "Jacobian dimension must match the search and state vector dimension");

   //! Find the solution to the system.
   //! This will take a callable entity that has the signature
   //! StateVector aFunction(const SearchVector& aSearch) and a tolerance
   //! in the search space and will iterate to find the solution of the system.
   //! This method reports if the solution is found, and provides the solution via
   //! @p aSolution. The iteration stops when the change in
   //! the search vector over an iteration has no component with a magnitude
   //! larger than \p aTolerance.
   //! \param aFunction  - a callable entity mapping SearchVectors into StateVectors.
   //! \param aTolerance - the convergence tolerance.
   //! \returns          - true if a solution is found; false otherwise.
   template<typename Callable>
   static bool Solve(const SearchVector& aInitialGuess,
                     const StateVector&  aTargetState,
                     const Callable&     aFunction,
                     double              aTolerance,
                     SearchVector&       aSolution)
   {
      static constexpr int cMAX_ITER = 400;

      SearchVector Q_Current = aInitialGuess;
      SearchVector deltaQ    = Q_Current;
      StateVector  X_Current = aFunction(Q_Current);
      StateVector  deltaX    = X_Current - aTargetState;

      for (int k = 0; k < cMAX_ITER && (k == 0 || !WithinTolerance(deltaQ, aTolerance)); ++k)
      {
         Jacobian J = ComputeJacobian(aFunction, Q_Current, X_Current);
         try
         {
            deltaQ = J.InverseProduct(deltaX);
         }
         catch (const std::exception&) // Singular matrix.
         {
            break; // No solution
         }
         Q_Current -= deltaQ;
         X_Current = aFunction(Q_Current);
         deltaX    = X_Current - aTargetState;
      }

      aSolution = Q_Current;

      return WithinTolerance(deltaQ, aTolerance);
   }

   //! Find the solution to the system.
   //! This will take a callable entity that has the signature
   //! StateVector aFunction(const SearchVector& aSearch) and a tolerance
   //! in the state space and will iterate to find the solution of the system.
   //! This method reports if the solution is found, and returns the solution via
   //! @p aSolution. The iteration stops when the change in
   //! the state vector over an iteration has no component with a magnitude
   //! larger than \p aTolerance.
   //! \param aFunction  - a callable entity mapping SearchVectors into StateVectors.
   //! \param aTolerance - the convergence tolerance.
   //! \returns          - true if a solution is found; false otherwise.
   template<typename Callable>
   static bool SolveState(const SearchVector& aInitialGuess,
                          const StateVector&  aTargetState,
                          const Callable&     aFunction,
                          double              aTolerance,
                          SearchVector&       aSolution)
   {
      static constexpr int cMAX_ITER = 100;

      SearchVector Q_Current = aInitialGuess;
      SearchVector deltaQ    = Q_Current;
      StateVector  X_Current = aFunction(Q_Current);
      StateVector  deltaX    = X_Current - aTargetState;

      for (int k = 0; k < cMAX_ITER && (k == 0 || !WithinStateTolerance(deltaX, aTolerance)); ++k)
      {
         Jacobian J = ComputeJacobian(aFunction, Q_Current, X_Current);
         try
         {
            deltaQ = J.InverseProduct(deltaX);
         }
         catch (const std::exception&) // Singular matrix.
         {
            break; // No solution
         }
         Q_Current -= deltaQ;
         X_Current = aFunction(Q_Current);
         deltaX    = X_Current - aTargetState;
      }

      aSolution = Q_Current;

      return WithinStateTolerance(deltaX, aTolerance);
   }

private:
   //! Return if the given search vector is within the given tolerance.
   //! This compares the magnitude of each component of @p aVector to the tolerance.
   //! This is equivalent to requiring the infinity norm of the given vector be smaller
   //! than the given @p aTolerance.
   //! @param aVector    - the vector to compare.
   //! @param aTolerance - the maximum allowable infinity norm.
   //! @returns          - true if within tolerance; false otherwise.
   static bool WithinTolerance(const SearchVector& aVector, double aTolerance)
   {
      bool retval{true};
      for (int i = 0; i < SearchVector::cDIMENSION; ++i)
      {
         if (fabs(aVector.Component(i)) > aTolerance)
         {
            retval = false;
            break;
         }
      }
      return retval;
   }

   //! Return if the given state vector is within the given tolerance.
   //! This compares the magnitude of each component of @p aVector to the tolerance.
   //! This is equivalent to requiring the infinity norm of the given vector be smaller
   //! than the given @p aTolerance.
   //! @param aVector    - the vector to compare.
   //! @param aTolerance - the maximum allowable infinity norm.
   //! @returns          - true if within tolerance; false otherwise.
   static bool WithinStateTolerance(const StateVector& aVector, double aTolerance)
   {
      bool retval{true};
      for (int i = 0; i < StateVector::cDIMENSION; ++i)
      {
         if (fabs(aVector.Component(i)) > aTolerance)
         {
            retval = false;
            break;
         }
      }
      return retval;
   }

   //! Compute the Jacobian numerically.
   //! This performs a first-order finite difference approximation to the Jacobian for the
   //! system. This will perform one extra evaluation of @p aFunction for each dimension of
   //! search space.
   //! @param aFunction - the mapping function from search space into state space.
   //! @param aQ_Zero   - the element of search space at which to evaluate the Jacobian.
   //! @param aX_Zero   - the element of state space that is the image of @p aQ_Zero under
   //!                    @p aFunction.
   //! @returns         - the approximate Jacobian.
   template<typename Callable>
   static Jacobian ComputeJacobian(const Callable& aFunction, const SearchVector& aQ_Zero, const StateVector& aX_Zero)
   {
      static const double cDIFF_STEP              = 1.0e-6;
      static const double cSEARCH_COMPONENT_FLOOR = 0.01;

      Jacobian retval{};
      for (int i = 0; i < Jacobian::cDIMENSION; ++i)
      {
         double       h       = cDIFF_STEP * std::max(fabs(aQ_Zero.Component(i)), cSEARCH_COMPONENT_FLOOR);
         SearchVector Q_Prime = aQ_Zero + SearchVector::UnitVector(i, h);
         StateVector  X_Prime = aFunction(Q_Prime);
         StateVector  J_i     = (X_Prime - aX_Zero) * (1.0 / h);
         retval.SetColumn(i, J_i);
      }
      return retval;
   }
};

//! One dimensional (real) Newton-Raphson method.
//!
//! This method finds the roots of the given @p aFunction. The
//! provided @p aFunction should be any callable entity that takes
//! a single double argument and returns a double. The provided
//! @p aDerivative should also take a single double argument and
//! return a double, and it should accurately reflect the
//! derivative of the provided @p aFunction. The quality of the
//! given @p aInitialGuess will determine the efficiency and even
//! if this method will yield a result. Functions with regions
//! that have very small derivatives often behave poorly in this
//! method.
//!
//! \param aFunction     - The function for which the root is needed.
//! \param aDerivative   - The derivative of @p aFunction.
//! \param aInitialGuess - An initial guess for the value of the independent variable.
//! \param aTolerance    - Tolerance for the root search.
//! \returns             - A pair containing the root and a boolean indicating if the search
//!                        was successful.
template<typename Function, typename Derivative>
std::pair<double, bool> NewtonRaphson1D(Function aFunction, Derivative aDerivative, double aInitialGuess, double aTolerance)
{
   static constexpr int cMAX_ITER{100};

   double xCurrent = aInitialGuess;
   double fCurrent = aFunction(xCurrent);

   for (int k = 0; k < cMAX_ITER && fabs(fCurrent) > aTolerance; ++k)
   {
      xCurrent -= fCurrent / aDerivative(xCurrent);
      fCurrent = aFunction(xCurrent);
   }

   bool success = fabs(fCurrent) <= aTolerance;
   return std::make_pair(xCurrent, success);
}

} // namespace ut

#endif // UTNEWTONRAPHSON_HPP
