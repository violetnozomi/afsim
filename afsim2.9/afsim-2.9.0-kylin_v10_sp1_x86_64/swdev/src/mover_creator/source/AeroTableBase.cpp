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

#include "AeroTableBase.hpp"

#include <cmath>
#include <sstream>
#include <stdexcept>

#include <QJsonObject>
#include <QJsonArray>

#include "UtLog.hpp"

namespace Designer
{

double AeroTableBase::mEpsilon = std::numeric_limits<double>::epsilon();

bool AeroTableBase::Interpolate(const std::vector<std::pair<double, double>>& aData,
                                const double&                                 aIndependentValue,
                                double&                                       aResult) const
{
   size_t lowerBoundIndex      = 0;
   size_t upperBoundIndex      = 0;
   bool   lowerBoundCondition  = false;
   bool   upperBoundCondition  = false;
   bool   singleBoundCondition = false;

   bool result = GetUpperAndLowerBoundIndices(aData, aIndependentValue, lowerBoundIndex, upperBoundIndex,
                                              lowerBoundCondition, upperBoundCondition, singleBoundCondition);

   if (!result)
   {
      return false;
   }

   if (singleBoundCondition)
   {
      aResult = aData.at(lowerBoundIndex).second;
      return true;
   }

   if (lowerBoundCondition)
   {
      double dx = aData.at(upperBoundIndex).first - aData.at(lowerBoundIndex).first;
      double dy = aData.at(upperBoundIndex).second - aData.at(lowerBoundIndex).second;

      if (dx < mEpsilon)
      {
         // Infinite slope - we will clamp it to end point
         aResult = aData.at(lowerBoundIndex).second;
         return true;
      }

      double slope = dy / dx;
      dx = aIndependentValue - aData.at(lowerBoundIndex).first;
      aResult = aData.at(lowerBoundIndex).second + slope * dx;
      return true;
   }

   if (upperBoundCondition)
   {
      double dx = aData.at(upperBoundIndex).first - aData.at(lowerBoundIndex).first;
      double dy = aData.at(upperBoundIndex).second - aData.at(lowerBoundIndex).second;

      if (dx < mEpsilon)
      {
         // Infinite slope - we will clamp it to end point
         aResult = aData.at(upperBoundIndex).second;
         return true;
      }

      double slope = dy / dx;
      dx = aIndependentValue - aData.at(upperBoundIndex).first;
      aResult = aData.at(lowerBoundIndex).second + slope * dx;
      return true;
   }

   aResult = InterpolateUsingPairs(aData.at(lowerBoundIndex), aData.at(upperBoundIndex), aIndependentValue);

   return true;
}

void AeroTableBase::ApplyModifier(const QJsonObject& aModifier)
{
   for (auto variable = aModifier.constBegin(); variable != aModifier.constEnd(); ++variable)
   {
      std::string varName = variable.key().toStdString();
      if (HasIndependentVarName(varName))
      {
         QJsonObject varMod = variable->toObject();
         auto gainIter = varMod.constFind("gain");
         if (gainIter != varMod.constEnd())
         {
            if (gainIter->isObject())
            {
               auto modifierLine = AeroTableBase::GetModifierLine(gainIter->toObject());
               SetListBasedGainModifier(varName, modifierLine);
            }
            else if (gainIter->isDouble())
            {
               double gainModifierValue = gainIter->toDouble();
               SetSingleGainModifier(varName, gainModifierValue);
            }
            else
            {
               std::stringstream ss{};
               ss << "Bad input in \"modified_aero\" block for table '" << GetTableName()
                  << "': \"gain\" should be a single value, or an object with two arrays of values.";
               throw std::runtime_error(ss.str());
            }
         }

         auto offsetIter = varMod.constFind("offset");
         if (offsetIter != varMod.constEnd())
         {
            if (offsetIter->isObject())
            {
               auto modifierLine = AeroTableBase::GetModifierLine(offsetIter->toObject());
               SetListBasedOffsetModifier(varName, modifierLine);
            }
            else if (offsetIter->isDouble())
            {
               double offsetModifierValue = offsetIter->toDouble();
               SetSingleOffsetModifier(varName, offsetModifierValue);
            }
            else
            {
               throw std::runtime_error("Bad input in \"modified_aero\" block: \"offset\" should be a single value, or an object with to arrays of values.");
            }
         }
      }
      else
      {
         std::stringstream ss{};
         ss << "Bad input in \"modified_aero\" block: There is no variable '"
            << varName << "' in the table '" << GetTableName() << "'";
         throw std::runtime_error(ss.str());
      }
   }
}

bool AeroTableBase::GetUpperAndLowerBoundIndices(const std::vector<std::pair<double, double>>& aData,
                                                 const double&                                 aIndependentValue,
                                                 size_t&                                       aLowerBoundIndex,
                                                 size_t&                                       aUpperBoundIndex,
                                                 bool&                                         aLowerBoundCondition,
                                                 bool&                                         aUpperBoundCondition,
                                                 bool&                                         aSingleBoundCondition) const
{
   size_t numElements = aData.size();
   if (numElements == 0)
   {
      // Empty list
      return false;
   }

   if (numElements == 1)
   {
      // Single item
      aLowerBoundIndex      = 0;
      aUpperBoundIndex      = 0;
      aLowerBoundCondition  = false;
      aUpperBoundCondition  = false;
      aSingleBoundCondition = true;

      return true;
   }

   size_t lowerBoundIndex = 0;
   double lowerIndependentVar = aData.at(lowerBoundIndex).first;

   // Check for lower bound condition
   if (aIndependentValue < lowerIndependentVar)
   {
      aLowerBoundIndex      = lowerBoundIndex;
      aUpperBoundIndex      = lowerBoundIndex + 1;
      aLowerBoundCondition  = true;
      aUpperBoundCondition  = false;
      aSingleBoundCondition = false;

      return true;
   }

   size_t upperBoundIndex = numElements - 1;
   double upperIndependentVar = aData.at(upperBoundIndex).first;

   // Check for upper bound condition
   if (aIndependentValue > upperIndependentVar)
   {
      // Clamp to the upper bound index and return
      aLowerBoundIndex      = upperBoundIndex - 1;
      aUpperBoundIndex      = upperBoundIndex;
      aLowerBoundCondition  = false;
      aUpperBoundCondition  = true;
      aSingleBoundCondition = false;

      return true;
   }

   // Upper bound edge case, treat it as a single bound condition when the point falls on the upper bound
   if (std::abs(aIndependentValue - upperIndependentVar) <= mEpsilon)
   {
      // The independent value is within epsilon of the test point,
      // so return that index for both low and high.
      aLowerBoundIndex = upperBoundIndex;
      aUpperBoundIndex = upperBoundIndex;
      aSingleBoundCondition = true;
      return true;
   }

   // Clear flags
   aLowerBoundCondition  = false;
   aUpperBoundCondition  = false;
   aSingleBoundCondition = false;

   // If we have only two elements (and since we did not hit the
   // lower or upper bound conditions) we are done
   if (numElements == 2)
   {
      aLowerBoundIndex = lowerBoundIndex;
      aUpperBoundIndex = upperBoundIndex;
      return true;
   }

   while (true)
   {
      size_t delIndex = upperBoundIndex - lowerBoundIndex;

      if (delIndex == 1)
      {
         // We are done
         aLowerBoundIndex = lowerBoundIndex;
         aUpperBoundIndex = upperBoundIndex;
         return true;
      }
      else if (delIndex < 1)
      {
         auto outStream = ut::log::error() << "delIndex is < 1 in AeroTableBase::GetUpperAndLowerBoundIndices().";
         outStream.AddNote() << "delIndex: " << delIndex;
         return false;
      }

      size_t midIndex  = delIndex / 2;
      size_t testIndex = lowerBoundIndex + midIndex;

      double testIndependentVar = aData.at(testIndex).first;

      if (std::abs(aIndependentValue - testIndependentVar) <= mEpsilon)
      {
         // The independent value is within epsilon of the test point,
         // so return that index for both low and high.
         aLowerBoundIndex      = testIndex;
         aUpperBoundIndex      = testIndex;
         aSingleBoundCondition = true;
         return true;
      }
      else if (aIndependentValue < testIndependentVar)
      {
         // The test point is higher than the target, so make the test point the new upper value
         upperBoundIndex     = testIndex;
      }
      else // aIndependentValue > testIndependentVar
      {
         // The test point is lower than the target, so make the test point the new lower value
         lowerBoundIndex     = testIndex;
      }
   }

   // The function should return prior to this point, so if we get here, return false,
   // to indicate data is not valid
   return false;
}

double AeroTableBase::InterpolateUsingPairs(const std::pair<double, double>& aLowerPair,
                                            const std::pair<double, double>& aUpperPair,
                                            const double&                    aTarget) const
{
   // Check for equality
   if (std::abs(aTarget - aLowerPair.first) < mEpsilon)
   {
      return aLowerPair.second;
   }
   if (std::abs(aTarget - aUpperPair.first) < mEpsilon)
   {
      return aUpperPair.second;
   }

   // Check for exceeding lower end
   if (aTarget < (aLowerPair.first + mEpsilon))
   {
      // Need to extrapolate

      double dx = aUpperPair.first - aLowerPair.first;
      double dy = aUpperPair.second - aLowerPair.second;

      if (dx < mEpsilon)
      {
         // Infinite slope - we will clamp it to end point
         return aLowerPair.second;
      }

      double slope = dy / dx;
      dx = aTarget - aLowerPair.first;
      return slope * dx;
   }

   // Check for exceeding upper end
   if (aTarget > (aUpperPair.first - mEpsilon))
   {
      // Need to extrapolate

      double dx = aUpperPair.first - aLowerPair.first;
      double dy = aUpperPair.second - aLowerPair.second;

      if (dx < mEpsilon)
      {
         // Infinite slope - we will clamp it to end point
         return aUpperPair.second;
      }

      double slope = dy / dx;
      dx = aTarget - aUpperPair.first;
      return slope * dx;
   }

   // Determine delta for independent var
   double deltaIndependent = aUpperPair.first - aLowerPair.first;

   // Check for delta = zero
   if (std::abs(deltaIndependent) <= mEpsilon)
   {
      // Return the lower-end value
      return aLowerPair.second;
   }

   // Determine delta for dependent var
   double deltaDependent = aUpperPair.second - aLowerPair.second;

   // Determine fraction for independent var
   double fraction = (aTarget - aLowerPair.first) / deltaIndependent;

   return aLowerPair.second + deltaDependent * fraction;
}

double AeroTableBase::CalcSlope(const std::pair<double, double>& aLowerPair,
                                const std::pair<double, double>& aUpperPair)
{
   double deltaX = std::abs(aUpperPair.first - aLowerPair.first);
   double deltaY = std::abs(aUpperPair.second - aLowerPair.second);
   double slope = 0.0;
   if (deltaX > mEpsilon)
   {
      slope = deltaY / deltaX;
   }

   return slope;
}

std::vector<std::pair<double, double>> AeroTableBase::GetModifierLine(const QJsonObject& aModifierDetails)
{
   auto xValIter = aModifierDetails.constFind("x_values");
   if (xValIter == aModifierDetails.constEnd() || !xValIter->isArray())
   {
      throw std::runtime_error("Bad input in \"modified_aero block\": modifier line's \"x_values\" is not an array");
   }

   auto yValIter = aModifierDetails.constFind("y_values");
   if (yValIter == aModifierDetails.constEnd() || !yValIter->isArray())
   {
      throw std::runtime_error("Bad input in \"modified_aero\" block: modifier line's \"y_values\" is not an array");
   }

   std::vector<std::pair<double, double>> retval{};
   QJsonArray xVals = xValIter->toArray();
   QJsonArray yVals = yValIter->toArray();
   auto xIter = xVals.begin();
   auto yIter = yVals.begin();
   while (xIter != xVals.end())
   {
      retval.emplace_back(std::make_pair(xIter->toDouble(), yIter->toDouble()));
      ++xIter;
      ++yIter;
   }
   return retval;
}

} // namespace Designer
