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

#include "CoverageDataManager.hpp"

#include <QMessageBox>

#include "CoverageDataReader.hpp"

namespace CoverageOverlay
{

const CoverageData* CoverageDataManager::GetCoverageData(const QString& aFileName) const
{
   const CoverageData* retvalPtr{nullptr};
   auto                iter = mCoverages.find(aFileName);
   if (iter != mCoverages.end())
   {
      retvalPtr = iter->second.get();
   }
   return retvalPtr;
}

void CoverageDataManager::LoadCoverageFiles(const QStringList& aFileNames)
{
   for (auto& fileName : aFileNames)
   {
      LoadCoverageData(fileName);
   }
}

void CoverageDataManager::LoadCoverageData(const QString& aFileName)
{
   try
   {
      CoverageDataReader reader{aFileName};
      auto               covPtr = reader.GetLoadedData();
      auto               iter   = mCoverages.find(aFileName);
      if (iter == mCoverages.end())
      {
         if (mCoverages.emplace(aFileName, std::move(covPtr)).second)
         {
            emit CoverageLoaded(aFileName);
         }
      }
   }
   catch (std::runtime_error& err)
   {
      QMessageBox::critical(nullptr,
                            QString{"Error loading coverage data file."},
                            aFileName + QString{": "} + QString::fromStdString(err.what()),
                            QMessageBox::Ok);
   }
}

void CoverageDataManager::UnloadCoverageData(const QString& aFileName)
{
   auto iter = mCoverages.find(aFileName);
   if (iter != mCoverages.end())
   {
      mCoverages.erase(aFileName);
      emit CoverageUnloaded(aFileName);
   }
}

} // namespace CoverageOverlay
