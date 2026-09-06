// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfOverlayWsfDraw.hpp"

#include <algorithm>
#include <assert.h>
#include <cstdint>
#include <ios>
#include <iostream>
#include <string>

#include <QColor>
#include <QFont>

#include "GenI.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtStringUtil.hpp"
#include "Uto.hpp"
#include "UtoPolyLineShape.hpp"
#include "UtoRawShape.hpp"
#include "UtoShaderUtilities.hpp"
#include "UtoShaders.hpp"
#include "VaCamera.hpp"
#include "VaDecoratorNode.hpp"
#include "VaEntity.hpp"
#include "VaEnvironment.hpp"
#include "VaModelDatabase.hpp"
#include "VaObserver.hpp"
#include "VaScenario.hpp"
#include "VaTimeManager.hpp"
#include "VaViewer.hpp"
#include "VaViewerManager.hpp"

double wkf::OverlayWsfDraw::cFOREVER = 1.0E20;

void wkf::OverlayWsfDrawBase::ConvertLocalOrientation(UtEntity& aEntity, UtoOrientation& aOri)
{
   if (vespa::VaPosition::GetSceneProjectionType(GetViewer()->GetUniqueId()) == UtoMapProjection::Geocentric)
   {
      aEntity.GetOrientationWCS(aOri.yaw, aOri.pitch, aOri.roll);
   }
   else
   {
      aEntity.GetOrientationNED(aOri.yaw, aOri.pitch, aOri.roll);
      aOri.yaw += UtMath::cPI_OVER_2;
   }
   aOri.yaw *= UtMath::cDEG_PER_RAD;
   aOri.pitch *= UtMath::cDEG_PER_RAD;
   aOri.roll *= UtMath::cDEG_PER_RAD;
}


void wkf::OverlayWsfDrawBase::GetCurrentLocationECEF(const OverlayWsfDrawBase::VertexObject& aVertex,
                                                     double                                  aLocationWCS[3])
{
   if (aVertex.mVertexType == OverlayWsfDrawBase::cABSOLUTE_WCS)
   {
      aLocationWCS[0] = aVertex.mPosition[0];
      aLocationWCS[1] = aVertex.mPosition[1];
      aLocationWCS[2] = aVertex.mPosition[2];
   }
   else if (aVertex.mVertexType == OverlayWsfDrawBase::cRELATIVE_ZERO)
   {
      aVertex.mReferenceEntityPtr->GetUtoEntity()->BodyCF().GetLocationWCS(aLocationWCS);
   }
   else if (aVertex.mVertexType == OverlayWsfDrawBase::cRELATIVE_ECS)
   {
      UtEntity& refEntity = aVertex.mReferenceEntityPtr->GetUtoEntity()->BodyCF();
      refEntity.ConvertECSToWCS(aVertex.mPosition.GetData(), aLocationWCS);
   }
   else if (aVertex.mVertexType == OverlayWsfDrawBase::cRELATIVE_NED)
   {
      UtEntity& refEntity = aVertex.mReferenceEntityPtr->GetUtoEntity()->BodyCF();
      refEntity.ConvertNEDToWCS(aVertex.mPosition.GetData(), aLocationWCS);
   }
   else if (aVertex.mVertexType == OverlayWsfDrawBase::cABSOLUTE_SCREEN)
   {
      vespa::VaViewer* viewerPtr = vespa::VaEnvironment::Instance().GetViewerManager()->CurrentViewer();
      if (viewerPtr)
      {
         double tx = 0.0;
         double ty = 0.0;
         double tz = 0.0;

         if (aVertex.mReferenceEntityPtr)
         {
            viewerPtr->WorldToScreen(aVertex.mReferenceEntityPtr->GetPosition(), tx, ty, tz);
         }

         unsigned int w = 0;
         unsigned int h = 0;
         viewerPtr->GetCamera()->GetViewport(w, h);
         double      x = (aVertex.mPosition[0] * static_cast<double>(w)) + tx;
         double      y = (aVertex.mPosition[1] * static_cast<double>(h)) + ty;
         UtoPosition pos(x, y, 0.05);
         if (viewerPtr->WindowToWorld(pos))
         {
            aLocationWCS[0] = pos[0];
            aLocationWCS[1] = pos[1];
            aLocationWCS[2] = pos[2];
            vespa::VaPosition::GetSceneProjection(GetViewer()->GetUniqueId())->ConvertFromECEF(aLocationWCS);
         }
      }
   }
}

void wkf::OverlayWsfDrawBase::GetCurrentLocation(const OverlayWsfDrawBase::VertexObject& aVertex, double aLocationWCS[3])
{
   GetCurrentLocationECEF(aVertex, aLocationWCS);
   vespa::VaPosition::GetSceneProjection(GetViewer()->GetUniqueId())->ConvertFromECEF(aLocationWCS);
}

wkf::OverlayWsfDraw::Extended& wkf::OverlayWsfDraw::Element::GetExtended()
{
   if (!mExtended)
      mExtended = new Extended();
   return *mExtended;
}

QString wkf::OverlayWsfDraw::Element::GetType() const
{
   switch (mElementType & ~cRELATIVE_BIT)
   {
   case (cPOINT):
      return "Point";
   case (cLINE):
      return "Line";
   case (cICON):
      return "Icon";
   case (cELLIPSE):
      return "Ellipse";
   case (cELLIPSOID):
      return "Ellipsoid";
   case (cQUADRILATERAL):
      return "Quadrilateral";
   case (cTEXT):
      return "Text";
   case (cTIMER):
      return "Timer";
   default:
      return "Unknown";
   }
}

wkf::OverlayWsfDrawBase::OverlayWsfDrawBase()
   : vespa::VaOverlay("wsfdraw_overlay", cOVERLAY_WORLD)
{
   SetType<OverlayWsfDrawBase>();
}

wkf::OverlayWsfDrawBase::OverlayWsfDrawBase(const std::string& aName)
   : vespa::VaOverlay(aName, cOVERLAY_WORLD)
{
   SetType<OverlayWsfDrawBase>();
}

void wkf::OverlayWsfDrawBase::UpdateDynamicPoints()
{
   for (const auto& line : mDynamicLines)
   {
      UtoPosition pos1, pos2;
      GetCurrentLocation(line.mPoint1, &pos1.x());
      GetCurrentLocation(line.mPoint2, &pos2.x());
      LineShape* shapePtr = (LineShape*)line.mShapePtr;
      shapePtr->mPolyShapePtr->SetAt(line.GetIndex() * 2, pos1);
      shapePtr->mPolyShapePtr->SetAt(line.GetIndex() * 2 + 1, pos2);
   }

   for (const auto& quad : mDynamicQuadrilaterals)
   {
      UtoPosition pos1, pos2, pos3, pos4;
      GetCurrentLocation(quad.mPoint1, &pos1.x());
      GetCurrentLocation(quad.mPoint2, &pos2.x());
      GetCurrentLocation(quad.mPoint3, &pos3.x());
      GetCurrentLocation(quad.mPoint4, &pos4.x());
      QuadrilateralShape* shapePtr = (QuadrilateralShape*)quad.mShapePtr;
      shapePtr->mPolygonShapePtr->SetAt(0, pos1);
      shapePtr->mPolygonShapePtr->SetAt(1, pos2);
      shapePtr->mPolygonShapePtr->SetAt(2, pos3);
      shapePtr->mPolygonShapePtr->SetAt(3, pos4);
   }

   for (const auto& point : mDynamicPoints)
   {
      if (point.mShapePtr->IsPoint())
      {
         PointShape* pointPtr = (PointShape*)point.mShapePtr;
         UtoPosition pos1;
         GetCurrentLocation(point.mPoint, &pos1.x());
         pointPtr->mPointShapePtr->SetAt(point.GetIndex(), pos1);
         ;
      }
      else if (point.mShapePtr->IsIcon())
      {
         IconShape*  iconPtr = (IconShape*)point.mShapePtr;
         UtoPosition pos1;
         GetCurrentLocation(point.mPoint, &pos1.x());
         iconPtr->mModelPtr->SetPosition(pos1);
         iconPtr->mModelPtr->SetUniform3f("UTO_EntityPosition", pos1.x(), pos1.y(), pos1.z());
         if (point.mPoint.mReferenceEntityPtr != nullptr)
         {
            UtEntity tmp;
            tmp.SetLocationWCS(pos1.ptr());
            tmp.SetOrientationNED(point.mPoint.mReferenceEntityPtr->GetHeading() * UtMath::cRAD_PER_DEG, 0, 0);
            UtoOrientation ori;
            ConvertLocalOrientation(tmp, ori);
            iconPtr->mModelPtr->SetOrientation(ori);
         }
      }
      else if (point.mShapePtr->IsEllipse())
      {
         EllipseShape* ellipsePtr = (EllipseShape*)point.mShapePtr;
         UtoPosition   pos1;
         GetCurrentLocation(point.mPoint, &pos1.x());
         ellipsePtr->mEllipseShapePtr->SetPosition(pos1);
         if (point.mPoint.mReferenceEntityPtr != nullptr)
         {
            UtEntity tmp;
            tmp.SetLocationWCS(pos1.ptr());
            tmp.SetOrientationNED(point.mPoint.mReferenceEntityPtr->GetHeading() * UtMath::cRAD_PER_DEG, 0, 0);
            UtoOrientation ori;
            ConvertLocalOrientation(tmp, ori);
            ellipsePtr->mEllipseShapePtr->SetOrientation(ori);
         }
      }
      else if (point.mShapePtr->IsEllipsoid())
      {
         EllipsoidShape* ellipsoidPtr = (EllipsoidShape*)point.mShapePtr;
         UtoPosition     pos1;
         GetCurrentLocation(point.mPoint, &pos1.x());
         ellipsoidPtr->mEllipsoidShapePtr->SetPosition(pos1);
         if (point.mPoint.mReferenceEntityPtr != nullptr)
         {
            UtEntity tmp;
            tmp.SetLocationWCS(pos1.ptr());
            tmp.SetOrientationNED(point.mPoint.mReferenceEntityPtr->GetHeading() * UtMath::cRAD_PER_DEG, 0, 0);
            UtoOrientation ori;
            ConvertLocalOrientation(tmp, ori);
            ellipsoidPtr->mEllipsoidShapePtr->SetOrientation(ori);
         }
      }
      else if (point.mShapePtr->IsText())
      {
         TextShape*  textPtr = (TextShape*)point.mShapePtr;
         UtoPosition pos1;
         GetCurrentLocation(point.mPoint, &pos1.x());
         textPtr->mTextShapePtr->SetPosition(pos1);
      }
      else if (point.mShapePtr->IsTimer())
      {
         TimerShape* timerPtr = (TimerShape*)point.mShapePtr;
         UtoPosition pos1;
         GetCurrentLocation(point.mPoint, &pos1.x());
         timerPtr->mTimerShapePtr->SetPosition(pos1);
      }
   }
}

wkf::OverlayWsfDraw::ShapeIndex wkf::OverlayWsfDrawBase::GetIndex(const ShapeIndexBase& aBaseIndex)
{
   ShapeIndex ind;
   (ShapeIndexBase&)ind = aBaseIndex;
   switch (aBaseIndex.mShapePtr->mShapeType & ~cRELATIVE_BIT)
   {
   case cPOINT:
      ind.mUserId = ((PointShape*)aBaseIndex.mShapePtr)->mUserIndices[aBaseIndex.GetIndex()];
      break;
   case cLINE:
      ind.mUserId = ((LineShape*)aBaseIndex.mShapePtr)->mUserIndices[aBaseIndex.GetIndex()];
      break;
   case cICON:
      ind.mUserId = ((IconShape*)aBaseIndex.mShapePtr)->mUserIndex;
      break;
   case cELLIPSE:
      ind.mUserId = ((EllipseShape*)aBaseIndex.mShapePtr)->mUserIndex;
      break;
   }
   return ind;
}

void wkf::OverlayWsfDrawBase::ReindexP(const ShapeIndexBase& aOldIndex, const ShapeIndexBase& aNewIndex)
{
   if (!aOldIndex.mShapePtr->IsRelative())
      return;
   if (aOldIndex.mShapePtr->IsLine())
   {
      mDynamicLines.erase(aNewIndex);
      std::set<LinePosition>::iterator iter = mDynamicLines.find(aOldIndex);
      if (iter != mDynamicLines.end())
      {
         LinePosition newPos = *iter;
         newPos.SetIndex(aNewIndex.GetIndex());
         mDynamicLines.erase(iter);
         mDynamicLines.insert(newPos);
      }
      else
      {
         assert(false);
      }
   }
   else
   {
      mDynamicPoints.erase(aNewIndex);
      std::set<PointPosition>::iterator iter = mDynamicPoints.find(aOldIndex);
      if (iter != mDynamicPoints.end())
      {
         PointPosition newPos = *iter;
         newPos.SetIndex(aNewIndex.GetIndex());
         mDynamicPoints.erase(iter);
         mDynamicPoints.insert(newPos);
      }
      else
      {
         assert(false);
      }
   }
}

void wkf::OverlayWsfDrawBase::AddLogarithmicDepthBuffer(UtoShape* aShape)
{
   std::set<std::string> defset;
   osg::Program*         shader = UtoShaders::UseProgram("VaOverlayDraw.vert", "VaOverlayDraw.frag", defset);
   UtoColor              color  = aShape->Color();
   aShape->SetUniform4f("staticColor", color.RedF(), color.GreenF(), color.BlueF(), color.AlphaF());
   aShape->SetProgram(shader, true);
}

void wkf::OverlayWsfDrawBase::RemovedIndexP(const ShapeIndexBase& aIndex)
{
   if (!aIndex.mShapePtr->IsRelative())
      return;
   if (aIndex.mShapePtr->IsLine())
   {
      mDynamicLines.erase(aIndex);
   }
   else if (aIndex.mShapePtr->IsQuadrilateral())
   {
      mDynamicQuadrilaterals.erase(aIndex);
   }
   else
   {
      mDynamicPoints.erase(aIndex);
   }
}

wkf::OverlayWsfDrawBase::ShapeIndex wkf::OverlayWsfDrawBase::AddPoint(int           aUserId,
                                                                      VertexObject& aPoint,
                                                                      int           aPointSize,
                                                                      unsigned char aRed,
                                                                      unsigned char aGreen,
                                                                      unsigned char aBlue,
                                                                      unsigned char aAlpha /*=255*/)
{
   bool               relative = aPoint.IsRelative();
   ShapeIndex         index;
   PointConfiguration config;
   config.mPointSize      = aPointSize | (relative ? cRELATIVE_BIT : 0);
   config.mColor          = (int(aRed) << 24) | (int(aGreen) << 16) | (int(aBlue) << 8) | int(aAlpha);
   PointShape& pointShape = mPointShapes[config];
   if (pointShape.mPointShapePtr == nullptr)
   {
      UtoPointListShape shape;
      shape.SetPointSize(aPointSize);
      shape.SetPointSmooth(aPointSize > 1);
      shape.SetReferenceFrame(UtoShape::cFRAME_WORLD);
      shape.SetColor(UtoColor(aRed, aGreen, aBlue, aAlpha));
      std::stringstream ss;
      ss << "post_process_draw_point_" << (int)config.mColor << "_" << config.mPointSize;
      pointShape.mPointShapePtr = dynamic_cast<UtoPointListShape*>(BindShape(ss.str(), shape));
      pointShape.mConfiguration = config;
      if (relative)
      {
         pointShape.mShapeType |= cRELATIVE_BIT;
      }
   }

   index.SetIndex(pointShape.mPointShapePtr->Size());
   assert((int)pointShape.mUserIndices.size() == pointShape.mPointShapePtr->Size());
   index.mShapePtr = &pointShape;
   index.mUserId   = aUserId;
   UtoPosition pos;
   GetCurrentLocation(aPoint, pos.ptr());
   pointShape.mPointShapePtr->Insert(index.GetIndex(), pos);
   pointShape.mUserIndices.push_back(aUserId);

   PointPosition pointPos(index);
   pointPos.mPoint = aPoint;
   if (relative)
   {
      mDynamicPoints.insert(pointPos);
   }
   assert((int)pointShape.mUserIndices.size() == pointShape.mPointShapePtr->Size());
   AddLogarithmicDepthBuffer(pointShape.mPointShapePtr);
   return index;
}

void wkf::OverlayWsfDrawBase::DeletePoint(const ShapeIndexBase& aShapeIndex)
{
   PointShape*        pointShape = (PointShape*)aShapeIndex.mShapePtr;
   UtoPointListShape* shapePtr   = pointShape->mPointShapePtr;
   int                shapeSize  = shapePtr->Size();
   assert((int)pointShape->mUserIndices.size() > aShapeIndex.GetIndex());
   assert((int)pointShape->mUserIndices.size() == shapeSize);
   ShapeIndex oldIndex;
   oldIndex.mUserId = pointShape->mUserIndices.back();
   oldIndex.SetIndex(shapeSize - 1);
   oldIndex.mShapePtr = pointShape;
   if (shapeSize - 1 == aShapeIndex.GetIndex())
   {
      // pop back
      RemovedIndexP(oldIndex);
      RemovedIndex(oldIndex);
      shapePtr->Remove(aShapeIndex.GetIndex());
      pointShape->mUserIndices.pop_back();
      assert((int)pointShape->mUserIndices.size() == shapeSize - 1);
   }
   else
   {
      // Swap and pop back
      shapePtr->SetAt(aShapeIndex.GetIndex(), shapePtr->At(shapeSize - 1));
      ShapeIndex newIndex;
      (ShapeIndexBase&)newIndex = aShapeIndex;
      newIndex.mUserId          = pointShape->mUserIndices[aShapeIndex.GetIndex()];
      ReindexP(oldIndex, newIndex);
      Reindex(oldIndex, newIndex);
      shapePtr->Remove(shapeSize - 1);
      pointShape->mUserIndices[aShapeIndex.GetIndex()] = newIndex.mUserId;
      pointShape->mUserIndices.pop_back();
      assert((int)pointShape->mUserIndices.size() == shapeSize - 1);
   }
}

wkf::OverlayWsfDrawBase::ShapeIndex wkf::OverlayWsfDrawBase::AddLine(int           aUserId,
                                                                     VertexObject& aPoint1,
                                                                     VertexObject& aPoint2,
                                                                     unsigned char aLineSize,
                                                                     unsigned char aLineStyle,
                                                                     unsigned char aRed,
                                                                     unsigned char aGreen,
                                                                     unsigned char aBlue,
                                                                     unsigned char aAlpha /*=255*/)
{
   ShapeIndex        index;
   LineConfiguration config;
   bool              relative = (aPoint1.IsRelative() || aPoint2.IsRelative());
   config.mLineStyle          = (aLineSize | (aLineStyle << 8)) | (relative ? cRELATIVE_BIT : 0);
   config.mColor              = (int(aRed) << 24) | (int(aGreen) << 16) | (int(aBlue) << 8) | int(aAlpha);
   LineShape& lineShape       = mLineShapes[config];
   if (lineShape.mPolyShapePtr == nullptr)
   {
      UtoPolyLineShape shape;
      shape.SetAggregation(UtoPolyLineShape::LINES);
      shape.SetReferenceFrame(UtoShape::cFRAME_WORLD);

      shape.SetWidth(aLineSize);
      shape.SetColor(UtoColor(aRed, aGreen, aBlue, aAlpha));
      int pattern = aLineStyle;
      switch (pattern)
      {
      case 0:
         pattern = UtoPolyLineShape::DASHED;
         break;
      case 1:
         pattern = UtoPolyLineShape::LONG_DASHED;
         break;
      case 2:
         pattern = UtoPolyLineShape::DOTTED;
         break;
      case 3:
         pattern = UtoPolyLineShape::DOTTED2;
         break;
      case 4:
         pattern = UtoPolyLineShape::DASH_DOT;
         break;
      case 5:
         pattern = UtoPolyLineShape::DASH_DOT2;
         break;
      case 6:
      default:
         pattern = UtoPolyLineShape::SOLID;
      }
      shape.SetPattern(pattern);
      std::stringstream ss;
      ss << "post_process_draw_line_" << (int)config.mColor << "_" << (int)config.mLineStyle;
      lineShape.mPolyShapePtr  = dynamic_cast<UtoPolyLineShape*>(BindShape(ss.str(), shape));
      lineShape.mConfiguration = config;
      if (relative)
      {
         lineShape.mShapeType |= cRELATIVE_BIT;
      }
   }

   index.SetIndex(lineShape.mPolyShapePtr->Size() / 2);
   index.mShapePtr = &lineShape;
   index.mUserId   = aUserId;
   UtoPosition pos1, pos2;
   GetCurrentLocation(aPoint1, &pos1.x());
   GetCurrentLocation(aPoint2, &pos2.x());
   lineShape.mPolyShapePtr->Insert(index.GetIndex() * 2, pos1);
   lineShape.mPolyShapePtr->Insert(index.GetIndex() * 2 + 1, pos2);
   lineShape.mUserIndices.push_back(aUserId);

   if (relative)
   {
      LinePosition linePos(index);
      linePos.mPoint1 = aPoint1;
      linePos.mPoint2 = aPoint2;
      mDynamicLines.insert(linePos);
   }
   AddLogarithmicDepthBuffer(lineShape.mPolyShapePtr);
   return index;
}

void wkf::OverlayWsfDrawBase::DeleteLine(const ShapeIndexBase& aShapeIndex)
{
   LineShape*        lineShape = (LineShape*)aShapeIndex.mShapePtr;
   UtoPolyLineShape* shapePtr  = lineShape->mPolyShapePtr;
   int               shapeSize = shapePtr->Size();

   assert((int)lineShape->mUserIndices.size() > aShapeIndex.GetIndex());
   assert((int)lineShape->mUserIndices.size() == shapeSize / 2);

   ShapeIndex oldIndex;
   oldIndex.mUserId = lineShape->mUserIndices.back();
   oldIndex.SetIndex(shapeSize / 2 - 1);
   oldIndex.mShapePtr = lineShape;
   if (shapeSize == aShapeIndex.GetIndex() * 2 + 2)
   {
      // pop back
      RemovedIndexP(oldIndex);
      RemovedIndex(oldIndex);
      shapePtr->Remove(aShapeIndex.GetIndex() * 2 + 1);
      shapePtr->Remove(aShapeIndex.GetIndex() * 2);
      lineShape->mUserIndices.pop_back();
      assert((int)lineShape->mUserIndices.size() == shapeSize / 2 - 1);
   }
   else
   {
      // Swap and pop back
      shapePtr->SetAt(aShapeIndex.GetIndex() * 2 + 1, shapePtr->At(shapeSize - 1));
      shapePtr->SetAt(aShapeIndex.GetIndex() * 2, shapePtr->At(shapeSize - 2));
      ShapeIndex newIndex;
      (ShapeIndexBase&)newIndex = aShapeIndex;
      newIndex.mUserId          = lineShape->mUserIndices[aShapeIndex.GetIndex()];
      ReindexP(oldIndex, newIndex);
      Reindex(oldIndex, newIndex);
      shapePtr->Remove(shapeSize - 1);
      shapePtr->Remove(shapeSize - 2);
      lineShape->mUserIndices[aShapeIndex.GetIndex()] = newIndex.mUserId;
      lineShape->mUserIndices.pop_back();
   }
}

wkf::OverlayWsfDrawBase::ShapeIndex wkf::OverlayWsfDrawBase::AddIcon(int           aUserId,
                                                                     VertexObject& aPoint,
                                                                     float         aHeading,
                                                                     std::string   aIconName,
                                                                     unsigned char aRed,
                                                                     unsigned char aGreen,
                                                                     unsigned char aBlue,
                                                                     unsigned char aAlpha /*=255*/)
{
   bool       relative = aPoint.IsRelative();
   ShapeIndex index;
   IconShape& iconShape = *new IconShape(relative);
   mIconShapes.insert(&iconShape);


   static unsigned int uid = 0;
   std::stringstream   ss;
   ss << "post_process_draw_icon_" << aIconName << "_" << std::to_string(uid++);
   iconShape.mName = ss.str();

   UtoRawShape rawShape;
   UtoRawShape* shapePtr = dynamic_cast<UtoRawShape*>(BindShape(iconShape.mName, rawShape)); // todo, include a name or id
   shapePtr->SetReferenceFrame(UtoShape::cFRAME_ENTITY);
   shapePtr->GetRoot()->getOrCreateStateSet()->addUniform(
      new osg::Uniform("VA_TeamColor", osg::Vec4(aRed, aGreen, aBlue, aAlpha) / 255.0));
   iconShape.mModelPtr = shapePtr;

   if (iconShape.mDB_ModelRefPtr.valid())
   {
      shapePtr->GetRoot()->removeChild(0, 1);
      vespa::VaEnvironment::Instance().GetModelDatabase()->ReleaseModel(aIconName, iconShape.mDB_ModelRefPtr, GetViewer());
   }
   iconShape.mDB_ModelRefPtr =
      vespa::VaEnvironment::Instance().GetModelDatabase()->RequestSharedModel(aIconName, GetViewer());

   assert(iconShape.mDB_ModelRefPtr.valid());
   shapePtr->GetRoot()->addChild(iconShape.mDB_ModelRefPtr.get());
   shapePtr->GetRoot()->getOrCreateStateSet()->getAttributeList();

   iconShape.mUserIndex = aUserId;
   iconShape.mShapeType |= (aPoint.IsRelative() ? cRELATIVE_BIT : 0);
   iconShape.mIcon = aIconName;

   index.mShapePtr = &iconShape;
   index.mUserId   = aUserId;

   UtoPosition pos;
   UtEntity    tmp;
   GetCurrentLocationECEF(aPoint, pos.ptr());
   tmp.SetLocationWCS(pos.ptr());
   tmp.SetOrientationNED(aHeading * UtMath::cRAD_PER_DEG, 0, 0);

   UtoOrientation ori;
   ConvertLocalOrientation(tmp, ori);
   vespa::VaPosition::GetSceneProjection(GetViewer()->GetUniqueId())->ConvertFromECEF(pos.ptr());

   iconShape.mModelPtr->SetPosition(pos);
   iconShape.mModelPtr->SetUniform3f("UTO_EntityPosition", pos.x(), pos.y(), pos.z());
   iconShape.mModelPtr->SetOrientation(ori);

   // Note: When there is a large distance between the displayed model and tethered platform (such as in a space
   // spatial_domain), there are scaling issues that make the model difficult/impossible to see
   //       Additionally, even if SetColor is called, there is no color applied to the model, but this is likely a
   //       design choice as the tethered platform also exhibits the same behavior. No additional shader is required,
   //       since the model node obtained from VaModelDataBase should already have a model shader program applied to it

   if (iconShape.IsRelative())
   {
      PointPosition pointPos(index);
      pointPos.mPoint = aPoint;
      mDynamicPoints.insert(pointPos);
   }
   return index;
}

void wkf::OverlayWsfDrawBase::DeleteIcon(const ShapeIndexBase& aShapeIndex)
{
   IconShape* pointShape = (IconShape*)aShapeIndex.mShapePtr;
   ShapeIndex ind;
   (ShapeIndexBase&)ind = aShapeIndex;
   ind.mUserId          = pointShape->mUserIndex;
   RemovedIndexP(ind);
   RemovedIndex(ind);

   pointShape->mModelPtr->GetRoot()->removeChild(0, 1);
   vespa::VaEnvironment::Instance().GetModelDatabase()->ReleaseModel(pointShape->mIcon,
                                                                     pointShape->mDB_ModelRefPtr,
                                                                     GetViewer());

   UnbindShape(pointShape->mName);
   mIconShapes.erase(pointShape);
   delete pointShape;
}

wkf::OverlayWsfDrawBase::ShapeIndex wkf::OverlayWsfDrawBase::AddEllipse(int           aUserId,
                                                                        VertexObject& aPoint,
                                                                        float         aHeading,
                                                                        float         aAxisA,
                                                                        float         aAxisB,
                                                                        unsigned char aLineSize,
                                                                        unsigned char aLineStyle,
                                                                        unsigned char aEllipseMode,
                                                                        unsigned char aRed,
                                                                        unsigned char aGreen,
                                                                        unsigned char aBlue,
                                                                        unsigned char aAlpha /*=255*/)
{
   bool          relative     = aPoint.IsRelative();
   EllipseShape& ellipseShape = *new EllipseShape(relative);
   ellipseShape.mUserIndex    = aUserId;
   mEllipseShapes.insert(&ellipseShape);

   UtoEllipseShape shape;
   shape.SetRadii(aAxisA, aAxisB);
   shape.SetSliceAngle(10);
   shape.SetLineWidth(aLineSize);
   shape.SetColor(UtoColor(aRed, aGreen, aBlue, aAlpha));
   shape.EnableSmoothSides(true);
   shape.EnableLineMode(aEllipseMode != 0);
   shape.EnableLighting(false);
   int pattern = aLineStyle;
   switch (pattern)
   {
   case 0:
      pattern = UtoPolyLineShape::DASHED;
      break;
   case 1:
      pattern = UtoPolyLineShape::LONG_DASHED;
      break;
   case 2:
      pattern = UtoPolyLineShape::DOTTED;
      break;
   case 3:
      pattern = UtoPolyLineShape::DOTTED2;
      break;
   case 4:
      pattern = UtoPolyLineShape::DASH_DOT;
      break;
   case 5:
      pattern = UtoPolyLineShape::DASH_DOT2;
      break;
   case 6:
   default:
      pattern = UtoPolyLineShape::SOLID;
   }
   shape.SetPattern(pattern);

   static unsigned int eid       = 0;
   unsigned int        lineStyle = (aLineSize | (aLineStyle << 8)) | (relative ? cRELATIVE_BIT : 0);
   std::stringstream   ss;
   ss << "post_process_draw_ellipse_" << (int)lineStyle << "_" << std::to_string(eid++);
   ellipseShape.mName            = ss.str();
   ellipseShape.mEllipseShapePtr = dynamic_cast<UtoEllipseShape*>(BindShape(ellipseShape.mName, shape));
   if (relative)
   {
      ellipseShape.mShapeType |= cRELATIVE_BIT;
   }

   ShapeIndex index;
   index.mShapePtr = &ellipseShape;
   index.mUserId   = aUserId;

   UtoPosition pos;
   UtEntity    tmp;
   GetCurrentLocationECEF(aPoint, pos.ptr());
   tmp.SetLocationWCS(pos.ptr());

   if (aPoint.mVertexType == cABSOLUTE_SCREEN)
   {
      long flags = UtoShape::BILLBOARD | UtoShape::AUTO_SCALE;
      ellipseShape.mEllipseShapePtr->SetShapeFlags(flags, flags);
      vespa::VaPosition::GetSceneProjection(GetViewer()->GetUniqueId())->ConvertFromECEF(pos.ptr());
      ellipseShape.mEllipseShapePtr->SetPosition(pos);
   }
   else
   {
      tmp.SetOrientationNED(aHeading * UtMath::cRAD_PER_DEG, 0, 0);

      UtoOrientation ori;
      ConvertLocalOrientation(tmp, ori);
      vespa::VaPosition::GetSceneProjection(GetViewer()->GetUniqueId())->ConvertFromECEF(pos.ptr());
      ellipseShape.mEllipseShapePtr->SetPosition(pos);
      ellipseShape.mEllipseShapePtr->SetOrientation(ori);
   }

   if (relative)
   {
      PointPosition ellipsePos(index);
      ellipsePos.mPoint = aPoint;
      mDynamicPoints.insert(ellipsePos);
   }
   AddLogarithmicDepthBuffer(ellipseShape.mEllipseShapePtr);
   return index;
}

void wkf::OverlayWsfDrawBase::DeleteEllipse(const ShapeIndexBase& aShapeIndex)
{
   EllipseShape* ellipseShape = (EllipseShape*)aShapeIndex.mShapePtr;
   ShapeIndex    ind;
   (ShapeIndexBase&)ind = aShapeIndex;
   ind.mUserId          = ellipseShape->mUserIndex;
   RemovedIndexP(ind);
   RemovedIndex(ind);

   UnbindShape(ellipseShape->mName);
   mEllipseShapes.erase(ellipseShape);
   delete ellipseShape;
}

wkf::OverlayWsfDrawBase::ShapeIndex wkf::OverlayWsfDrawBase::AddEllipsoid(int           aUserId,
                                                                          VertexObject& aPoint,
                                                                          float         aHeading,
                                                                          float         aPitch,
                                                                          float         aRoll,
                                                                          float         aAxisA,
                                                                          float         aAxisB,
                                                                          float         aAxisC,
                                                                          unsigned char aLineSize,
                                                                          unsigned char aLineStyle,
                                                                          unsigned char aEllipseMode,
                                                                          unsigned char aRed,
                                                                          unsigned char aGreen,
                                                                          unsigned char aBlue,
                                                                          unsigned char aAlpha /* =255 */)
{
   bool            relative       = aPoint.IsRelative();
   EllipsoidShape& ellipsoidShape = *new EllipsoidShape(relative);
   ellipsoidShape.mUserIndex      = aUserId;
   mEllipsoidShapes.insert(&ellipsoidShape);

   UtoSphereShape shape(1.0, 32, 32, aAxisA, aAxisB, aAxisC, aEllipseMode != 0, aEllipseMode != 0);
   shape.ShowSphereGeometry(aEllipseMode == 0);

   int pattern = aLineStyle;
   switch (pattern)
   {
   case 0:
      pattern = UtoPolyLineShape::DASHED;
      break;
   case 1:
      pattern = UtoPolyLineShape::LONG_DASHED;
      break;
   case 2:
      pattern = UtoPolyLineShape::DOTTED;
      break;
   case 3:
      pattern = UtoPolyLineShape::DOTTED2;
      break;
   case 4:
      pattern = UtoPolyLineShape::DASH_DOT;
      break;
   case 5:
      pattern = UtoPolyLineShape::DASH_DOT2;
      break;
   case 6:
   default:
      pattern = UtoPolyLineShape::SOLID;
   }
   shape.SetPattern(pattern);
   shape.SetLineWidth(aLineSize);
   shape.SetColor(UtoColor(aRed, aGreen, aBlue, aAlpha));
   shape.EnableLighting(false);

   static unsigned int eid       = 0;
   unsigned int        lineStyle = (aLineSize | (aLineStyle << 8)) | (relative ? cRELATIVE_BIT : 0);
   std::stringstream   ss;
   ss << "post_process_draw_ellipsoid_" << (int)lineStyle << "_" << std::to_string(eid++);
   ellipsoidShape.mName              = ss.str();
   ellipsoidShape.mEllipsoidShapePtr = dynamic_cast<UtoSphereShape*>(BindShape(ellipsoidShape.mName, shape));
   if (relative)
   {
      ellipsoidShape.mShapeType |= cRELATIVE_BIT;
   }

   ShapeIndex index;
   index.mShapePtr = &ellipsoidShape;
   index.mUserId   = aUserId;

   UtoPosition pos;
   UtEntity    tmp;
   GetCurrentLocationECEF(aPoint, pos.ptr());
   tmp.SetLocationWCS(pos.ptr());
   tmp.SetOrientationNED(aHeading * UtMath::cRAD_PER_DEG, aPitch * UtMath::cRAD_PER_DEG, aRoll * UtMath::cRAD_PER_DEG);

   UtoOrientation ori;
   ConvertLocalOrientation(tmp, ori);
   vespa::VaPosition::GetSceneProjection(GetViewer()->GetUniqueId())->ConvertFromECEF(pos.ptr());
   ellipsoidShape.mEllipsoidShapePtr->SetPosition(pos);
   ellipsoidShape.mEllipsoidShapePtr->SetOrientation(ori);

   if (relative)
   {
      PointPosition ellipsoidPos(index);
      ellipsoidPos.mPoint = aPoint;
      mDynamicPoints.insert(ellipsoidPos);
   }
   AddLogarithmicDepthBuffer(ellipsoidShape.mEllipsoidShapePtr);
   return index;
}

void wkf::OverlayWsfDrawBase::DeleteEllipsoid(const ShapeIndexBase& aShapeIndex)
{
   EllipsoidShape* ellipsoidShape = (EllipsoidShape*)aShapeIndex.mShapePtr;
   ShapeIndex      ind;
   (ShapeIndexBase&)ind = aShapeIndex;
   ind.mUserId          = ellipsoidShape->mUserIndex;
   RemovedIndexP(ind);
   RemovedIndex(ind);

   UnbindShape(ellipsoidShape->mName);
   mEllipsoidShapes.erase(ellipsoidShape);
   delete ellipsoidShape;
}

wkf::OverlayWsfDrawBase::ShapeIndex wkf::OverlayWsfDrawBase::AddQuadrilateral(int           aUserId,
                                                                              VertexObject& aPoint1,
                                                                              VertexObject& aPoint2,
                                                                              VertexObject& aPoint3,
                                                                              VertexObject& aPoint4,
                                                                              unsigned char aRed,
                                                                              unsigned char aGreen,
                                                                              unsigned char aBlue,
                                                                              unsigned char aAlpha /*=255 */)
{
   bool relative = (aPoint1.IsRelative() || aPoint2.IsRelative() || aPoint3.IsRelative() || aPoint4.IsRelative());

   QuadrilateralShape& quadrilateralShape = *new QuadrilateralShape(relative);
   quadrilateralShape.mUserIndex          = aUserId;
   mQuadrilateralShapes.insert(&quadrilateralShape);

   UtoPosition corners[4];
   GetCurrentLocation(aPoint1, corners[0].ptr()); // top left corner
   GetCurrentLocation(aPoint2, corners[1].ptr());
   GetCurrentLocation(aPoint3, corners[2].ptr());
   GetCurrentLocation(aPoint4, corners[3].ptr());
   UtoPolygonShape shape(corners, 4);
   shape.SetColor(UtoColor(aRed, aGreen, aBlue, aAlpha));
   shape.EnableLighting(false);

   static unsigned int eid = 0;
   std::stringstream   ss;
   ss << "post_process_draw_quadrilateral_" << std::to_string(eid++);
   quadrilateralShape.mName            = ss.str();
   quadrilateralShape.mPolygonShapePtr = dynamic_cast<UtoPolygonShape*>(BindShape(quadrilateralShape.mName, shape));

   // Quadrilateral does not need a position set
   // YPR is currently zeroed
   quadrilateralShape.mPolygonShapePtr->SetOrientation(UtoOrientation(0, 0, 0));

   ShapeIndex index;
   index.mShapePtr = &quadrilateralShape;
   index.mUserId   = quadrilateralShape.mUserIndex;

   if (relative)
   {
      quadrilateralShape.mShapeType |= cRELATIVE_BIT;
      QuadrilateralPosition QuadrilateralPosition(index);
      QuadrilateralPosition.mPoint1 = aPoint1;
      QuadrilateralPosition.mPoint2 = aPoint2;
      QuadrilateralPosition.mPoint3 = aPoint3;
      QuadrilateralPosition.mPoint4 = aPoint4;
      mDynamicQuadrilaterals.insert(QuadrilateralPosition);
   }
   AddLogarithmicDepthBuffer(quadrilateralShape.mPolygonShapePtr);
   return index;
}

void wkf::OverlayWsfDrawBase::DeleteQuadrilateral(const ShapeIndexBase& aShapeIndex)
{
   QuadrilateralShape* quadrilateralShape = (QuadrilateralShape*)aShapeIndex.mShapePtr;
   ShapeIndex          ind;
   (ShapeIndexBase&)ind = aShapeIndex;
   ind.mUserId          = quadrilateralShape->mUserIndex;
   RemovedIndexP(ind);
   RemovedIndex(ind);

   UnbindShape(quadrilateralShape->mName);
   mQuadrilateralShapes.erase(quadrilateralShape);
   delete quadrilateralShape;
}

wkf::OverlayWsfDrawBase::ShapeIndex wkf::OverlayWsfDrawBase::AddText(int           aUserId,
                                                                     VertexObject& aPoint,
                                                                     int           aTextSize,
                                                                     std::string   aText,
                                                                     unsigned char aRed,
                                                                     unsigned char aGreen,
                                                                     unsigned char aBlue,
                                                                     unsigned char aAlpha /* =255 */)
{
   bool       relative  = aPoint.IsRelative();
   TextShape& textShape = *new TextShape(relative);
   textShape.mUserIndex = aUserId;
   mTextShapes.insert(&textShape);

   UtoRawShape shape;

   static unsigned int uid = 0;
   std::stringstream   ss;
   ss << "post_process_draw_text_" << std::to_string(uid++);
   textShape.mName         = ss.str();
   textShape.mTextShapePtr = dynamic_cast<UtoRawShape*>(BindShape(textShape.mName, shape));

   osg::ref_ptr<vespa::VaDecoratorNode> decorator = new vespa::VaDecoratorNode(vespa::VaDecoratorNode::cHideBehindEarth);
   QFont                                f;
   f.setPointSize(aTextSize);
   decorator->SetText(QString::fromStdString(aText), QColor(aRed, aGreen, aBlue), f, vespa::VaDecoratorNode::cOutline);
   textShape.mTextShapePtr->GetRoot()->addChild(decorator);


   if (relative)
   {
      textShape.mShapeType |= cRELATIVE_BIT;
   }

   ShapeIndex index;
   index.mShapePtr = &textShape;
   index.mUserId   = aUserId;
   UtoPosition pos;
   GetCurrentLocation(aPoint, pos.ptr());
   textShape.mTextShapePtr->SetPosition(pos);

   if (relative)
   {
      PointPosition textPos(index);
      textPos.mPoint = aPoint;
      mDynamicPoints.insert(textPos);
   }
   return index;
}

void wkf::OverlayWsfDrawBase::DeleteText(const ShapeIndexBase& aShapeIndex)
{
   TextShape* textShape = (TextShape*)aShapeIndex.mShapePtr;
   ShapeIndex ind;
   (ShapeIndexBase&)ind = aShapeIndex;
   ind.mUserId          = textShape->mUserIndex;
   RemovedIndexP(ind);
   RemovedIndex(ind);

   UnbindShape(textShape->mName);
   mTextShapes.erase(textShape);
   delete textShape;
}

wkf::OverlayWsfDrawBase::ShapeIndex wkf::OverlayWsfDrawBase::AddTimer(int           aUserId,
                                                                      VertexObject& aPoint,
                                                                      int           aPointSize,
                                                                      double        aStartTime,
                                                                      double        aDuration,
                                                                      unsigned char aRed,
                                                                      unsigned char aGreen,
                                                                      unsigned char aBlue,
                                                                      unsigned char aAlpha /* =255 */)
{
   bool        relative   = aPoint.IsRelative();
   TimerShape& timerShape = *new TimerShape(relative);
   timerShape.mUserIndex  = aUserId;
   mTimerShapes.insert(&timerShape);

   std::set<std::string> defset;
   osg::Program*         shader = UtoShaders::UseProgram("VaOverlayDrawTimer.vert", "VaOverlayDrawTimer.frag", defset);
   UtoTimerShape         shape;
   shape.SetProgram(shader, true);
   shape.SetColor(UtoColor(aRed, aGreen, aBlue, aAlpha));

   static unsigned int uid = 0;
   std::stringstream   ss;
   ss << "post_process_draw_timer_" << std::to_string(uid++);
   timerShape.mName          = ss.str();
   timerShape.mTimerShapePtr = dynamic_cast<UtoTimerShape*>(BindShape(timerShape.mName, shape));
   if (relative)
   {
      timerShape.mShapeType |= cRELATIVE_BIT;
   }

   ShapeIndex index;
   index.mShapePtr = &timerShape;
   index.mUserId   = aUserId;
   UtoPosition pos;
   GetCurrentLocation(aPoint, pos.ptr());
   timerShape.mTimerShapePtr->SetPosition(pos);
   timerShape.mTimerShapePtr->SetTiming(aStartTime, aDuration);
   timerShape.mTimerShapePtr->SetSize(aPointSize);

   if (relative)
   {
      PointPosition timerPos(index);
      timerPos.mPoint = aPoint;
      mDynamicPoints.insert(timerPos);
   }
   return index;
}

void wkf::OverlayWsfDrawBase::DeleteTimer(const ShapeIndexBase& aShapeIndex)
{
   TimerShape* timerShape = (TimerShape*)aShapeIndex.mShapePtr;
   ShapeIndex  ind;
   (ShapeIndexBase&)ind = aShapeIndex;
   ind.mUserId          = timerShape->mUserIndex;
   RemovedIndexP(ind);
   RemovedIndex(ind);

   UnbindShape(timerShape->mName);
   mTimerShapes.erase(timerShape);
   delete timerShape;
}

void wkf::OverlayWsfDrawBase::RemovedIndex(const ShapeIndex& aIndex) {}

//! Changes the user ID of a shape.
void wkf::OverlayWsfDrawBase::SetUserId(const ShapeIndexBase& aIndex, int aNewUserId)
{
   switch (aIndex.mShapePtr->mShapeType & ~cRELATIVE_BIT)
   {
   case cPOINT:
      ((PointShape*)aIndex.mShapePtr)->mUserIndices[aIndex.GetIndex()] = aNewUserId;
      break;
   case cLINE:
      ((LineShape*)aIndex.mShapePtr)->mUserIndices[aIndex.GetIndex()] = aNewUserId;
      break;
   case cICON:
      ((IconShape*)aIndex.mShapePtr)->mUserIndex = aNewUserId;
      break;
   case cELLIPSE:
      ((EllipseShape*)aIndex.mShapePtr)->mUserIndex = aNewUserId;
      break;
   case cELLIPSOID:
      ((EllipsoidShape*)aIndex.mShapePtr)->mUserIndex = aNewUserId;
      break;
   case cQUADRILATERAL:
      ((QuadrilateralShape*)aIndex.mShapePtr)->mUserIndex = aNewUserId;
      break;
   case cTIMER:
      ((TimerShape*)aIndex.mShapePtr)->mUserIndex = aNewUserId;
      break;
   case cTEXT:
      ((TextShape*)aIndex.mShapePtr)->mUserIndex = aNewUserId;
   }
}

void wkf::OverlayWsfDrawBase::DeleteShape(const ShapeIndexBase& aShapeIndex)
{
   switch (aShapeIndex.mShapePtr->mShapeType & ~cRELATIVE_BIT)
   {
   case cPOINT:
      DeletePoint(aShapeIndex);
      break;
   case cLINE:
      DeleteLine(aShapeIndex);
      break;
   case cICON:
      DeleteIcon(aShapeIndex);
      break;
   case cELLIPSE:
      DeleteEllipse(aShapeIndex);
      break;
   case cELLIPSOID:
      DeleteEllipsoid(aShapeIndex);
      break;
   case cQUADRILATERAL:
      DeleteQuadrilateral(aShapeIndex);
      break;
   case cTEXT:
      DeleteText(aShapeIndex);
      break;
   case cTIMER:
      DeleteTimer(aShapeIndex);
      break;
   default:
      assert(false);
   }
}

wkf::OverlayWsfDrawManager::~OverlayWsfDrawManager()
{
   for (auto& overlay : mOverlays)
   {
      std::string overlayName = overlay.second->GetName();
      overlay.second->GetViewer()->RemoveOverlay(overlayName);
   }
}

void wkf::OverlayWsfDrawManager::ProcessDisOtherData(GenI&              aGenI,
                                                     const std::string& aConnection,
                                                     const int          aVersion,
                                                     double             aSimTime /*= -1.0*/)
{
   // Version changes:
   // version 1: Added layers.
   // version 2: Added ellipses, which moved cERASE to 4 in the ElementType enumeration.
   // version 3: Added ellipsoids.
   // version 4: Added quadrilaterals.
   // version 5: Added timers

   struct ElementTypeV01 // Versions 0 and 1.
   {
      enum
      {
         cLINE,
         cPOINT,
         cICON,
         cERASE
      };
   };

   struct ElementType // Latest version.
   {
      enum
      {
         cLINE,
         cPOINT,
         cICON,
         cELLIPSE,
         cERASE,
         cELLIPSOID,
         cQUADRILATERAL,
         cTEXT,
         cTIMER,
         cNONE
      };
   };

   if (mScenarioPtr != nullptr)
   {
      OverlayWsfDraw* wsfDrawPtr = nullptr;

      double time;
      aGenI >> time;
      // check the time param in case ignore pdu time stamp is checked
      if (aSimTime > -1.0)
      {
         time = aSimTime;
      }

      while (aGenI.GetGetOffset(GenBuf::FromDataEnd) > 4)
      {
         bool                         validCommand = true;
         std::string                  iconName;
         std::string                  text;
         OverlayWsfDraw::VertexObject verts[4];
         uint32_t                     ID;
         uint16_t                     drawType;
         float                        heading;
         float                        pitch;
         float                        roll;
         uint8_t                      lineSize    = 0;
         uint8_t                      pointSize   = 0;
         uint8_t                      textSize    = 0;
         uint8_t                      lineStyle   = 0;
         uint8_t                      ellipseMode = 0;
         uint8_t                      r = 0, g = 0, b = 0, a = 0;
         float                        axisA = 0, axisB = 0, axisC = 0;
         float                        duration;

         if (aVersion >= 1)
         {
            aGenI.GetString(mCurrentLayerName);
         }

         aGenI >> drawType >> ID;

         if (wsfDrawPtr != nullptr)
         {
            if (wsfDrawPtr->GetName() != mCurrentLayerName)
            {
               wsfDrawPtr = nullptr;
            }
         }

         if (wsfDrawPtr == nullptr)
         {
            Overlays::const_iterator overlayIter = mOverlays.find(mCurrentLayerName);
            if (overlayIter != mOverlays.end())
            {
               wsfDrawPtr = overlayIter->second;
            }
            else
            {
               // Check if layer name is associated with an entity
               vespa::VaEntity* entityPtr = mScenarioPtr->FindEntity(mCurrentLayerName);

               vespa::VaViewer* viewerPtr = vespa::VaEnvironment::Instance().GetViewerManager()->CurrentViewer();
               wsfDrawPtr                 = dynamic_cast<OverlayWsfDraw*>(
                  viewerPtr->AddOverlay("wsfdraw",
                                        mCurrentLayerName + " (" + aConnection + ")",
                                        (entityPtr != nullptr) ? vespa::VaOverlay::cOVERLAY_ENTITY :
                                                                                 vespa::VaOverlay::cOVERLAY_GLOBAL));
               if (wsfDrawPtr != nullptr)
               {
                  mOverlays.insert(std::make_pair(mCurrentLayerName, wsfDrawPtr));
               }
               else
               {
                  continue;
               }
            }
         }

         // Handle version 0 and 1 of the WsfDraw packet.
         if (aVersion < 2)
         {
            if (drawType != ElementTypeV01::cERASE)
            {
               // Read the color and duration to apply to the shape that we're drawing.
               aGenI >> r >> g >> b >> a;
               aGenI >> duration;
            }

            switch (drawType)
            {
            case ElementTypeV01::cLINE:
               validCommand &= ReadVertexObject(aGenI, verts[0]);
               validCommand &= ReadVertexObject(aGenI, verts[1]);
               aGenI >> lineSize >> lineStyle;
               if (validCommand)
               {
                  wsfDrawPtr->AddLine(time, time + duration, ID, verts[0], verts[1], lineSize, lineStyle, r, g, b, a);
               }
               break;
            case ElementTypeV01::cPOINT:
               validCommand &= ReadVertexObject(aGenI, verts[0]);
               aGenI >> pointSize;
               if (validCommand)
               {
                  wsfDrawPtr->AddPoint(time, time + duration, ID, verts[0], pointSize, r, g, b, a);
               }
               break;
            case ElementTypeV01::cICON:
               validCommand &= ReadVertexObject(aGenI, verts[0]);
               aGenI >> heading;
               aGenI.GetString(iconName);
               if (validCommand)
               {
                  wsfDrawPtr->AddIcon(time, time + duration, ID, verts[0], heading, iconName, r, g, b, a);
               }
               break;
            case ElementTypeV01::cERASE:
               if (validCommand)
               {
                  EraseIdFromLayers(time, ID);
               }
               break;
            default:
               break;
            }
         }
         // Handle later versions of the WsfDraw packet.
         else // (aVersion >= 2)
         {
            if (drawType != ElementType::cERASE)
            {
               // Read the color and duration to apply to the shape that we're drawing.
               aGenI >> r >> g >> b >> a;
               aGenI >> duration;
            }

            switch (drawType)
            {
            case ElementType::cLINE:
               validCommand &= ReadVertexObject(aGenI, verts[0]);
               validCommand &= ReadVertexObject(aGenI, verts[1]);
               aGenI >> lineSize >> lineStyle;
               if (validCommand)
               {
                  wsfDrawPtr->AddLine(time, time + duration, ID, verts[0], verts[1], lineSize, lineStyle, r, g, b, a);
               }
               break;
            case ElementType::cPOINT:
               validCommand &= ReadVertexObject(aGenI, verts[0]);
               aGenI >> pointSize;
               if (validCommand)
               {
                  wsfDrawPtr->AddPoint(time, time + duration, ID, verts[0], pointSize, r, g, b, a);
               }
               break;
            case ElementType::cICON:
               validCommand &= ReadVertexObject(aGenI, verts[0]);
               aGenI >> heading;
               aGenI.GetString(iconName);
               if (validCommand)
               {
                  wsfDrawPtr->AddIcon(time, time + duration, ID, verts[0], heading, iconName, r, g, b, a);
               }
               break;
            case ElementType::cELLIPSE:
               validCommand &= ReadVertexObject(aGenI, verts[0]);
               aGenI >> lineSize >> lineStyle;
               aGenI >> ellipseMode;
               aGenI >> heading;
               aGenI >> axisA >> axisB;
               if (validCommand)
               {
                  wsfDrawPtr->AddEllipse(time,
                                         time + duration,
                                         ID,
                                         verts[0],
                                         heading,
                                         axisA,
                                         axisB,
                                         lineSize,
                                         lineStyle,
                                         ellipseMode,
                                         r,
                                         g,
                                         b,
                                         a);
               }
               break;
            case ElementType::cERASE:
               if (validCommand)
               {
                  EraseIdFromLayers(time, ID);
               }
               break;
            default:
               break;
            }

            if (aVersion > 2)
            {
               switch (drawType)
               {
               case ElementType::cELLIPSOID:
                  validCommand &= ReadVertexObject(aGenI, verts[0]);
                  aGenI >> lineSize >> lineStyle;
                  aGenI >> ellipseMode;
                  aGenI >> heading >> pitch >> roll;
                  aGenI >> axisA >> axisB >> axisC;
                  if (validCommand)
                  {
                     wsfDrawPtr->AddEllipsoid(time,
                                              time + duration,
                                              ID,
                                              verts[0],
                                              heading,
                                              pitch,
                                              roll,
                                              axisA,
                                              axisB,
                                              axisC,
                                              lineSize,
                                              lineStyle,
                                              ellipseMode,
                                              r,
                                              g,
                                              b,
                                              a);
                  }
                  break;

               case ElementType::cQUADRILATERAL:
                  validCommand &= ReadVertexObject(aGenI, verts[0]);
                  validCommand &= ReadVertexObject(aGenI, verts[1]);
                  validCommand &= ReadVertexObject(aGenI, verts[2]);
                  validCommand &= ReadVertexObject(aGenI, verts[3]);
                  if (validCommand)
                  {
                     wsfDrawPtr->AddQuadrilateral(time, time + duration, ID, verts[0], verts[1], verts[2], verts[3], r, g, b, a);
                  }
                  break;

               case ElementType::cTEXT:
                  validCommand &= ReadVertexObject(aGenI, verts[0]);
                  aGenI >> textSize;
                  aGenI.GetString(text);
                  if (validCommand)
                  {
                     wsfDrawPtr->AddText(time, time + duration, ID, verts[0], textSize, text, r, g, b, a);
                  }
                  break;
               case ElementType::cTIMER:
                  validCommand &= ReadVertexObject(aGenI, verts[0]);
                  aGenI >> pointSize;
                  if (validCommand)
                  {
                     wsfDrawPtr->AddTimer(time, time + duration, ID, verts[0], pointSize, r, g, b, a);
                  }
                  break;
               default:
                  break;
               }
            }
         }
      }
   }
}

bool wkf::OverlayWsfDrawManager::ReadVertexObject(GenI& aGenI, OverlayWsfDraw::VertexObject& aVertex)
{
   // A vertex may be invalid if the entity isn't available yet or doesn't exist.
   bool validVertex = false;

   uint8_t vType = 0;
   aGenI >> vType;
   aVertex.mVertexType = (OverlayWsfDraw::VertexType)vType;
   if (aVertex.mVertexType != OverlayWsfDraw::cRELATIVE_ZERO)
   {
      aGenI >> aVertex.mPosition[0] >> aVertex.mPosition[1] >> aVertex.mPosition[2];
      validVertex = true;
   }
   if ((aVertex.mVertexType != OverlayWsfDraw::cUNSET_VERTEX) && (aVertex.mVertexType != OverlayWsfDraw::cABSOLUTE_WCS))
   {
      uint16_t site;
      uint16_t application;
      uint16_t entity;
      aGenI >> site;
      aGenI >> application;
      aGenI >> entity;

      vespa::VaEntity* entityPtr = mScenarioPtr->FindEntity(site, application, entity);
      if (entityPtr != nullptr)
      {
         aVertex.mReferenceEntityPtr = entityPtr;
         validVertex                 = true;
      }
   }
   else
   {
      aVertex.mReferenceEntityPtr = nullptr;
   }
   return validVertex;
}

void wkf::OverlayWsfDrawManager::EraseIdFromLayers(double aSimTime, unsigned int aId)
{
   for (auto& overlay : mOverlays)
   {
      OverlayWsfDraw* overlayPtr = overlay.second;
      if (overlayPtr != nullptr)
      {
         overlayPtr->Erase(aSimTime, aId);
      }
   }
}

wkf::OverlayWsfDraw::OverlayWsfDraw()
   : mCurrentTime(0.0)
   , mInitialized(false)
   , mMutex()
{
   SetType<OverlayWsfDraw>();
   mCallbacks.Add(vespa::VaObserver::MapProjectionChanged.Connect(&OverlayWsfDraw::MapProjectionChangedCB, this));
}

wkf::OverlayWsfDraw::OverlayWsfDraw(const std::string& aName)
   : OverlayWsfDrawBase(aName)
   , mCurrentTime(0.0)
   , mEraseCacheNextElement(0)
   , mInitialized(false)
{
   SetType<OverlayWsfDraw>();
   mCallbacks.Add(vespa::VaObserver::MapProjectionChanged.Connect(&OverlayWsfDraw::MapProjectionChangedCB, this));
}

wkf::OverlayWsfDraw::OverlayWsfDraw(const OverlayWsfDraw& aSrc)
   : OverlayWsfDrawBase(aSrc)
   , mCurrentTime(aSrc.mCurrentTime)
   , mEraseCacheNextElement(0)
   , mInitialized(false)
{
   SetType<OverlayWsfDraw>();
   mCallbacks.Add(vespa::VaObserver::MapProjectionChanged.Connect(&OverlayWsfDraw::MapProjectionChangedCB, this));
}

wkf::OverlayWsfDraw::~OverlayWsfDraw()
{
   // clean up the extended element data
   for (size_t i = 0; i < mElements.size(); ++i)
   {
      Extended*& ex = mElements[i].mExtended;
      if (ex)
      {
         delete ex;
         ex = nullptr;
      }
   }
}

wkf::OverlayWsfDraw* wkf::OverlayWsfDraw::Clone() const
{
   return new OverlayWsfDraw(*this);
}

bool wkf::OverlayWsfDraw::Initialize()
{
   mInitialized = true;
   return true;
}

void wkf::OverlayWsfDraw::DeactivateElementsWithReference(const std::string& aReferenceEntityName)
{
   int index = 0;
   for (auto& e : mElements)
   {
      bool referenced = false;
      for (auto& p : e.mPoints)
      {
         if (p.mReferenceEntityPtr != nullptr)
         {
            if (p.mReferenceEntityPtr->GetName() == aReferenceEntityName)
            {
               referenced = true;
               if (e.mShapeIndex.IsValid())
               {
                  DeleteShape(e.mShapeIndex);
                  e.mShapeIndex = ShapeIndex();
               }
               p.mReferenceEntityPtr = nullptr;
            }
         }
      }
      if (referenced)
      {
         emit ElementActivated(index, false);
      }
      ++index;
   }
}

//! Add a line to be drawn
void wkf::OverlayWsfDraw::AddLine(double        aStartTime,
                                  double        aStopTime,
                                  unsigned int  aId,
                                  VertexObject& aPoint1,
                                  VertexObject& aPoint2,
                                  unsigned char aLineSize,
                                  unsigned char aLineStyle,
                                  unsigned char aRed,
                                  unsigned char aGreen,
                                  unsigned char aBlue,
                                  unsigned char aAlpha /*=255*/)
{
   Element e;
   e.mID          = aId;
   e.mStartTime   = aStartTime;
   e.mStopTime    = std::min(cFOREVER, aStopTime);
   e.mLineStyle   = aLineStyle;
   e.mGraphicSize = aLineSize;
   e.mPoints[0]   = aPoint1;
   e.mPoints[1]   = aPoint2;
   e.mElementType = cLINE;
   e.mColor       = (aRed << 24) | (aGreen << 16) | (aBlue << 8) | (aAlpha);
   InsertElement(e);
}

//! Add a point to be drawn
void wkf::OverlayWsfDraw::AddPoint(double        aStartTime,
                                   double        aStopTime,
                                   unsigned int  aId,
                                   VertexObject& aPoint,
                                   int           aPointSize,
                                   unsigned char aRed,
                                   unsigned char aGreen,
                                   unsigned char aBlue,
                                   unsigned char aAlpha /*=255*/)
{
   Element e;
   e.mID          = aId;
   e.mStartTime   = aStartTime;
   e.mStopTime    = aStopTime;
   e.mPoints[0]   = aPoint;
   e.mGraphicSize = ut::safe_cast<unsigned char, int>(aPointSize);
   e.mElementType = cPOINT;
   e.mColor       = (aRed << 24) | (aGreen << 16) | (aBlue << 8) | (aAlpha);
   InsertElement(e);
}

//! Add an icon to be drawn
void wkf::OverlayWsfDraw::AddIcon(double        aStartTime,
                                  double        aStopTime,
                                  unsigned int  aId,
                                  VertexObject& aPoint,
                                  float         aHeading,
                                  std::string   aIconName,
                                  unsigned char aRed,
                                  unsigned char aGreen,
                                  unsigned char aBlue,
                                  unsigned char aAlpha /*=255*/)
{
   Element e;
   e.mID          = aId;
   e.mStartTime   = aStartTime;
   e.mStopTime    = aStopTime;
   e.mPoints[0]   = aPoint;
   Extended& ex   = e.GetExtended();
   ex.mHeading    = aHeading;
   e.mElementType = cICON;
   ex.mText       = aIconName;
   e.mColor       = (aRed << 24) | (aGreen << 16) | (aBlue << 8) | (aAlpha);
   InsertElement(e);
}

//! Add an ellipse to be drawn
void wkf::OverlayWsfDraw::AddEllipse(double        aStartTime,
                                     double        aStopTime,
                                     unsigned int  aId,
                                     VertexObject& aPoint,
                                     float         aHeading,
                                     float         aAxisA,
                                     float         aAxisB,
                                     unsigned char aLineSize,
                                     unsigned char aLineStyle,
                                     unsigned char aEllipseMode,
                                     unsigned char aRed,
                                     unsigned char aGreen,
                                     unsigned char aBlue,
                                     unsigned char aAlpha /*=255*/)
{
   Element e;
   e.mID           = aId;
   e.mStartTime    = aStartTime;
   e.mStopTime     = aStopTime;
   e.mPoints[0]    = aPoint;
   Extended& ex    = e.GetExtended();
   ex.mHeading     = aHeading;
   e.mElementType  = cELLIPSE;
   e.mGraphicSize  = aLineSize;
   e.mLineStyle    = aLineStyle;
   ex.mEllipseMode = aEllipseMode;
   ex.mAxisA       = aAxisA;
   ex.mAxisB       = aAxisB;
   e.mColor        = (aRed << 24) | (aGreen << 16) | (aBlue << 8) | (aAlpha);
   InsertElement(e);
}

//! Add an ellipsoid to be drawn
void wkf::OverlayWsfDraw::AddEllipsoid(double        aStartTime,
                                       double        aStopTime,
                                       unsigned int  aId,
                                       VertexObject& aPoint,
                                       float         aHeading,
                                       float         aPitch,
                                       float         aRoll,
                                       float         aAxisA,
                                       float         aAxisB,
                                       float         aAxisC,
                                       unsigned char aLineSize,
                                       unsigned char aLineStyle,
                                       unsigned char aEllipseMode,
                                       unsigned char aRed,
                                       unsigned char aGreen,
                                       unsigned char aBlue,
                                       unsigned char aAlpha /* =255 */)
{
   Element e;
   e.mID           = aId;
   e.mStartTime    = aStartTime;
   e.mStopTime     = aStopTime;
   e.mPoints[0]    = aPoint;
   Extended& ex    = e.GetExtended();
   ex.mHeading     = aHeading;
   ex.mPitch       = aPitch;
   ex.mRoll        = aRoll;
   e.mElementType  = cELLIPSOID;
   e.mGraphicSize  = aLineSize;
   e.mLineStyle    = aLineStyle;
   ex.mEllipseMode = aEllipseMode;
   ex.mAxisA       = aAxisA;
   ex.mAxisB       = aAxisB;
   ex.mAxisC       = aAxisC;
   e.mColor        = (aRed << 24) | (aGreen << 16) | (aBlue << 8) | (aAlpha);
   InsertElement(e);
}


void wkf::OverlayWsfDraw::AddQuadrilateral(double        aStartTime,
                                           double        aStopTime,
                                           unsigned int  aId,
                                           VertexObject& aPoint1,
                                           VertexObject& aPoint2,
                                           VertexObject& aPoint3,
                                           VertexObject& aPoint4,
                                           unsigned char aRed,
                                           unsigned char aGreen,
                                           unsigned char aBlue,
                                           unsigned char aAlpha /*=255*/)
{
   Element e;
   e.mID          = aId;
   e.mStartTime   = aStartTime;
   e.mStopTime    = std::min(cFOREVER, aStopTime);
   e.mPoints[0]   = aPoint1;
   e.mPoints[1]   = aPoint2;
   e.mPoints[2]   = aPoint3;
   e.mPoints[3]   = aPoint4;
   e.mGraphicSize = 0;
   e.mElementType = cQUADRILATERAL;
   e.mColor       = (aRed << 24) | (aGreen << 16) | (aBlue << 8) | (aAlpha);
   InsertElement(e);
}

void wkf::OverlayWsfDraw::AddText(double        aStartTime,
                                  double        aStopTime,
                                  unsigned int  aId,
                                  VertexObject& aPoint,
                                  int           aTextSize,
                                  std::string   aText,
                                  unsigned char aRed,
                                  unsigned char aGreen,
                                  unsigned char aBlue,
                                  unsigned char aAlpha /* =255 */)
{
   Element e;
   e.mID                 = aId;
   e.mStartTime          = aStartTime;
   e.mStopTime           = aStopTime;
   e.mPoints[0]          = aPoint;
   e.mGraphicSize        = ut::safe_cast<unsigned char, int>(aTextSize);
   e.GetExtended().mText = aText;
   e.mElementType        = cTEXT;
   e.mColor              = (aRed << 24) | (aGreen << 16) | (aBlue << 8) | (aAlpha);
   InsertElement(e);
}

void wkf::OverlayWsfDraw::AddTimer(double        aStartTime,
                                   double        aStopTime,
                                   unsigned int  aId,
                                   VertexObject& aPoint,
                                   int           aPointSize,
                                   unsigned char aRed,
                                   unsigned char aGreen,
                                   unsigned char aBlue,
                                   unsigned char aAlpha /* =255 */)
{
   Element e;
   e.mID          = aId;
   e.mStartTime   = aStartTime;
   e.mStopTime    = std::min(cFOREVER, aStopTime);
   e.mGraphicSize = ut::safe_cast<unsigned char, int>(aPointSize);
   e.mPoints[0]   = aPoint;
   e.mElementType = cTIMER;
   e.mColor       = (aRed << 24) | (aGreen << 16) | (aBlue << 8) | (aAlpha);
   InsertElement(e);
}

//! Reduce the stop-time to aSimTime for all elements with ID=aId
void wkf::OverlayWsfDraw::Erase(double aSimTime, unsigned int aId)
{
   std::lock_guard<std::mutex> lock(mMutex);
   UpdateEraseCache();
   std::map<int, int>::iterator iter = mEraseCache.find(aId);
   if (iter != mEraseCache.end())
   {
      int i = iter->second;
      for (; i < ut::cast_to_int(mElements.size()); ++i)
      {
         Element& e = mElements[i];
         if (e.mStartTime > aSimTime)
         {
            break;
         }
         if (e.mID == aId)
         {
            // Erase() will only reduce the stop-time
            if (aSimTime < e.mStopTime)
            {
               double oldStopTime = e.mStopTime;
               mStopEvents.erase(ElementEvent(e.mStopTime, i));
               e.mStopTime = (e.mStartTime > aSimTime) ? e.mStartTime : aSimTime;
               mStopEvents.insert(ElementEvent(e.mStopTime, i));

               if (mCurrentTime < oldStopTime && mCurrentTime >= e.mStartTime && mCurrentTime > e.mStopTime)
               {
                  DeactivateElement(i);
               }
            }
         }
      }
      // The next Erase() for this ID will start here.
      iter->second = i;
   }
}

// TODO-UNUSED
// namespace
//{
//   void ComputeMountPosition(VaEntity*    aEntityPtr,
//                             double       aLocationECS[3],
//                             UtoPosition& aPosition)
//   {
//      double ecef[3], heading, pitch, roll;
//      aEntityPtr->GetPosition().GetECEF(ecef);
//      aEntityPtr->GetOrientation(heading, pitch, roll);
//      UtEntity tmp;
//      tmp.SetLocationWCS(ecef);
//      tmp.SetOrientationNED(heading, pitch, roll);
//      tmp.ConvertECSToWCS(aLocationECS, aPosition.ptr());
//   }
//}

void wkf::OverlayWsfDraw::GetCurrentLocation(VertexObject& aVertex, double aLocationWCS[3])
{
   if (aVertex.mVertexType == cABSOLUTE_WCS)
   {
      aLocationWCS[0] = aVertex.mPosition[0];
      aLocationWCS[1] = aVertex.mPosition[1];
      aLocationWCS[2] = aVertex.mPosition[2];
   }
   else if (aVertex.mVertexType == cRELATIVE_ZERO)
   {
      aVertex.mReferenceEntityPtr->GetPosition().GetECEF(aLocationWCS);
   }
   else if (aVertex.mVertexType == cRELATIVE_ECS)
   {
      double ecef[3], heading, pitch, roll;
      aVertex.mReferenceEntityPtr->GetPosition().GetECEF(ecef);
      aVertex.mReferenceEntityPtr->GetOrientation(heading, pitch, roll);
      UtEntity tmp;
      tmp.SetLocationWCS(ecef);
      tmp.SetOrientationNED(heading, pitch, roll);
      tmp.ConvertECSToWCS(aVertex.mPosition.GetData(), aLocationWCS);
   }
   else if (aVertex.mVertexType == cRELATIVE_NED)
   {
      double ecef[3];
      aVertex.mReferenceEntityPtr->GetPosition().GetECEF(ecef);
      UtEntity tmp;
      tmp.SetLocationWCS(ecef);
      tmp.ConvertNEDToWCS(aVertex.mPosition.GetData(), aLocationWCS);
   }
   else if (aVertex.mVertexType == cABSOLUTE_SCREEN)
   {
      vespa::VaViewer* viewerPtr = vespa::VaEnvironment::Instance().GetViewerManager()->CurrentViewer();
      if (viewerPtr)
      {
         double tx = 0.0;
         double ty = 0.0;
         double tz = 0.0;

         if (aVertex.mReferenceEntityPtr)
         {
            viewerPtr->WorldToScreen(aVertex.mReferenceEntityPtr->GetPosition(), tx, ty, tz);
         }

         unsigned int w = 0;
         unsigned int h = 0;
         viewerPtr->GetCamera()->GetViewport(w, h);
         double      x = (aVertex.mPosition[0] * static_cast<double>(w)) + tx;
         double      y = (aVertex.mPosition[1] * static_cast<double>(h)) + ty;
         UtoPosition pos(x, y, 0.05);
         if (viewerPtr->WindowToWorld(pos))
         {
            aLocationWCS[0] = pos[0];
            aLocationWCS[1] = pos[1];
            aLocationWCS[2] = pos[2];
         }
      }
   }

   int viewerId = GetViewer()->GetUniqueId();
   if (vespa::VaPosition::GetSceneProjectionType(viewerId) != UtoMapProjection::Geocentric)
   {
      double lla[3];
      UtEllipsoidalEarth::ConvertECEFToLLA(aLocationWCS, lla[0], lla[1], lla[2]);
      vespa::VaPosition pos(lla[0], lla[1], lla[2], aLocationWCS[0], aLocationWCS[1], aLocationWCS[2]);
      pos.GetSceneXYZ(aLocationWCS, viewerId);
   }
}

//! Activates / Deactivates elements using the start/stop times
void wkf::OverlayWsfDraw::Update()
{
   if (mMutex.try_lock())
   {
      // Screen elements need to be updated even if time is constant.
      for (const auto& screenEl : mScreenElements)
      {
         if (screenEl >= mElements.size())
         {
            break;
         }
         Element& element = mElements[screenEl];
         if (element.mShapeIndex.IsValid())
         {
            for (int i = 0; i < 4; ++i)
            {
               if (element.mPoints[i].mVertexType == cABSOLUTE_SCREEN)
               {
                  UtoPosition pos;
                  GetCurrentLocation(element.mPoints[i], &pos.x());

                  if (element.mElementType == cPOINT)
                  {
                     PointShape* pointPtr = (PointShape*)element.mShapeIndex.mShapePtr;
                     pointPtr->mPointShapePtr->SetAt(element.mShapeIndex.GetIndex(), pos);
                  }
                  else if (element.mElementType == cLINE)
                  {
                     if (i > 1)
                     {
                        break;
                     }
                     LineShape* shapePtr = (LineShape*)element.mShapeIndex.mShapePtr;
                     shapePtr->mPolyShapePtr->SetAt(element.mShapeIndex.GetIndex() * 2 + i, pos);
                  }
                  else if (element.mElementType == cICON)
                  {
                     IconShape* iconPtr = (IconShape*)element.mShapeIndex.mShapePtr;
                     iconPtr->mModelPtr->SetPosition(pos);
                     iconPtr->mModelPtr->SetUniform3f("UTO_EntityPosition", pos.x(), pos.y(), pos.z());
                  }
                  else if (element.mElementType == cELLIPSE)
                  {
                     EllipseShape* ellipsePtr = (EllipseShape*)element.mShapeIndex.mShapePtr;
                     ellipsePtr->mEllipseShapePtr->SetPosition(pos);
                  }
                  else if (element.mElementType == cELLIPSOID)
                  {
                     EllipsoidShape* ellipsoidPtr = (EllipsoidShape*)element.mShapeIndex.mShapePtr;
                     ellipsoidPtr->mEllipsoidShapePtr->SetPosition(pos);
                  }
                  else if (element.mElementType == cQUADRILATERAL)
                  {
                     QuadrilateralShape* shapePtr = (QuadrilateralShape*)element.mShapeIndex.mShapePtr;
                     shapePtr->mPolygonShapePtr->SetAt(i, pos);
                  }
                  else if (element.mElementType == cTEXT)
                  {
                     TextShape* textPtr = (TextShape*)element.mShapeIndex.mShapePtr;
                     textPtr->mTextShapePtr->SetPosition(pos);
                  }
                  else if (element.mElementType == cTIMER)
                  {
                     TimerShape* timerPtr = (TimerShape*)element.mShapeIndex.mShapePtr;
                     timerPtr->mTimerShapePtr->SetPosition(pos);
                  }
               }
            }
         }
      }

      if (!IsVisible())
      {
         mMutex.unlock();
         return;
      }

      double simTime = vespa::VaEnvironment::Instance().GetTime()->GetCurrentTime();
      double t1      = mCurrentTime;
      double t2      = simTime;
      if (t1 > t2)
      {
         std::swap(t1, t2);
      }
      else if (t1 == t2)
      {
         mMutex.unlock();
         return;
      }

      EventSet::iterator startBeg = mStartEvents.lower_bound(ElementEvent(t1, 0));
      EventSet::iterator startEnd = mStartEvents.upper_bound(ElementEvent(t2, 0));
      EventSet::iterator stopBeg  = mStopEvents.lower_bound(ElementEvent(t1, 0));
      EventSet::iterator stopEnd  = mStopEvents.upper_bound(ElementEvent(t2, 0));

      // Gather sorted list of elements being started and stopped
      std::vector<int> starts;
      for (EventSet::iterator i = startBeg; i != startEnd; ++i)
         starts.push_back(i->second);

      std::vector<int> stops;
      for (EventSet::iterator i = stopBeg; i != stopEnd; ++i)
         stops.push_back(i->second);

      bool reverse = simTime < mCurrentTime;
      if (reverse) // if reverse
      {
         std::swap(starts, stops);
      }
      // if we added a new element, but it missed the window, lets add it now
      for (const auto& e : mInsertedElementsOfInterest)
      {
         int    idx   = e.first;
         double start = e.second.first;
         double end   = e.second.second;
         if ((simTime < end) && (simTime > start))
         {
            if (std::find(starts.begin(), starts.end(), idx) == starts.end())
            {
               starts.push_back(idx);
            }
         }
      }
      mInsertedElementsOfInterest.clear();
      std::sort(starts.begin(), starts.end());
      std::sort(stops.begin(), stops.end());

      size_t startIndex = 0;
      size_t stopIndex  = 0;
      if (!starts.empty() || !stops.empty())
      {
         // Eliminate any values shared by both start and stop, no change should take place
         for (; startIndex < starts.size() && stopIndex < stops.size();)
         {
            if (starts[startIndex] == stops[stopIndex])
            {
               starts[startIndex++] = -1;
               if (!IsActive(stopIndex))
               {
                  // avoid using erase(), it is O(N)
                  stops[stopIndex] = -1;
               }
               stopIndex++;
            }
            else if (starts[startIndex] < stops[stopIndex])
            {
               ++startIndex;
            }
            else
            {
               ++stopIndex;
            }
         }
         for (std::vector<int>::reverse_iterator i = stops.rbegin(); i != stops.rend(); ++i)
         {
            int ind = *i;
            if (ind != -1)
            {
               DeactivateElement(ind);
            }
         }
         for (size_t i = 0; i < starts.size(); ++i)
         {
            int ind = starts[i];
            if (ind != -1)
            {
               ActivateElement(ind);
            }
         }
      }
      UpdateDynamicPoints();

      for (auto& timerShp : mTimerShapes)
      {
         timerShp->mTimerShapePtr->UpdateTime(mCurrentTime);
      }

      mCurrentTime = simTime;
      mMutex.unlock();
   }
}

void wkf::OverlayWsfDraw::InsertElement(Element& aElement)
{
   std::lock_guard<std::mutex> lock(mMutex);
   unsigned int                index = mElements.size();
   mElements.push_back(aElement);
   mStartEvents.insert(ElementEvent(aElement.mStartTime, index));
   if (aElement.mStopTime != cFOREVER)
   {
      mStopEvents.insert(ElementEvent(aElement.mStopTime, index));
   }
   if (aElement.mStartTime <= mCurrentTime && aElement.mStopTime > mCurrentTime)
   {
      mInsertedElementsOfInterest.emplace_back(index, std::make_pair(aElement.mStartTime, aElement.mStopTime));
   }

   if (aElement.mPoints[0].mVertexType == cABSOLUTE_SCREEN || aElement.mPoints[1].mVertexType == cABSOLUTE_SCREEN ||
       aElement.mPoints[2].mVertexType == cABSOLUTE_SCREEN || aElement.mPoints[3].mVertexType == cABSOLUTE_SCREEN)
   {
      mScreenElements.push_back(mElements.size() - 1);
   }
   emit ElementAdded(index);
}

bool wkf::OverlayWsfDraw::IsActive(int aIndex) const
{
   if (aIndex < ut::cast_to_int(mElements.size()))
   {
      const Element& element = mElements.at(aIndex);
      return element.mShapeIndex.IsValid();
   }
   return false;
}

void wkf::OverlayWsfDraw::DeactivateElement(int aIndex)
{
   Element& element = mElements[aIndex];
   if (element.mShapeIndex.IsValid())
   {
      DeleteShape(element.mShapeIndex);
      element.mShapeIndex = ShapeIndex();
   }
   emit ElementActivated(aIndex, false);
}

void wkf::OverlayWsfDraw::ActivateElement(int aIndex, bool aActive)
{
   if (aActive)
   {
      ActivateElement(aIndex);
   }
   else
   {
      DeactivateElement(aIndex);
   }
}

void wkf::OverlayWsfDraw::ActivateElement(int aIndex)
{
   if (!IsActive(aIndex))
   {
      switch (mElements[aIndex].mElementType)
      {
      case cLINE:
         ActivateLine(aIndex);
         break;
      case cPOINT:
         ActivatePoint(aIndex);
         break;
      case cICON:
         ActivateIcon(aIndex);
         break;
      case cELLIPSE:
         ActivateEllipse(aIndex);
         break;
      case cELLIPSOID:
         ActivateEllipsoid(aIndex);
         break;
      case cQUADRILATERAL:
         ActivateQuadrilateral(aIndex);
         break;
      case cTEXT:
         ActivateText(aIndex);
         break;
      case cTIMER:
         ActivateTimer(aIndex);
         break;
      default:
         break;
      }
   }
   emit ElementActivated(aIndex, true);
}

void wkf::OverlayWsfDraw::Reindex(const ShapeIndex& aOldIndex, ShapeIndex& aNewIndex)
{
   aNewIndex.mUserId = aOldIndex.mUserId;
   Element& e        = mElements[aOldIndex.mUserId];
   e.mShapeIndex.SetIndex(aNewIndex.GetIndex());
}

void wkf::OverlayWsfDraw::ActivatePoint(int aPointIndex)
{
   Element&      pointElement = mElements[aPointIndex];
   unsigned char r            = (unsigned char)(pointElement.mColor >> 24);
   unsigned char g            = (unsigned char)(pointElement.mColor >> 16);
   unsigned char b            = (unsigned char)(pointElement.mColor >> 8);
   unsigned char a            = (unsigned char)(pointElement.mColor);
   pointElement.mShapeIndex =
      OverlayWsfDrawBase::AddPoint(aPointIndex, pointElement.mPoints[0], pointElement.mGraphicSize, r, g, b, a);
}

void wkf::OverlayWsfDraw::ActivateLine(int aLineIndex)
{
   Element&      lineElement = mElements[aLineIndex];
   unsigned char r           = (unsigned char)(lineElement.mColor >> 24);
   unsigned char g           = (unsigned char)(lineElement.mColor >> 16);
   unsigned char b           = (unsigned char)(lineElement.mColor >> 8);
   unsigned char a           = (unsigned char)(lineElement.mColor);
   lineElement.mShapeIndex   = OverlayWsfDrawBase::AddLine(aLineIndex,
                                                         lineElement.mPoints[0],
                                                         lineElement.mPoints[1],
                                                         lineElement.mGraphicSize,
                                                         lineElement.mLineStyle,
                                                         r,
                                                         g,
                                                         b,
                                                         a);
}

void wkf::OverlayWsfDraw::ActivateIcon(int aIconIndex)
{
   Element&      iconElement = mElements[aIconIndex];
   unsigned char r           = (unsigned char)(iconElement.mColor >> 24);
   unsigned char g           = (unsigned char)(iconElement.mColor >> 16);
   unsigned char b           = (unsigned char)(iconElement.mColor >> 8);
   unsigned char a           = (unsigned char)(iconElement.mColor);
   iconElement.mShapeIndex   = OverlayWsfDrawBase::AddIcon(aIconIndex,
                                                         iconElement.mPoints[0],
                                                         iconElement.GetExtended().mHeading,
                                                         iconElement.GetExtended().mText,
                                                         r,
                                                         g,
                                                         b,
                                                         a);
}

void wkf::OverlayWsfDraw::ActivateEllipse(int aEllipseIndex)
{
   Element&      ellipseElement = mElements[aEllipseIndex];
   unsigned char r              = (unsigned char)(ellipseElement.mColor >> 24);
   unsigned char g              = (unsigned char)(ellipseElement.mColor >> 16);
   unsigned char b              = (unsigned char)(ellipseElement.mColor >> 8);
   unsigned char a              = (unsigned char)(ellipseElement.mColor);
   Extended&     ex             = ellipseElement.GetExtended();
   ellipseElement.mShapeIndex   = OverlayWsfDrawBase::AddEllipse(aEllipseIndex,
                                                               ellipseElement.mPoints[0],
                                                               ex.mHeading,
                                                               ex.mAxisA,
                                                               ex.mAxisB,
                                                               ellipseElement.mGraphicSize,
                                                               ellipseElement.mLineStyle,
                                                               ex.mEllipseMode,
                                                               r,
                                                               g,
                                                               b,
                                                               a);
}

void wkf::OverlayWsfDraw::ActivateEllipsoid(int aEllipsoidIndex)
{
   Element&      ellipsoidElement = mElements[aEllipsoidIndex];
   unsigned char r                = (unsigned char)(ellipsoidElement.mColor >> 24);
   unsigned char g                = (unsigned char)(ellipsoidElement.mColor >> 16);
   unsigned char b                = (unsigned char)(ellipsoidElement.mColor >> 8);
   unsigned char a                = (unsigned char)(ellipsoidElement.mColor);
   Extended&     ex               = ellipsoidElement.GetExtended();
   ellipsoidElement.mShapeIndex   = OverlayWsfDrawBase::AddEllipsoid(aEllipsoidIndex,
                                                                   ellipsoidElement.mPoints[0],
                                                                   ex.mHeading,
                                                                   ex.mPitch,
                                                                   ex.mRoll,
                                                                   ex.mAxisA,
                                                                   ex.mAxisB,
                                                                   ex.mAxisC,
                                                                   ellipsoidElement.mGraphicSize,
                                                                   ellipsoidElement.mLineStyle,
                                                                   ex.mEllipseMode,
                                                                   r,
                                                                   g,
                                                                   b,
                                                                   a);
}

void wkf::OverlayWsfDraw::ActivateQuadrilateral(int aQuadrilateralIndex)
{
   Element&      quadrilateralElement = mElements[aQuadrilateralIndex];
   unsigned char r                    = (unsigned char)(quadrilateralElement.mColor >> 24);
   unsigned char g                    = (unsigned char)(quadrilateralElement.mColor >> 16);
   unsigned char b                    = (unsigned char)(quadrilateralElement.mColor >> 8);
   unsigned char a                    = (unsigned char)(quadrilateralElement.mColor);
   quadrilateralElement.mShapeIndex   = OverlayWsfDrawBase::AddQuadrilateral(aQuadrilateralIndex,
                                                                           quadrilateralElement.mPoints[0],
                                                                           quadrilateralElement.mPoints[1],
                                                                           quadrilateralElement.mPoints[2],
                                                                           quadrilateralElement.mPoints[3],
                                                                           r,
                                                                           g,
                                                                           b,
                                                                           a);
}

void wkf::OverlayWsfDraw::ActivateText(int aTextIndex)
{
   Element&      textElement = mElements[aTextIndex];
   unsigned char r           = (unsigned char)(textElement.mColor >> 24);
   unsigned char g           = (unsigned char)(textElement.mColor >> 16);
   unsigned char b           = (unsigned char)(textElement.mColor >> 8);
   unsigned char a           = (unsigned char)(textElement.mColor);
   Extended&     ex          = textElement.GetExtended();
   textElement.mShapeIndex =
      OverlayWsfDrawBase::AddText(aTextIndex, textElement.mPoints[0], textElement.mGraphicSize, ex.mText, r, g, b, a);
}

void wkf::OverlayWsfDraw::ActivateTimer(int aTimerIndex)
{
   Element&      timerElement = mElements[aTimerIndex];
   unsigned char r            = (unsigned char)(timerElement.mColor >> 24);
   unsigned char g            = (unsigned char)(timerElement.mColor >> 16);
   unsigned char b            = (unsigned char)(timerElement.mColor >> 8);
   unsigned char a            = (unsigned char)(timerElement.mColor);
   timerElement.mShapeIndex   = OverlayWsfDrawBase::AddTimer(aTimerIndex,
                                                           timerElement.mPoints[0],
                                                           timerElement.mGraphicSize,
                                                           timerElement.mStartTime,
                                                           timerElement.mStopTime - timerElement.mStartTime,
                                                           r,
                                                           g,
                                                           b,
                                                           a);
}

void wkf::OverlayWsfDraw::UpdateEraseCache()
{
   // Store the first time an ID is used on an element.
   // mEraseCacheNextElement points to the next element which has not been processed
   if (mEraseCacheNextElement >= mElements.size())
   {
      return;
   }
   int lastElementId = mElements[mEraseCacheNextElement].mID;
   if (mEraseCache.find(lastElementId) == mEraseCache.end())
   {
      mEraseCache[lastElementId] = mEraseCacheNextElement;
   }
   while (++mEraseCacheNextElement < mElements.size())
   {
      // Adjacent elements likely have the same ID, if so we can avoid the map lookup
      int id = mElements[mEraseCacheNextElement].mID;
      if (id != lastElementId)
      {
         std::map<int, int>::iterator iter = mEraseCache.find(id);
         if (iter == mEraseCache.end())
         {
            mEraseCache[id] = mEraseCacheNextElement;
         }
         lastElementId = id;
      }
   }
}

// virtual
void wkf::OverlayWsfDraw::MapProjectionChangedCB(unsigned int aViewerId, const UtoMapProjection* aProjectionPtr)
{
   Rebuild();
}

void wkf::OverlayWsfDraw::Rebuild()
{
   std::vector<Element>::iterator it;
   int                            i = 0;
   for (const auto& ele : mElements)
   {
      if ((mCurrentTime >= ele.mStartTime) && (mCurrentTime < ele.mStopTime)) // if the element is active, rebuild it
      {
         DeactivateElement(i);
         ActivateElement(i);
      }
      ++i;
   } // build everything that is active
}

namespace WsfDrawFileEnum
{

enum LineStyle
{
   cDASHED,
   cLONG_DASHED,
   cDOTTED,
   cDOTTED2,
   cDASH_DOT,
   cDASH_DOT2,
   cSOLID
};

enum DrawType
{
   cLINE,
   cPOINT,
   cICON,
   cELLIPSE,
   cERASE,
   cELLIPSOID,
   cQUADRILATERAL,
   cTEXT,
   cTIMER,
   cNONE
};

enum VertexType
{
   cUNSET_VERTEX,
   cABSOLUTE_WCS,
   cRELATIVE_ZERO,
   cRELATIVE_ECS,
   cRELATIVE_NED,
   cABSOLUTE_SCREEN
};

enum EllipseMode
{
   cELLIPSE_FILL,
   cELLIPSE_LINE
};
} // namespace WsfDrawFileEnum
namespace
{

template<typename T>
bool Read(std::istream& aStream, T& aValue)
{
   aStream.read((char*)&aValue, sizeof(T));
   if (aStream)
      return true;
   return false;
}
bool ReadString(std::istream& aStream, std::string& aStr)
{
   std::streamsize stringLen(0);
   if (Read(aStream, stringLen) && stringLen < 10000 && stringLen >= 0)
   {
      aStr.resize(stringLen);
      aStream.read(&aStr[0], stringLen);
      if (aStream)
         return true;
   }
   return false;
}
bool ReadVertex(std::istream& aStream, wkf::OverlayWsfDrawBase::VertexObject& aVert)
{
   unsigned char vertType;
   Read(aStream, vertType);
   if (vertType == WsfDrawFileEnum::cABSOLUTE_WCS)
   {
      aVert.mReferenceEntityPtr = nullptr;
      aVert.mVertexType         = wkf::OverlayWsfDrawBase::cABSOLUTE_WCS;
      Read(aStream, aVert.mPosition[0]);
      Read(aStream, aVert.mPosition[1]);
      Read(aStream, aVert.mPosition[2]);
      return true;
   }
   return false;
}
} // namespace

bool wkf::OverlayWsfDraw::ReadDrawFile(std::istream& aStream, ByLayer& aDrawByLayer)
{
   char header[13] = {0};
   aStream.read(header, 13);
   if (strncmp(header, "WSF_DRAW ", 9) != 0)
      return false;
   std::string ver(header + 9, header + 13);
   if (ver != "0001")
      return false;

   unsigned char graphicSize, lineStyle;

   float           duration;
   float           heading, pitch, roll, axisA, axisB, axisC;
   unsigned char   ellipseMode;
   unsigned char   r = 0, g = 0, b = 0, a = 0;
   std::string     currentLayerName;
   OverlayWsfDraw* overlayPtr = nullptr;
   for (;;)
   {
      std::string layer;
      if (!ReadString(aStream, layer))
         break;

      if (currentLayerName != layer || !overlayPtr) // don't need to do lookup if there is no change
      {
         overlayPtr       = aDrawByLayer.GetLayer(layer);
         currentLayerName = layer;
      }


      int eventCount;
      if (!Read(aStream, eventCount))
         break;
      for (int i = 0; i < eventCount; ++i)
      {
         // if a read failure happens, keep going but don't draw it
         bool          ok = true;
         unsigned char drawType;
         ok |= Read(aStream, drawType);
         int id;
         ok |= Read(aStream, id);
         float simTime;
         ok |= Read(aStream, simTime);

         std::string text;


         VertexObject v1, v2;
         if (drawType != WsfDrawFileEnum::cERASE)
         {
            ok |= Read(aStream, r);
            ok |= Read(aStream, g);
            ok |= Read(aStream, b);
            ok |= Read(aStream, a);
            ok |= Read(aStream, duration);
         }

         switch (drawType)
         {
         case WsfDrawFileEnum::cLINE:
            ok |= ReadVertex(aStream, v1);
            ok |= ReadVertex(aStream, v2);
            ok |= Read(aStream, graphicSize);
            ok |= Read(aStream, lineStyle);
            if (overlayPtr && ok)
               overlayPtr->AddLine(simTime, simTime + duration, id, v1, v2, graphicSize, lineStyle, r, g, b, a);
            break;
         case WsfDrawFileEnum::cPOINT:
            ok |= ReadVertex(aStream, v1);
            ok |= Read(aStream, graphicSize);
            if (overlayPtr && ok)
               overlayPtr->AddPoint(simTime, simTime + duration, id, v1, graphicSize, r, g, b, a);
            break;
         case WsfDrawFileEnum::cICON:
            ok |= ReadVertex(aStream, v1);
            ok |= Read(aStream, heading);
            ok |= ReadString(aStream, text);
            if (overlayPtr && ok)
               overlayPtr->AddIcon(simTime, simTime + duration, id, v1, heading, text, r, g, b, a);
            break;
         case WsfDrawFileEnum::cELLIPSE:
            ok |= ReadVertex(aStream, v1);
            ok |= Read(aStream, graphicSize);
            ok |= Read(aStream, lineStyle);
            ok |= Read(aStream, ellipseMode);
            ok |= Read(aStream, heading);
            ok |= Read(aStream, axisA);
            ok |= Read(aStream, axisB);
            if (overlayPtr && ok)
               overlayPtr->AddEllipse(simTime,
                                      simTime + duration,
                                      id,
                                      v1,
                                      heading,
                                      axisA,
                                      axisB,
                                      graphicSize,
                                      lineStyle,
                                      ellipseMode,
                                      r,
                                      g,
                                      b,
                                      a);
            break;
         case WsfDrawFileEnum::cERASE:
            if (overlayPtr && ok)
               overlayPtr->Erase(simTime, id);
            break;
         case WsfDrawFileEnum::cELLIPSOID:
            ok |= ReadVertex(aStream, v1);
            ok |= Read(aStream, graphicSize);
            ok |= Read(aStream, lineStyle);
            ok |= Read(aStream, ellipseMode);
            ok |= Read(aStream, heading);
            ok |= Read(aStream, pitch);
            ok |= Read(aStream, roll);
            ok |= Read(aStream, axisA);
            ok |= Read(aStream, axisB);
            ok |= Read(aStream, axisC);
            if (overlayPtr && ok)
               overlayPtr->AddEllipsoid(simTime,
                                        simTime + duration,
                                        id,
                                        v1,
                                        heading,
                                        pitch,
                                        roll,
                                        axisA,
                                        axisB,
                                        axisC,
                                        graphicSize,
                                        lineStyle,
                                        ellipseMode,
                                        r,
                                        g,
                                        b,
                                        a);
            break;
         case WsfDrawFileEnum::cQUADRILATERAL:
            ok |= ReadVertex(aStream, v1);
            ok |= ReadVertex(aStream, v1);
            ok |= ReadVertex(aStream, v1);
            ok |= ReadVertex(aStream, v1);
            // todo: merge quad code
            break;
         case WsfDrawFileEnum::cTEXT:
            ok |= ReadVertex(aStream, v1);
            ok |= Read(aStream, graphicSize);
            ok |= ReadString(aStream, text);
            if (overlayPtr && ok)
               overlayPtr->AddText(simTime, simTime + duration, id, v1, graphicSize, text, r, g, b, a);
            break;
         case WsfDrawFileEnum::cTIMER:
            ok |= ReadVertex(aStream, v1);
            ok |= Read(aStream, graphicSize);
            if (overlayPtr && ok)
               overlayPtr->AddTimer(simTime, simTime + duration, id, v1, graphicSize, r, g, b, a);
         case WsfDrawFileEnum::cNONE:
            break;
         default:
            break;
         }
      }
   }
   return true;
}
