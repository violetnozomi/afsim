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

#include "HeatmapOverlay.hpp"

#include <QGradient>
#include <QImage>
#include <QLinearGradient>
#include <QPainter>
#include <osg/BlendFunc>
#include <osg/CullFace>
#include <osg/Depth>

#include "UtCast.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtQtGradientMap.hpp"
#include "UtoMemory.hpp"
#include "UtoShaders.hpp"
#include "VaCamera.hpp"
#include "VaViewer.hpp"

namespace
{

constexpr double       cMAX_DISTANCE_BETWEEN_LINES{0.25};
constexpr unsigned int cTEXTURE_SIZE{128};

} // namespace

namespace CoverageOverlay
{

HeatmapOverlay::HeatmapOverlay(const HeatmapFields& aFields, const UtQtGradientMap& aGradientMap)
   : VaOverlay(aFields.GetName(), cOVERLAY_WORLD, true)
   , mFieldsPtr(&aFields)
   , mGradientMap(aGradientMap)
{
   SetType<HeatmapOverlay>();
}

bool HeatmapOverlay::Initialize()
{
   UtoRawShape shape;
   mShapePtr = dynamic_cast<UtoRawShape*>(BindShape(GetName(), shape));
   if (!mShapePtr)
   {
      auto msg = ut::log::error() << "Unable to bind heatmap overlay.";
      msg.AddNote() << "Overlay: " << GetName();
   }

   if (mShapePtr)
   {
      mMatrixPtr = ut::osg::make_ref<osg::MatrixTransform>();
      mShapePtr->GetTree()->addChild(mMatrixPtr);
      mMatrixPtr->getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, true);
      mMatrixPtr->getOrCreateStateSet()->setMode(GL_LIGHTING, false);
      mMatrixPtr->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc{}, osg::StateAttribute::ON);
      mMatrixPtr->getOrCreateStateSet()->setRenderingHint(osg::StateSet::RenderingHint::TRANSPARENT_BIN);
      mMatrixPtr->getOrCreateStateSet()->setRenderBinDetails(std::numeric_limits<int>::max(), "DepthSortedBin");
      mMatrixPtr->getOrCreateStateSet()->setNestRenderBins(false);
      mMatrixPtr->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth{osg::Depth::Function::LESS, 0.0, 1.0, true});

      // Shader Program
      std::set<std::string> defs{};
      osg::Program*         programPtr =
         UtoShaders::UseProgramWithGeometry("heatmapoverlay.geom", "heatmapoverlay.vert", "heatmapoverlay.frag", defs);
      if (programPtr)
      {
         programPtr->addBindAttribLocation("position", 0);
         programPtr->addBindAttribLocation("fieldValue", 1);
         mShapePtr->SetProgram(programPtr, true);
      }
      else
      {
         return false;
      }

      // Initialize shader program uniforms.
      UpdateDisplayRanges(0.0f, 1.0f);
      UpdateDisplayOpacity();

      // Add new geometry node.
      mGeodePtr = ut::osg::make_ref<osg::Geode>();
      mMatrixPtr->addChild(mGeodePtr);

      // Create the Geometry below this node, one for each component of the HeatmapField.
      for (std::size_t i = 0; i < mFieldsPtr->GetNumComponents(); ++i)
      {
         mHeatmaps.emplace_back(new VisData{i});
         mHeatmaps[mHeatmaps.size() - 1]->Initialize();
         mGeodePtr->addDrawable(mHeatmaps[mHeatmaps.size() - 1]);
      }

      // Make sure all graphics data is available.
      RegenerateGeometry();
      RegenerateTexture();

      SetVisible(true);
      mInitialized = true;
   }
   return mInitialized;
}

void HeatmapOverlay::SetOpacity(unsigned int aOpacity)
{
   if (aOpacity == mOpacity || aOpacity > 255)
   {
      return;
   }
   mOpacity = aOpacity;
   UpdateDisplayOpacity();
}

void HeatmapOverlay::SetFieldRange(std::size_t aFieldIndex, float aMin, float aMax, bool aAuto)
{
   if (aMin > aMax)
   {
      std::swap(aMin, aMax);
   }
   mRanges[aFieldIndex] = RangeOptions{aMin, aMax, aAuto};
   if (aFieldIndex == mCurrentField)
   {
      UpdateDisplayRanges(aMin, aMax);
   }
}

void HeatmapOverlay::SetAutoFieldRange(std::size_t aFieldIndex)
{
   float minValue;
   float maxValue;
   mFieldsPtr->GetFieldValueRange(aFieldIndex, minValue, maxValue);
   SetFieldRange(aFieldIndex, minValue, maxValue, true);
}

void HeatmapOverlay::SetCurrentField(std::size_t aFieldIndex)
{
   if (mCurrentField != aFieldIndex)
   {
      mCurrentField = aFieldIndex;
      for (auto& heatmap : mHeatmaps)
      {
         heatmap->SetCurrentField(*mFieldsPtr, aFieldIndex, GetViewer());
         heatmap->dirtyDisplayList();
      }
   }
}

void HeatmapOverlay::SetCurrentField(const std::string& aFieldName)
{
   for (std::size_t i = 0; i < mFieldsPtr->GetNumFields(); ++i)
   {
      if (mFieldsPtr->GetFieldName(i) == aFieldName)
      {
         SetCurrentField(i);
         break;
      }
   }
}

void HeatmapOverlay::SetGradientSelection(const QString& aGradientSelection)
{
   if (mGradientSelection != aGradientSelection)
   {
      mGradientSelection = aGradientSelection;
      RegenerateTexture();
   }
}

bool HeatmapOverlay::HasFieldRange(std::size_t aFieldIndex)
{
   return mRanges.find(aFieldIndex) != mRanges.end();
}

bool HeatmapOverlay::GetFieldRange(std::size_t aFieldIndex, float& aMin, float& aMax, bool& aAutoRange) const
{
   bool retval{false};
   auto iter = mRanges.find(aFieldIndex);
   if (iter != mRanges.end())
   {
      aAutoRange = iter->second.mAutoRange;
      aMin       = iter->second.mMinValue;
      aMax       = iter->second.mMaxValue;
      retval     = true;
   }
   return retval;
}

std::vector<std::string> HeatmapOverlay::GetFieldNames() const
{
   std::vector<std::string> retval{};
   for (std::size_t i = 0; i < mFieldsPtr->GetNumFields(); ++i)
   {
      retval.emplace_back(mFieldsPtr->GetFieldName(i));
   }
   return retval;
}

void HeatmapOverlay::RegenerateTexture()
{
   // Paint the gradient onto a QImage, making use of Qt to compute the
   // intermediate colors.
   QImage          gradientImage{ut::cast_to_int(cTEXTURE_SIZE), 1, QImage::Format_RGB32};
   QPainter        painter{&gradientImage};
   QLinearGradient grad(0, 0, cTEXTURE_SIZE, 0);
   grad.setStops(mGradientMap.GradientStops(mGradientSelection));
   painter.fillRect(QRect(0, 0, cTEXTURE_SIZE, 1), grad);

   // Now create the osgImage from that
   osg::Image* imagePtr = new osg::Image{};
   imagePtr->allocateImage(cTEXTURE_SIZE, 1, 1, GL_RGB, GL_FLOAT);
   for (unsigned int s = 0; s < cTEXTURE_SIZE; ++s)
   {
      auto color = gradientImage.pixelColor(QPoint(s, 0));
      imagePtr->setColor(osg::Vec4(color.redF(), color.greenF(), color.blueF(), color.alphaF()), s);
   }

   // Create the texture as needed.
   if (!mTexturePtr)
   {
      mTexturePtr = ut::osg::make_ref<osg::Texture1D>();
      mTexturePtr->setFilter(osg::Texture1D::MIN_FILTER, osg::Texture1D::LINEAR);
      mTexturePtr->setFilter(osg::Texture1D::MAG_FILTER, osg::Texture1D::LINEAR);
      mTexturePtr->setWrap(osg::Texture1D::WRAP_S, osg::Texture1D::CLAMP_TO_EDGE);
      mTexturePtr->setWrap(osg::Texture1D::WRAP_T, osg::Texture1D::CLAMP_TO_EDGE);
   }

   // Set the texture data.
   mTexturePtr->setImage(imagePtr);

   mMatrixPtr->getOrCreateStateSet()->setTextureAttributeAndModes(0, mTexturePtr);
}

void HeatmapOverlay::RegenerateGeometry()
{
   auto cameraPtr = GetViewer()->GetCamera();

   osg::Matrix mat;
   mat.makeIdentity();
   if (cameraPtr->IsFlat())
   {
      mat.preMultTranslate(osg::Vec3(0.0f, 0.0f, 100.0f));
   }
   mMatrixPtr->setMatrix(mat);

   for (auto& heatmap : mHeatmaps)
   {
      heatmap->SetCurrentField(*mFieldsPtr, mCurrentField, GetViewer(), true);
   }
}

void HeatmapOverlay::UpdateDisplayRanges(float aMinValue, float aMaxValue)
{
   mMatrixPtr->getOrCreateStateSet()->getOrCreateUniform("minValue", osg::Uniform::FLOAT)->set(aMinValue);
   mMatrixPtr->getOrCreateStateSet()->getOrCreateUniform("maxValue", osg::Uniform::FLOAT)->set(aMaxValue);
}

void HeatmapOverlay::UpdateDisplayOpacity()
{
   mMatrixPtr->getOrCreateStateSet()->getOrCreateUniform("opacity", osg::Uniform::FLOAT)->set(static_cast<float>(mOpacity) / 255.0f);
}

void HeatmapOverlay::VisData::Initialize()
{
   mV_ArrayPtr    = ut::osg::make_ref<osg::Vec3Array>();
   mValueArrayPtr = ut::osg::make_ref<osg::FloatArray>();
   setVertexAttribArray(0, mV_ArrayPtr.get(), osg::Array::BIND_PER_VERTEX);
   setVertexAttribArray(1, mValueArrayPtr.get(), osg::Array::BIND_PER_VERTEX);
}

void HeatmapOverlay::VisData::SetCurrentField(const HeatmapFields& aFields,
                                              std::size_t          aFieldIndex,
                                              vespa::VaViewer*     aViewerPtr,
                                              bool                 aForceGeometry)
{
   if (aForceGeometry)
   {
      mGeometryGenerated = false;
   }
   GenerateGeometry(aFields, aViewerPtr);
   GenerateValues(aFields, aFieldIndex, aViewerPtr);
}

void HeatmapOverlay::VisData::GenerateGeometry(const HeatmapFields& aFields, vespa::VaViewer* aViewerPtr)
{
   if (!mGeometryGenerated)
   {
      removePrimitiveSet(0, getNumPrimitiveSets());
      mV_ArrayPtr->clear();

      SetDimensions(aFields);

      auto cameraPtr = aViewerPtr->GetCamera();
      if (aViewerPtr->GetMapProjection().projection == UtoMapProjection::Geocentric)
      {
         GenerateCurvedGrid(aFields, cameraPtr);
      }
      else
      {
         GenerateFlatGrid(aFields, cameraPtr);
      }

      mGeometryGenerated = true;
   }
}

void HeatmapOverlay::VisData::SetDimensions(const HeatmapFields& aFields)
{
   mNumLatLines = aFields.GetNumLats(mComponentIndex);
   mNumLonLines = aFields.GetNumLons(mComponentIndex);

   // Get the maximum delta in lat and lon
   float latPatchDelta{std::numeric_limits<float>::lowest()};
   float lonPatchDelta{std::numeric_limits<float>::lowest()};
   for (unsigned int i = 0; i < mNumLatLines - 1; ++i)
   {
      for (unsigned int j = 0; j < mNumLonLines - 1; ++j)
      {
         Patch patch{aFields, mComponentIndex, i, j};
         latPatchDelta = std::max(latPatchDelta, patch.GetDeltaLat());
         lonPatchDelta = std::max(lonPatchDelta, patch.GetDeltaLon());
      }
   }

   mNumLatPerPatch = static_cast<unsigned int>(std::ceil(latPatchDelta / cMAX_DISTANCE_BETWEEN_LINES)) + 1;
   mDeltaLat       = latPatchDelta / mNumLatPerPatch;
   mNumLonPerPatch = static_cast<unsigned int>(std::ceil(lonPatchDelta / cMAX_DISTANCE_BETWEEN_LINES)) + 1;
   mDeltaLon       = lonPatchDelta / mNumLonPerPatch;

   mArrayColSize = (mNumLatLines - 1) * (mNumLatPerPatch - 1) + 1;
   mArraySize    = mArrayColSize * ((mNumLonLines - 1) * (mNumLonPerPatch - 1) + 1);
}

void HeatmapOverlay::VisData::GenerateCurvedGrid(const HeatmapFields& aFields, vespa::VaCamera* aCameraPtr)
{
   // Compute the altitude offset so that the geometry will be tangent to the surface.
   double zero[3];
   aCameraPtr->ConvertLLAToXYZ(zero, 0.0, 0.0, 0.0);
   double r     = sqrt(zero[0] * zero[0] + zero[1] * zero[1] + zero[2] * zero[2]);
   double theta = sqrt(mDeltaLat * mDeltaLat + mDeltaLon * mDeltaLon) * UtMath::cRAD_PER_DEG;
   mAltOffset   = r * (1.0 / cos(theta) - 1.0);

   GenerateVertices(aFields, aCameraPtr);
   GenerateStrips();
}

void HeatmapOverlay::VisData::GenerateFlatGrid(const HeatmapFields& aFields, vespa::VaCamera* aCameraPtr)
{
   mAltOffset = 0.0;
   GenerateVertices(aFields, aCameraPtr);
   GenerateStrips();
}

void HeatmapOverlay::VisData::GenerateVertices(const HeatmapFields& aFields, vespa::VaCamera* aCameraPtr)
{
   mV_ArrayPtr->clear();
   mV_ArrayPtr->resize(mArraySize);

   for (unsigned int nx = 0; nx < mNumLonLines; ++nx)
   {
      unsigned int end = (nx == mNumLonLines - 1) ? 1 : mNumLonPerPatch - 1;
      for (unsigned int ny = 0; ny < mNumLatLines; ++ny)
      {
         unsigned int last = (ny == mNumLatLines - 1) ? 1 : mNumLatPerPatch - 1;

         Patch p{aFields, mComponentIndex, ny, nx};
         p.SetNumDivisions(mNumLatPerPatch, mNumLonPerPatch);

         for (unsigned int mx = 0; mx < end; ++mx)
         {
            for (unsigned int my = 0; my < last; ++my)
            {
               // Generate geometry
               float  lat = p.GetLatitude(my, mx);
               float  lon = p.GetLongitude(my, mx);
               float  alt = p.GetAltitude(my, mx);
               double vec[3];
               aCameraPtr->ConvertLLAToXYZ(vec, lat, lon, alt + mAltOffset);

               // get the index in the array
               unsigned int index_x = nx * (mNumLonPerPatch - 1) + mx;
               unsigned int index_y = ny * (mNumLatPerPatch - 1) + my;
               unsigned int index   = index_x * mArrayColSize + index_y;

               mV_ArrayPtr->at(index) =
                  osg::Vec3{static_cast<float>(vec[0]), static_cast<float>(vec[1]), static_cast<float>(vec[2])};
            }
         }
      }
   }
}

void HeatmapOverlay::VisData::GenerateStrips()
{
   if (mArraySize == 0)
   {
      return;
   }

   unsigned int end = mV_ArrayPtr->size() / mArrayColSize;
   for (unsigned int i = 1; i < end; ++i)
   {
      osg::DrawElementsUInt* stripPtr = new osg::DrawElementsUInt(GL_TRIANGLE_STRIP);
      for (unsigned int j = 0; j < mArrayColSize; ++j)
      {
         stripPtr->push_back(i * mArrayColSize + j);
         stripPtr->push_back((i - 1) * mArrayColSize + j);
      }
      addPrimitiveSet(stripPtr);
   }
}

void HeatmapOverlay::VisData::GenerateValues(const HeatmapFields& aFields, std::size_t aFieldIndex, vespa::VaViewer* aViewerPtr)
{
   mValueArrayPtr->clear();
   mValueArrayPtr->resize(mArraySize);

   for (unsigned int nx = 0; nx < mNumLonLines; ++nx)
   {
      unsigned int end = (nx == mNumLonLines - 1) ? 1 : mNumLonPerPatch - 1;
      for (unsigned int ny = 0; ny < mNumLatLines; ++ny)
      {
         unsigned int last = (ny == mNumLatLines - 1) ? 1 : mNumLatPerPatch - 1;

         Patch p{aFields, mComponentIndex, ny, nx};
         p.SetNumDivisions(mNumLatPerPatch, mNumLonPerPatch);

         for (unsigned int mx = 0; mx < end; ++mx)
         {
            for (unsigned int my = 0; my < last; ++my)
            {
               unsigned int index_x = nx * (mNumLonPerPatch - 1) + mx;
               unsigned int index_y = ny * (mNumLatPerPatch - 1) + my;
               unsigned int index   = index_x * mArrayColSize + index_y;

               mValueArrayPtr->at(index) = p.GetField(my, mx, aFieldIndex);
            }
         }
      }
   }
}

} // namespace CoverageOverlay
