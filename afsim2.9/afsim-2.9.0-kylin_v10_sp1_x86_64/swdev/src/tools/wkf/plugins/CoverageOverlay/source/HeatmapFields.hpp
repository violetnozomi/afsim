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

#ifndef HEATMAPFIELDS_HPP
#define HEATMAPFIELDS_HPP

#include "UtBoundingBox.hpp"

namespace CoverageOverlay
{

//! This object provides data to a HeatmapOverlay object.
//!
//! The data is arranged in a set of components, each of which is a grid in
//! latitude and longitude made up of quadrillaterals. The component grids are
//! organized as if they were regular rectangular grids, but the latitudes
//! for the points in a 'row' of the grid, or the longitudes in a 'column' of
//! the grid need not be identical along the row or column. For each grid point,
//! there are a number of field values available. Each grid component will
//! provide data for the same set of fields. The fields report which unit type
//! is appropriate for the value stored, but the data is always stored in the
//! standard unit of the appropriate type.
class HeatmapFields
{
public:
   virtual ~HeatmapFields() = default;

   //! Get the name of the set of fields.
   virtual std::string GetName() const = 0;

   //! Get the number of components.
   virtual std::size_t GetNumComponents() const = 0;

   //! Get the number of fields.
   virtual std::size_t GetNumFields() const = 0;

   //! Get the name of the indicated field.
   virtual std::string GetFieldName(std::size_t aFieldIndex) const = 0;

   //! Get the type of the indicated field.
   virtual std::string GetFieldType(std::size_t aFieldIndex) const = 0;

   //! Get the subtype of the indicated field.
   virtual std::string GetFieldSubtype(std::size_t aFieldIndex) const = 0;

   //! Get the unit type of the indicated field.
   virtual int GetFieldUnitType(std::size_t aFieldIndex) const = 0;

   //! Get the range of values of the indicated field.
   virtual void GetFieldValueRange(std::size_t aFieldIndex, float& aMinValue, float& aMaxValue) const = 0;

   //! Get the number of latitude 'lines' in the component with the given @p aComponentIndex.
   virtual std::size_t GetNumLats(std::size_t aComponentIndex) const = 0;

   //! Get the number of longitude 'lines' in the component with the given @p aComponentIndex.
   virtual std::size_t GetNumLons(std::size_t aComponentIndex) const = 0;

   //! Get the latitude of the given point.
   virtual float GetLatitude(std::size_t aComponentIndex, std::size_t aLatIndex, std::size_t aLonIndex) const = 0;

   //! Get the longitude of the given point.
   virtual float GetLongitude(std::size_t aComponentIndex, std::size_t aLatIndex, std::size_t aLonIndex) const = 0;

   //! Get altitude at the given point.
   virtual float GetAltitude(std::size_t aComponentIndex, std::size_t aLatIndex, std::size_t aLonIndex) const = 0;

   //! Get field data.
   virtual float GetField(std::size_t aComponentIndex,
                          std::size_t aLatIndex,
                          std::size_t aLonIndex,
                          std::size_t aFieldIndex) const = 0;
};

//! This represents one quadrilateral section of the grid from a HeatmapField.
//!
//! This class is a utility class that provides interpolation on a regular subdivision of
//! that quadrilateral. Locations at which to interpolate are specified via integral indices
//! that are from a range specified by the user.
class Patch
{
public:
   Patch(const HeatmapFields& aFields, std::size_t aComponentIndex, std::size_t aLatIndex, std::size_t aLonIndex);

   //! Set the number of latitude and longitude subdivisions of the patch.
   //!
   //! The interpolation provided by this class works at a fixed set of points
   //! on the quadrilateral. Locations at which to interpolate are specified via integral
   //! indices that are from a range specified by the user. This method sets the
   //! number of allowed locations (including the boundaries) in each direction.
   //!
   //! \param aNumLatDivisions - The number of divisions in the latitude direction.
   //! \param aNumLonDivisions - The number of divisions in the longitude direction.
   void SetNumDivisions(std::size_t aNumLatDivisions, std::size_t aNumLonDivisions);

   //! Return the latitude value of the point with the given index.
   float GetLatitude(std::size_t aLatIndex, std::size_t aLonIndex) const;

   //! Return the longitude value of the point with the given index.
   float GetLongitude(std::size_t aLatIndex, std::size_t aLonIndex) const;

   //! Return the altitude value of the point with the given index.
   float GetAltitude(std::size_t aLatIndex, std::size_t aLonIndex) const;

   //! Return the value of the field with the given @p aFieldIndex at the point with the given index.
   float GetField(std::size_t aLatIndex, std::size_t aLonIndex, std::size_t aFieldIndex) const;

   //! Return the longest span in latitude between any of the four corners of the patch.
   float GetDeltaLat() const;

   //! Return the longest span in longitude between any of the four corners of the patch.
   float GetDeltaLon() const;

private:
   void  SetupIndices();
   float Interpolate(float       aValueLL,
                     float       aValueLR,
                     float       aValueUL,
                     float       aValueUR,
                     std::size_t aLatIndex,
                     std::size_t aLonIndex) const;

   const HeatmapFields& mFields;
   std::size_t          mComponentIndex;
   std::size_t          mLowLatIndex{0};
   std::size_t          mLowLonIndex{0};
   std::size_t          mHighLatIndex{0};
   std::size_t          mHighLonIndex{0};
   std::size_t          mNumLatDivisions{1};
   std::size_t          mNumLonDivisions{1};
};

} // namespace CoverageOverlay

#endif // HEATMAPFIELDS_HPP
