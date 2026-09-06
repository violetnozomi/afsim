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

#ifndef HEATMAPOVERLAY_HPP
#define HEATMAPOVERLAY_HPP

#include <limits>
#include <map>
#include <string>
#include <vector>

#include <QString>
#include <osg/Array>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Texture1D>
#include <osg/Vec4>
#include <osg/ref_ptr>

#include "HeatmapFields.hpp"
#include "UtQtGradientMap.hpp"
#include "UtoRawShape.hpp"
#include "VaOverlay.hpp"

namespace vespa
{

class VaCamera;

}

namespace CoverageOverlay
{

//! Overlay that displays field data in rectangular lat/lon grids.
//!
//! This overlay adds numerical data to the main viewer visualized as
//! a grid colored according to some gradient, with a range that is configurable.
//! The overlay displays information for one instance of a HeatmapFields data object,
//! which may be formed of a number of component grids, each with a number of
//! field values to represent.
class HeatmapOverlay : public vespa::VaOverlay
{
public:
   HeatmapOverlay(const HeatmapFields& aFields, const UtQtGradientMap& aGradientMap);
   HeatmapOverlay(const HeatmapOverlay& aOther) = default;
   ~HeatmapOverlay() override                   = default;

   HeatmapOverlay* Clone() const override { return new HeatmapOverlay(*this); }

   //! Initialize this overlay.
   //!
   //! Setup the visual representation of the fields this object is representing.
   //! If there is any difficulty, this method will return false.
   //!
   //! \returns true if successfully initialized; false otherwise.
   bool Initialize() override;

   //! Set the opacity of the overlay.
   void SetOpacity(unsigned int aOpacity);

   //! Set the range of values for a field.
   //!
   //! This method will set the range of values of the field with the given @p aFieldIndex
   //! that will be mapped into the color gradient selected for this object. If @p aAuto is
   //! true, this will indicate that the ranges were computed by taking the minimum and
   //! maximum over the field data, rather than from the client.
   //!
   //! \param aFieldIndex  The field index for which the display ranges are being configured.
   //! \param aMin         The field value that should map into the lowest end of the selected gradient.
   //! \param aMax         The field value that should map into the highest end of the selected gradient.
   //! \param aAuto        Set to true to indicate that these values were computed automatically based on
   //                      the field data.
   void SetFieldRange(std::size_t aFieldIndex, float aMin, float aMax, bool aAuto = false);

   //! Set the range of values for a field automatically.
   //!
   //! This will compute and set the display values for the given @p aFieldIndex.
   //!
   //! \param aFieldIndex  The field index for which the display ranges should be automatically set.
   void SetAutoFieldRange(std::size_t aFieldIndex);

   //! Set the currently displayed field to the one with the given @p aFieldIndex.
   void SetCurrentField(std::size_t aFieldIndex);

   //! Set the currently displayed field to the one with the given @p aFieldName.
   void SetCurrentField(const std::string& aFieldName);

   //! Set the display colors for this overlay to those for the named @p aGradientSelection.
   void SetGradientSelection(const QString& aGradientSelection);

   //! Get the name of the data displayed by this heatmap.
   std::string GetHeatmapName() const { return mFieldsPtr->GetName(); }

   //! Returns the opacity of this heatmap.
   unsigned int GetOpacity() const { return mOpacity; }

   //! Returns if the field with the given @p aFieldIndex has a specified display range.
   bool HasFieldRange(std::size_t aFieldIndex);

   //! Returns the display range for the field with the given @p aFieldIndex.
   //!
   //! This returns the configured display range for the field with the given @p aFieldIndex,
   //! but only if the range has been set for that field. If the field's range has been
   //! previously set, then this will supply the relevant values via the input arguments, and
   //! return true. Otherwise, this will return false.
   //!
   //! \param aFieldIndex       The index of the field for which the display range is to be retrieved.
   //! \param aMin [out]        The field value that maps to the bottom of the selected gradient.
   //! \param aMax [out]        The field value that maps to the top of the selected gradient.
   //! \param aAutoRange [out]  Were the field display ranges set automatically.
   //! \returns                 true if the range data was returned via the input arguments; false otherwise.
   bool GetFieldRange(std::size_t aFieldIndex, float& aMin, float& aMax, bool& aAutoRange) const;

   //! Return the index of the currently displayed field.
   std::size_t GetCurrentField() const { return mCurrentField; }

   //! Return the set of names of the available fields.
   std::vector<std::string> GetFieldNames() const;

   //! Return the name of the currently selected field.
   std::string GetCurrentFieldName() const { return mFieldsPtr->GetFieldName(mCurrentField); }

   //! Return the type of the currently selected field.
   std::string GetCurrentFieldType() const { return mFieldsPtr->GetFieldType(mCurrentField); }

   //! Return the subtype of the currently selected field.
   std::string GetCurrentFieldSubtype() const { return mFieldsPtr->GetFieldSubtype(mCurrentField); }

   //! Return the units of the currently selected field.
   int GetCurrentFieldUnitType() const { return mFieldsPtr->GetFieldUnitType(mCurrentField); }

   //! Return the name of gradient selected that will be used to make field values into color.
   const QString& GetGradientSelection() const { return mGradientSelection; }

   //! Regenerate the texture data, usually as a response to the gradient selection changing.
   void RegenerateTexture();

   //! Regenerate the geometry.
   void RegenerateGeometry();

private:
   void UpdateDisplayRanges(float aMinValue, float aMaxValue);
   void UpdateDisplayOpacity();

   //! This drawable OSG node contains the data for a single component of the field data.
   class VisData : public osg::Geometry
   {
   public:
      VisData() = default;
      explicit VisData(std::size_t aComponentIndex)
         : mComponentIndex{aComponentIndex}
      {
      }
      ~VisData() = default;

      //! Initialize thie drawable OSG node.
      void Initialize();

      //! Set the current field for this node.
      //!
      //! Update the geometry and data represented by this node to be that for
      //! the given @p aFieldIndex or the given @p aFields. Typical use will only
      //! recompute the field values, but when needed the geometry can be
      //! recomputed as well.
      //!
      //! \param aFields         The set of fields from which to retrieve data.
      //! \param aFieldIndex     The index of the field to display with this drawable node.
      //! \param aViewerPtr      The viewer in which this drawable node will be displayed.
      //! \param aForceGeometry  Set to true to require that geometry be recomputed.
      void SetCurrentField(const HeatmapFields& aFields,
                           std::size_t          aFieldIndex,
                           vespa::VaViewer*     aViewerPtr,
                           bool                 aForceGeometry = false);

   private:
      void GenerateGeometry(const HeatmapFields& aFields, vespa::VaViewer* aViewerPtr);
      void SetDimensions(const HeatmapFields& aFields);
      void GenerateCurvedGrid(const HeatmapFields& aFields, vespa::VaCamera* aCameraPtr);
      void GenerateFlatGrid(const HeatmapFields& aFields, vespa::VaCamera* aCameraPtr);
      void GenerateVertices(const HeatmapFields& aFields, vespa::VaCamera* aCameraPtr);
      void GenerateStrips();
      void GenerateValues(const HeatmapFields& aFields, std::size_t aFieldIndex, vespa::VaViewer* aViewerPtr);

      std::size_t mComponentIndex;

      osg::ref_ptr<osg::Vec3Array>  mV_ArrayPtr{nullptr};
      osg::ref_ptr<osg::FloatArray> mValueArrayPtr{nullptr};

      double mDeltaLat{};
      double mDeltaLon{};

      double       mAltOffset{0.0};
      unsigned int mNumLatLines{1};
      unsigned int mNumLonLines{1};
      unsigned int mNumLatPerPatch{1};
      unsigned int mNumLonPerPatch{1};
      unsigned int mArraySize{0};
      unsigned int mArrayColSize{0};

      bool mGeometryGenerated{false};
   };

   struct RangeOptions
   {
      RangeOptions() = default;
      RangeOptions(float aMinValue, float aMaxValue, bool aAutoRange)
         : mMinValue{aMinValue}
         , mMaxValue{aMaxValue}
         , mAutoRange{aAutoRange}
      {
      }

      float mMinValue{};
      float mMaxValue{};
      bool  mAutoRange{true};
   };

   UtoRawShape*                       mShapePtr{nullptr};
   osg::ref_ptr<osg::MatrixTransform> mMatrixPtr{nullptr};
   osg::ref_ptr<osg::Geode>           mGeodePtr{nullptr};
   osg::ref_ptr<osg::Texture1D>       mTexturePtr{nullptr};
   std::vector<osg::ref_ptr<VisData>> mHeatmaps{}; //! The visual information for a set of fields.

   const HeatmapFields*                mFieldsPtr; //! The collection of sets of fields.
   unsigned int                        mOpacity{127};
   std::size_t                         mCurrentField{0}; //! The selected field for each set of fields.
   std::map<std::size_t, RangeOptions> mRanges{};
   const UtQtGradientMap&              mGradientMap;
   QString                             mGradientSelection{UtQtGradientMap::cDEFAULT_GRADIENT_KEY};
   bool                                mInitialized{false};
};

} // namespace CoverageOverlay

VA_DECLARE_OBJECT_TYPE(CoverageOverlay::HeatmapOverlay);

#endif // HEATMAPOVERLAY_HPP
