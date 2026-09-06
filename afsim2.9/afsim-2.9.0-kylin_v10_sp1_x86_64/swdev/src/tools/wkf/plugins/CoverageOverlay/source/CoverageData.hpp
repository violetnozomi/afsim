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

#ifndef COVERAGEDATA_HPP
#define COVERAGEDATA_HPP

#include <string>
#include <vector>

#include "HeatmapFields.hpp"
#include "VaBoundingBox.hpp"

namespace CoverageOverlay
{

//! Provides the field data from a coverage computation for a HeatmapOverlay.
class CoverageData : public HeatmapFields
{
public:
   CoverageData()                           = default;
   CoverageData(const CoverageData& aOther) = default;
   ~CoverageData() override                 = default;

   //! Return the name of the data set.
   std::string GetName() const override { return mCoverageName; }

   //! Return the number of data components.
   std::size_t GetNumComponents() const override { return mComponents.size(); }

   //! Return the number of fields provided by this data set.
   std::size_t GetNumFields() const override { return mNumFields; }

   //! Return the name of the field with the given @p aFieldIndex.
   std::string GetFieldName(std::size_t aFieldIndex) const override { return mFieldNames.at(aFieldIndex); }

   //! Return the type of the indicated field.
   std::string GetFieldType(std::size_t aFieldIndex) const override { return mFieldTypes.at(aFieldIndex); }

   //! Return the subtype of the indicated field.
   std::string GetFieldSubtype(std::size_t aFieldIndex) const override { return mFieldSubtypes.at(aFieldIndex); }

   //! Return the unit type of the indicated field.
   int GetFieldUnitType(std::size_t aFieldIndex) const override { return mFieldUnitTypes.at(aFieldIndex); }

   //! Compute the range of values for the given @p aFieldIndex.
   void GetFieldValueRange(std::size_t aFieldIndex, float& aMinValue, float& aMaxValue) const override;

   //! Get the number of latitude 'lines' in the component with the given @p aComponentIndex.
   std::size_t GetNumLats(std::size_t aComponentIndex) const override;

   //! Get the number of longitude 'lines' in the component with the given @p aComponentIndex.
   std::size_t GetNumLons(std::size_t aComponentIndex) const override;

   //! Get the latitude of the given point.
   //!
   //! This will return the latitude at the given @p aLatIndex and @p aLonIndex for the data
   //! component with the given @p aComponentIndex. If the provided indices are outside the
   //! bounds of the selected component, the closest point in the component will be used
   //! instead.
   //!
   //! \param aComponentIndex - The component from which to get a latitude value.
   //! \param aLatIndex       - The latitude index at which to get a latitude value.
   //! \param aLonIndex       - The longitude index at which to get a latitude value.
   //! \returns               - The latitude of the indicated point.
   float GetLatitude(std::size_t aComponentIndex, std::size_t aLatIndex, std::size_t aLonIndex) const override;

   //! Get the longitude of the given point.
   //!
   //! This will return the longitude at the given @p aLatIndex and @p aLonIndex for the data
   //! component with the given @p aComponentIndex. If the provided indices are outside the
   //! bounds of the selected component, the closest point in the component will be used
   //! instead.
   //!
   //! \param aComponentIndex - The component from which to get a longitude value.
   //! \param aLatIndex       - The latitude index at which to get a longitude value.
   //! \param aLonIndex       - The longitude index at which to get a longitude value.
   //! \returns               - The longitude of the indicated point.
   float GetLongitude(std::size_t aComponentIndex, std::size_t aLatIndex, std::size_t aLonIndex) const override;

   //! Return the altitude at the given grid location.
   //!
   //! This will return the altitude at the given @p aLatIndex and @p aLonIndex
   //! for the data component with the given @p aComponentIndex. If the provided indices are
   //! outside the bounds of the selected component, the closest point in the component
   //! will be used instead.
   //!
   //! \param aComponentIndex - The component from which to get an altitude value.
   //! \param aLatIndex       - The latitude index at which to get an altitude value.
   //! \param aLonIndex       - The longitude index at which to get an altitude value.
   //! \returns               - The altitude at the specified location.
   float GetAltitude(std::size_t aComponentIndex, std::size_t aLatIndex, std::size_t aLonIndex) const override;

   //! Return the field at the given grid location.
   //!
   //! This will return the field value at the given @p aLatIndex and @p aLonIndex
   //! for the data component with the given @p aComponentIndex. If the provided indices are
   //! outside the bounds of the selected component, the closest point in the component
   //! will be used instead. The given @p aFieldIndex selects which field value is interpolated
   //! in the result.
   //!
   //! \param aComponentIndex - The component from which to get an altitude value.
   //! \param aLatIndex       - The latitude index at which to get an altitude value.
   //! \param aLonIndex       - The longitude index at which to get an altitude value.
   //! \param aFieldIndex     - The index of the field for which a value will be returned.
   //! \returns               - The field value at the specified location.
   float GetField(std::size_t aComponentIndex,
                  std::size_t aLatIndex,
                  std::size_t aLonIndex,
                  std::size_t aFieldIndex) const override;

   //! Return the name of the file from which the data was loaded.
   const std::string& GetFileName() const { return mFileName; }

   //! Set the name of this data set.
   void SetCoverageName(const std::string& aCoverageName) { mCoverageName = aCoverageName; }

   //! Set the name of the file from which the data was loaded.
   void SetFileName(const std::string& aFileName) { mFileName = aFileName; }

   //! Set the number of fields provided by this data set.
   void SetNumFields(std::size_t aNumFields) { mNumFields = aNumFields; }

   //! Set the names of the fields provided by this data set.
   void SetFieldNames(std::vector<std::string>&& aNames) { mFieldNames = aNames; }

   //! Set the types of the fields provided by this data set.
   void SetFieldTypes(std::vector<std::string>&& aTypes) { mFieldTypes = aTypes; }

   //! Set the subtypes of the fields provided by this data set.
   void SetFieldSubtypes(std::vector<std::string>&& aSubtypes) { mFieldSubtypes = aSubtypes; }

   //! Set the unit types of the fields provided by this data set.
   void SetFieldUnitTypes(std::vector<int>&& aUnitTypes) { mFieldUnitTypes = aUnitTypes; }

   class Component
   {
   public:
      explicit Component(std::size_t aNumLats, std::size_t aNumLons);

      std::size_t GetNumLats() const { return mNumLats; }
      std::size_t GetNumLons() const { return mNumLons; }
      float       GetLatitude(std::size_t aLatIndex, std::size_t aLonIndex) const;
      float       GetLongitude(std::size_t aLatIndex, std::size_t aLonIndex) const;
      float       GetAltitude(std::size_t aLatIndex, std::size_t aLonIndex) const;
      float       GetField(std::size_t aLatIndex, std::size_t aLonIndex, std::size_t aFieldIndex) const;
      float       GetMinValue(std::size_t aFieldIndex) const;
      float       GetMaxValue(std::size_t aFieldIndex) const;

      void SetValues(std::size_t aLatIndex, std::size_t aLonIndex, const std::vector<float>& aValues);

      vespa::VaBoundingBox GetBoundingBox() const;

   private:
      float       GetValue(std::size_t aLatIndex, std::size_t aLonIndex, std::size_t aValueIndex) const;
      std::size_t LinearIndex(std::size_t aLatIndex, std::size_t aLonIndex) const;

      std::size_t                     mNumLats;
      std::size_t                     mNumLons;
      std::vector<std::vector<float>> mValues; // Lat, Lon, Alt, [fields]
   };

   //! Add the given component to this data set.
   void AddComponent(Component&& aComponent);

   vespa::VaBoundingBox GetComponentBoundingBox(std::size_t aComponentIndex) const;
   vespa::VaBoundingBox GetBoundingBox() const;

private:
   std::string              mCoverageName{};
   std::string              mFileName{};
   std::size_t              mNumFields{0};
   std::vector<Component>   mComponents{};
   std::vector<std::string> mFieldNames{};
   std::vector<std::string> mFieldTypes{};
   std::vector<std::string> mFieldSubtypes{};
   std::vector<int>         mFieldUnitTypes{};
};

} // namespace CoverageOverlay

#endif // COVERAGEDATA_HPP
