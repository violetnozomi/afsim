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

#ifndef COVERAGEDATAMANAGER_HPP
#define COVERAGEDATAMANAGER_HPP

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <QObject>
#include <QString>

#include "CoverageData.hpp"

namespace CoverageOverlay
{

//! The CoverageDataManager is the collection of any loaded coverage data file.
class CoverageDataManager : public QObject
{
   Q_OBJECT

public:
   CoverageDataManager() = default;

   //! Return the number of currently loaded coverages.
   std::size_t GetNumCoverages() const { return mCoverages.size(); }

   //! Return the coverage with the given @p aName if it exists.
   const CoverageData* GetCoverageData(const QString& aFileName) const;

   //! Attempt to load each file in the given list.
   void LoadCoverageFiles(const QStringList& aFileNames);

   //! Load data from a given file.
   void LoadCoverageData(const QString& aFileName);

   //! Remove the coverage data for the given file.
   void UnloadCoverageData(const QString& aFileName);

signals:
   void CoverageLoaded(const QString& aFileName);
   void CoverageUnloaded(const QString& aFileName);

private:
   std::map<QString, std::unique_ptr<CoverageData>> mCoverages{};
};

} // namespace CoverageOverlay

#endif // COVERAGEDATAMANAGER_HPP
