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

#ifndef AERO_TABLE_BASE_HPP
#define AERO_TABLE_BASE_HPP

#include <QTextStream>

#include <string>
#include <utility>
#include <vector>

class QJsonObject;

// ****************************************************************************
// AeroTableBase provides a base class and support functions for AeroTables classes.
// ****************************************************************************

namespace Designer
{
   class AeroTableBase
   {
      public:
         AeroTableBase() = default;
         virtual ~AeroTableBase() = default;

         // Returns true if the table is non-empty
         virtual bool IsEmpty() = 0;

         // Clears data, sets validity to false
         void Clear() { ClearAllData(); Disable(); }

         // Interpolates using the specified data set using the specified independent value, providing
         // the value in aResult. If the independent value is outside of the range of the data set, the
         // result will be linearly extrapolated. Returns true if the result is valid.
         virtual bool Interpolate(const std::vector<std::pair<double, double>>& aData,
                                  const double&                                 aIndependentValue,
                                  double&                                       aResult) const;

         static double CalcSlope(const std::pair<double, double>& aLowerPair,
                                 const std::pair<double, double>& aUpperPair);

         virtual void UpdateModifiedData() = 0;

         //revert the table's modified data back to matching the original data
         virtual void RevertPlot() = 0;

         // Apply the given modifier to the table object
         void ApplyModifier(const QJsonObject& aModifier);

         // Does the table have the given name as one of its independent variables
         virtual bool HasIndependentVarName(const std::string& aVarName) const = 0;

         virtual std::string GetTableName() const = 0;

         virtual void SetListBasedGainModifier(const std::string& aVarName, const std::vector<std::pair<double, double>>& aModifierLine) = 0;
         virtual void SetSingleGainModifier(const std::string& aVarName, double aModifierValue) = 0;
         virtual void SetListBasedOffsetModifier(const std::string& aVarName, const std::vector<std::pair<double, double>>& aModifierLine) = 0;
         virtual void SetSingleOffsetModifier(const std::string& aVarName, double aModifierValue) = 0;

         bool IsValid() { return mValid; }
         void Enable() { mValid = true; } // IsEmpty(); }
         void Disable() { mValid = false; }

      protected:

         // Determines the lower and upper bound indices for a vector pair for the specified independent
         // value. If the independent value is less than the lower end of the data range, aLowerBoundCondition
         // will be true and the lower bound index will be the lowest bound and the upper bound index will be
         // the next higher index. In a similar manner, if the independent value is greater than the upper end
         // of the data range, aUpperBoundCondition will be true and the upper bound index will be the highest
         // bound and the lower bound index will be the next lower index. If there is only a single pair in
         // the dataset, aSingleBoundCondition will be true. If the dataset is empty, the function will return
         // false, indicating invalid data. In all other cases it returns true.
         virtual bool GetUpperAndLowerBoundIndices(const std::vector<std::pair<double, double>>& aData,
                                                   const double&                                 aIndependentValue,
                                                   size_t&                                       aLowerBoundIndex,
                                                   size_t&                                       aUpperBoundIndex,
                                                   bool&                                         aLowerBoundCondition,
                                                   bool&                                         aUpperBoundCondition,
                                                   bool&                                         aSingleBoundCondition) const;

         // Interpolates between the two specified pairs at the specified target value and returns the result.
         // If the target is outside of the region defined by the lower and upper pairs, the result will be
         // linearly extrapolated.
         virtual double InterpolateUsingPairs(const std::pair<double, double>& aLowerPair,
                                              const std::pair<double, double>& aUpperPair,
                                              const double&                    aTarget) const;

         static std::vector<std::pair<double, double>> GetModifierLine(const QJsonObject& aModifierDetails);

         virtual void ClearAllData() = 0;

         static double mEpsilon;

      private:
         bool mValid = false;
   };
}
#endif // ! AERO_TABLE_BASE_HPP
