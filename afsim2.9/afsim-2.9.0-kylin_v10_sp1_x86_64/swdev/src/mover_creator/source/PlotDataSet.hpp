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

#ifndef PLOT_DATA_SET_HPP
#define PLOT_DATA_SET_HPP

#include <cstddef>
#include <vector>

namespace Designer
{
   class PlotDataSet
   {
      public:
         PlotDataSet() = default;
         ~PlotDataSet() = default;

         std::vector<std::pair<double, double>> GetPoints() const { return mPlotData; }
         std::pair<double, double> GetPoint(size_t aIndex) const { return mPlotData.at(aIndex); }

         // This adds a knot to the end of the list
         void AddPoint(std::pair<double, double> aPoint) {mPlotData.push_back(aPoint);}

         // Add a point at the given index
         bool AddPoint(std::pair<double, double> aPoint, size_t aIndex);
         bool CreatePointAtX(double aXCoord);
         bool DeletePoint(size_t aIndex);

         // Set the point at the given index to the new coords
         bool SetPoint(const std::pair<double, double>& aCoords, size_t aIndex);

         // Set the entire plot data vector
         void SetPlotData(std::vector<std::pair<double, double>>&& aPointVec);

         double GetYValueForX(double aX);

      private:
         std::vector<std::pair<double, double>> mPlotData;
   };
}
#endif // !PLOT_DATA_SET_HPP

