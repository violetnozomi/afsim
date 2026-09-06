// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef CUBIC_SPLINE_HPP
#define CUBIC_SPLINE_HPP

#include <cstddef>
#include <list>
#include <vector>

namespace Designer
{
   class CubicSpline
   {
      public:
         enum BC_Type
         {
            cFIRST_DERIV,
            cSECOND_DERIV,
            cUNSPECIFIED
         };

         struct Knot
         {
            Knot()
               : coordinates(0.0, 0.0)
               , BCSpecification(BC_Type::cFIRST_DERIV)
               , boundaryCondition(0.0) {}

            Knot(double  aFirstCoordinate,
                 double  aSecondCoordinate,
                 BC_Type aBCSpecification,
                 double  aBoundaryCondition)
               : coordinates(aFirstCoordinate, aSecondCoordinate)
               , BCSpecification(aBCSpecification)
               , boundaryCondition(aBoundaryCondition) {}

            std::pair<double, double> coordinates;
            BC_Type BCSpecification;
            double boundaryCondition;
         };

         CubicSpline();
         ~CubicSpline();

         std::vector<std::pair<double, double>> GetKnotPoints();
         const std::vector<std::pair<double, double>>& GetSpline() { return mSpline; }
         const std::vector<CubicSpline::Knot>& GetKnots() { return mKnots; }
         CubicSpline::Knot GetKnot(size_t aIndex);
         size_t GetPointCountBetweenKnots() { return mPointsBetweenKnots; }

         // This adds a knot to the end of the list
         void AddKnot(CubicSpline::Knot aKnot);

         bool AddKnot(CubicSpline::Knot aKnot, size_t aIndex);
         bool CreateKnotAtX(double aXCoord);
         bool DeleteKnot(size_t aIndex);

         bool SetKnotPoint(const std::pair<double, double>& aCoords, size_t aIndex);
         bool SetKnot(const CubicSpline::Knot& aKnot, size_t aIndex);

         void SetKnotVec(std::vector<CubicSpline::Knot>& aKnotList);
         void SetPointCountBetweenKnots(size_t aPointCount);

         // All generation of the spline data from external calls
         void GenerateSplineData() { GenerateSpline(); };

         double GetYValueForX(double aX);

      private:
         void GenerateSpline();

         size_t                                 mPointsBetweenKnots;
         std::vector<std::pair<double, double>> mSpline;
         std::vector<CubicSpline::Knot>         mKnots;
   };
}
#endif // !CUBIC_SPLINE_HPP
