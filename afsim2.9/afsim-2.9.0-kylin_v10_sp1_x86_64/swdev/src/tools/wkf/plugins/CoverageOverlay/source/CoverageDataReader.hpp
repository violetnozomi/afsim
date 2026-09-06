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

#ifndef COVERAGEDATAREADER_HPP
#define COVERAGEDATAREADER_HPP

#include <memory>
#include <vector>

class QString;
class QTextStream;

#include "CoverageData.hpp"

namespace CoverageOverlay
{

//! This class is responsible for reading the coverage overlay data file and
//! creating a CoverageData object from that file.
class CoverageDataReader
{
public:
   explicit CoverageDataReader(const QString& aFileName);

   //! Returns true if this object can return valid coverage data.
   //!
   //! This will return true if coverage data was successfully read from the file
   //! indicated when this object was constructed, and if the resulting data has
   //! not already been retrieved from this object.
   //!
   //! \returns true if this object has data that can be retrieved; false otherwise.
   bool IsValid() const { return mValid; }

   //! Return the data read from file.
   //!
   //! This will only return a non-null object if IsValid() would return true before
   //! the call to this function. After the call to this function, IsValid()
   //! will return false.
   //!
   //! \returns The coverage data read from file by this object, or nullptr.
   std::unique_ptr<CoverageData> GetLoadedData();

private:
   struct Record
   {
      size_t             mLatIndex;
      size_t             mLonIndex;
      std::vector<float> mFields{};
   };

   void ReadHeader(QTextStream& aStream);
   void ReadMeasureNames(QTextStream& aStream);
   void ReadMeasureTypes(QTextStream& aStream);
   void ReadMeasureSubtypes(QTextStream& aStream);
   void ReadBlock(QTextStream& aStream);
   void ProcessRecords(std::size_t aNumLats, std::size_t aNumLons, std::vector<Record>&& aRecords);

   std::unique_ptr<CoverageData> mData{nullptr};
   int                           mNumComponents{0};
   bool                          mValid{false};
};

} // namespace CoverageOverlay

#endif // COVERAGEDATAREADER_HPP
