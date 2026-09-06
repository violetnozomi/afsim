// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFQUANTUMMATRIX_HPP
#define WSFQUANTUMMATRIX_HPP

#include "wsf_mil_export.h"

#include <vector>

#include "WsfAssetPerception.hpp"
class WsfQuantumEvaluator;
#include "WsfQuantumTask.hpp"


class WSF_MIL_EXPORT WsfQuantumMatrix
{
public:
   WsfQuantumMatrix() = default;
   WsfQuantumMatrix(const WsfQuantumMatrix& aSrc);
   WsfQuantumMatrix& operator=(const WsfQuantumMatrix&);
   virtual ~WsfQuantumMatrix();

   std::vector<WsfAssetPerception*>& Assets() { return mAssets; }
   std::vector<WsfQuantumTask*>&     Tasks() { return mTasks; }
   std::vector<std::vector<double>>& Values() { return mValues; }
   double Value(size_t taskIndex, size_t assetIndex) { return mValues[taskIndex][assetIndex]; }
   std::vector<std::vector<double>>& Profit() { return mProfits; }
   double       Profit(size_t taskIndex, size_t assetIndex) { return mProfits[taskIndex][assetIndex]; }
   void         ClearAssets();
   void         SetAssets(std::vector<WsfAssetPerception*>& aAssets);
   void         SetTasks(std::vector<WsfQuantumTask*>& aTasks);
   virtual void UpdateEvaluations(double aSimTime, WsfQuantumEvaluator& aEvaluator, std::vector<WsfLocalTrack*>& aPerception);

   void ClearTasks();
   void EraseTask(size_t taskIndex);    // row
   void RemoveTask(size_t taskIndex);   // row
   void EraseAsset(size_t assetIndex);  // column
   void RemoveAsset(size_t assetIndex); // column
   // tasks are in "rows"
   static void SetRow(std::vector<std::vector<double>>& m, size_t aRow, double aValue);
   // tasks are in "rows"
   static void RemoveRow(std::vector<std::vector<double>>& m, size_t aRow);
   // assets are in "columns"
   static void SetColumn(std::vector<std::vector<double>>& m, size_t aCol, double aValue);
   // assets are in "columns"
   static void RemoveColumn(std::vector<std::vector<double>>& m, size_t aCol);

protected:
   std::vector<WsfAssetPerception*> mAssets;
   std::vector<WsfQuantumTask*>     mTasks;
   std::vector<std::vector<double>> mValues;
   std::vector<std::vector<double>> mProfits;
};

#endif
