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

#include "WsfQuantumMatrix.hpp"

#include "WsfLocalTrack.hpp"
#include "WsfQuantumEvaluator.hpp"


WsfQuantumMatrix::WsfQuantumMatrix(const WsfQuantumMatrix& aSrc)
{
   // clone assets
   for (auto asset : aSrc.mAssets)
   {
      mAssets.push_back(asset->Clone());
   }
   // copy tasks
   mTasks = aSrc.mTasks;

   // copy primitives
   mValues  = aSrc.mValues;
   mProfits = aSrc.mProfits;
}

WsfQuantumMatrix& WsfQuantumMatrix::operator=(const WsfQuantumMatrix& aSrc)
{
   // Deep copy assets
   ClearAssets();
   for (auto asset : aSrc.mAssets)
   {
      mAssets.push_back(asset->Clone());
   }
   // copy tasks
   mTasks = aSrc.mTasks;

   // copy primitives
   mValues  = aSrc.mValues;
   mProfits = aSrc.mProfits;
   return *this;
}

// virtual
WsfQuantumMatrix::~WsfQuantumMatrix()
{
   // delete previous assets
   // these SHOULD BE either clones or new objects, owned only by this matrix
   for (auto& asset : mAssets)
   {
      delete asset;
   }
}

void WsfQuantumMatrix::ClearAssets()
{
   for (auto& asset : mAssets)
   {
      delete asset;
   }
   mAssets.clear();
}

void WsfQuantumMatrix::SetAssets(std::vector<WsfAssetPerception*>& aAssets)
{
   // delete previous assets
   ClearAssets();
   // copy pointers directly
   mAssets = aAssets;

   // update size of matrix
   size_t rows = mTasks.size();
   size_t cols = mAssets.size();
   for (size_t i = 0; i < rows; ++i)
   {
      mValues[i].resize(cols);
      mProfits[i].resize(cols);
   }
}

void WsfQuantumMatrix::ClearTasks()
{
   // delete old tasks
   for (auto& task : mTasks)
   {
      delete task;
   }
   mTasks.clear();
}

void WsfQuantumMatrix::SetTasks(std::vector<WsfQuantumTask*>& aTasks)
{
   size_t prevRows = mTasks.size();

   ClearTasks();

   // copy pointers directly
   mTasks = aTasks;

   // update size of matrix
   size_t rows = mTasks.size();
   size_t cols = mAssets.size();
   mValues.resize(rows);  // could increase or decrease size
   mProfits.resize(rows); // could increase or decrease size
   if (rows > prevRows)
   {
      for (size_t i = prevRows; i < rows; ++i)
      {
         mValues[i].resize(cols);
         mProfits[i].resize(cols);
      }
   }
}


void WsfQuantumMatrix::UpdateEvaluations(double                       aSimTime,
                                         WsfQuantumEvaluator&         aEvaluator,
                                         std::vector<WsfLocalTrack*>& aPerception)
{
   size_t rows = mTasks.size();
   size_t cols = mAssets.size();
   for (size_t i = 0; i < rows; ++i)
   {
      // get the track for task "i"
      WsfStringId    targetName = mTasks[i]->GetTargetName();
      WsfLocalTrack* threatPtr  = nullptr;
      for (size_t k = 0; k < aPerception.size(); ++k)
      {
         if (aPerception[k]->GetTargetName() == targetName)
         {
            threatPtr = aPerception[k];
            break;
         }
      }
      for (size_t j = 0; j < cols; ++j)
      {
         mValues[i][j]  = aEvaluator.Evaluate(aSimTime, mTasks[i], mAssets[j], threatPtr);
         mProfits[i][j] = mValues[i][j] * mTasks[i]->GetPriority();
      }
   }
}


void WsfQuantumMatrix::EraseTask(size_t taskIndex) // row
{
   delete mTasks[taskIndex];
   RemoveTask(taskIndex);
}


void WsfQuantumMatrix::RemoveTask(size_t taskIndex) // row
{
   if (taskIndex < mTasks.size())
   {
      mTasks.erase(mTasks.begin() + taskIndex);
      RemoveRow(mValues, taskIndex);
      RemoveRow(mProfits, taskIndex);
   }
}


void WsfQuantumMatrix::EraseAsset(size_t assetIndex) // column
{
   delete mAssets[assetIndex];
   RemoveAsset(assetIndex);
}


void WsfQuantumMatrix::RemoveAsset(size_t assetIndex) // column
{
   if (assetIndex < mAssets.size())
   {
      mAssets.erase(mAssets.begin() + assetIndex);
      RemoveColumn(mValues, assetIndex);
      RemoveColumn(mProfits, assetIndex);
   }
}


// tasks are in "rows"
// static
void WsfQuantumMatrix::SetRow(std::vector<std::vector<double>>& m, size_t aRow, double aValue)
{
   if (aRow < m.size())
   {
      for (size_t i = 0; i < m[aRow].size(); ++i)
      {
         m[aRow][i] = aValue;
      }
   }
}


// tasks are in "rows"
// static
void WsfQuantumMatrix::RemoveRow(std::vector<std::vector<double>>& m, size_t aRow)
{
   if (aRow < m.size())
   {
      m.erase(m.begin() + aRow);
   }
}


// assets are in "columns"
// static
void WsfQuantumMatrix::SetColumn(std::vector<std::vector<double>>& m, size_t aCol, double aValue)
{
   if (m.size() > 0 && aCol < m[0].size())
   {
      for (size_t i = 0; i < m.size(); ++i)
      {
         m[i][aCol] = aValue;
      }
   }
}


// assets are in "columns"
// static
void WsfQuantumMatrix::RemoveColumn(std::vector<std::vector<double>>& m, size_t aCol)
{
   if (m.size() > 0 && aCol < m[0].size())
   {
      for (size_t i = 0; i < m.size(); ++i)
      {
         m[i].erase(m[i].begin() + aCol);
      }
   }
}
