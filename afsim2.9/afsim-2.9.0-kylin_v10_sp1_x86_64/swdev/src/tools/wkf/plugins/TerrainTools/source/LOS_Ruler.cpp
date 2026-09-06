// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "LOS_Ruler.hpp"

#include <algorithm>
#include <sstream>

#include <QBuffer>
#include <QLayout>
#include <QPainter>
#include <QPushButton>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/LineStipple>
#include <osg/LineWidth>

#include "UtEarth.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtLonPos.hpp"
#include "UtQtGL2DPlot.hpp"
#include "UtQtUnitLineEdit.hpp"
#include "UtSphericalEarth.hpp"
#include "UtoRawShape.hpp"
#include "VaCamera.hpp"
#include "VaEntity.hpp"
#include "VaObserver.hpp"
#include "VaScenario.hpp"
#include "VaViewer.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfPointOfInterest.hpp"
#include "WkfResourceManager.hpp"
#include "WkfVtkEnvironment.hpp"

TerrainTools::LOS_Ruler::LOS_Ruler(wkf::Entity& aSource, wkf::Entity& aTarget, vespa::VaViewer& aViewer)
   : vespa::VaAttachmentQt(aSource, &aViewer, "los_ruler")
   , mTarget(&aTarget)
{
   SetType<LOS_Ruler>();
   mCallbacks.Add(vespa::VaObserver::EntityDeleted.Connect(&LOS_Ruler::TargetDeletedCB, this, mTarget->GetUniqueId()));
   mRequesterId = wkfEnv.GetResourceManager().Register();
}

TerrainTools::LOS_Ruler::~LOS_Ruler()
{
   wkfEnv.GetResourceManager().ReleaseRequests(mRequesterId);

   if (mPlotDockWidgetPtr)
   {
      delete mPlotDockWidgetPtr;
   }
}

bool TerrainTools::LOS_Ruler::Pick(vespa::VaViewer*                aViewerPtr,
                                   int                             aMouseX,
                                   int                             aMouseY,
                                   bool                            aToggle,
                                   bool                            aDoubleClick /*= false*/,
                                   const vespa::VaAttachmentSubId& aAttachmentSubPartId /*= 0*/)
{
   if (!IsMyViewer(aViewerPtr))
   {
      return false;
   }
   bool selected = aToggle ? !GetSelected(aViewerPtr) : true;
   SetSelected(aViewerPtr, selected, aAttachmentSubPartId);
   return true;
}

void TerrainTools::LOS_Ruler::SetSelected(vespa::VaViewer*                aViewerPtr,
                                          bool                            aSelected,
                                          const vespa::VaAttachmentSubId& aSubId,
                                          bool                            aForce /*= false*/)
{
   if (!IsMyViewer(aViewerPtr))
   {
      return;
   }
   VaAttachment::SetSelected(aViewerPtr, aSelected, aSubId, aForce);
}

std::string TerrainTools::LOS_Ruler::GetDescription(unsigned int aSubId)
{
   if (!mImageBuilt)
   {
      BuildImage();
   }
   QString html = QString("<img src='data:image/png;base64, %0'>").arg(QString(mImageData.toBase64()));
   return html.toStdString();
}

void TerrainTools::LOS_Ruler::BuildContextMenu(QMenu& aMenu, unsigned int aSubPartId)
{
   QAction* plot = new QAction("Plot Terrain Profile...", &aMenu);
   aMenu.addAction(plot);
   QObject::connect(plot, &QAction::triggered, [this]() { ShowPlot(); });
   QAction* mast = new QAction("Set Mast Height...", &aMenu);
   QObject::connect(mast, &QAction::triggered, [this]() { ConfigureMastHeight(); });
   aMenu.addAction(mast);
}

void TerrainTools::LOS_Ruler::ConfigureMastHeight()
{
   QDialog dlg;
   dlg.setLayout(new QVBoxLayout(&dlg));
   UtQtUnitLineEdit<UtLength>* edit = new UtQtUnitLineEdit<UtLength>(&dlg);
   edit->SetValue(mMastHeight);
   dlg.layout()->addWidget(edit);
   QHBoxLayout* hbox   = new QHBoxLayout();
   QPushButton* cancel = new QPushButton("Cancel", &dlg);
   QPushButton* ok     = new QPushButton("OK", &dlg);
   hbox->addWidget(ok);
   hbox->addWidget(cancel);
   QObject::connect(edit, &QLineEdit::textChanged, [edit, ok]() { ok->setEnabled(edit->IsValid()); });
   QObject::connect(ok, &QPushButton::pressed, &dlg, &QDialog::accept);
   QObject::connect(cancel, &QPushButton::pressed, &dlg, &QDialog::reject);

   dlg.layout()->addItem(hbox);
   if (QDialog::Accepted == dlg.exec())
   {
      mMastHeight = edit->GetValue();
      Recalculate();
   }
}

void TerrainTools::LOS_Ruler::ShowPlot()
{
   if (mPlotDockWidgetPtr.isNull())
   {
      mPlotDockWidgetPtr = new wkf::PlotDockWidget(wkfEnv.GetMainWindow());
      mPlotDockWidgetPtr->setAttribute(Qt::WA_DeleteOnClose);
      QPushButton* updateButton = new QPushButton("Update", mPlotDockWidgetPtr);
      mPlotDockWidgetPtr->layout()->addWidget(updateButton);
      mPlotDockWidgetPtr->setWindowTitle(
         QString("Height Above Terrain between %1 and %2").arg(GetParent().GetName().c_str(), mTarget->GetName().c_str()));
      QObject::connect(updateButton, &QPushButton::clicked, [this]() { UpdatePlottingWidget(); });
      mPlotDockWidgetPtr->show();
   }
   UpdatePlottingWidget();
}

void TerrainTools::LOS_Ruler::UpdatePlottingWidget()
{
   mPlotDockWidgetPtr->GetPlot()->ClearData();
   mPlotDockWidgetPtr->GetPlot()->AddPoints(std::map<double, double>(mResults.begin(), mResults.end()));
   mPlotDockWidgetPtr->GetPlot()->SetLabelYAxis("Height Above Terrain");
   mPlotDockWidgetPtr->GetPlot()->AddPoint(0, 0, 2);
   mPlotDockWidgetPtr->GetPlot()->AddPoint(1, 0, 2); // add a zero line
   mPlotDockWidgetPtr->GetPlot()->MakePlotPretty();
   mPlotDockWidgetPtr->GetPlot()->SetXAxisBounds(0, 1);
}

void TerrainTools::LOS_Ruler::Build()
{
   mGeometry->removePrimitiveSet(0, mGeometry->getNumPrimitiveSets());
   if (mTarget)
   {
      osg::Vec4 see(0.4f, 1.0f, 0.4f, 1.0f);
      osg::Vec4 block(0.6f, 0.0f, 0.0f, 1.0f);
      if (!GetSelected(GetViewer()))
      {
         see.a()   = 0.5f;
         block.a() = 0.5f;
      }
      osg::Vec3Array* v = new osg::Vec3Array;
      mGeometry->setVertexArray(v);
      osg::Vec4Array* c = new osg::Vec4Array;
      mGeometry->setColorArray(c);
      mGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
      v->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
      c->push_back(see);
      double pecef[3];
      double tecef[3];
      GetParentECEF(pecef);
      mTarget->GetPosition().GetECEF(tecef);
      osg::Vec3 delta(tecef[0] - pecef[0], tecef[1] - pecef[1], tecef[2] - pecef[2]);
      if (mBlockage >= 1.0)
      {
         v->push_back(delta);
         c->push_back(see);
         mGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 2));
      }
      else
      {
         v->push_back(delta * mBlockage);
         c->push_back(see);
         v->push_back(delta * mBlockage);
         c->push_back(block);
         v->push_back(delta);
         c->push_back(block);
         mGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 4));
      }
   }
}

void TerrainTools::LOS_Ruler::BuildImage()
{
   double h = 100;

   double*           hi;
   double            hia;
   vespa::VaPosition ground;
   double            g[3];
   double            p1[3];
   GetParentECEF(p1);
   double p2[3];
   mTarget->GetPosition().GetECEF(p2);
   if (GetParent().GetPosition().GetAlt() + mMastHeight > mTarget->GetPosition().GetAlt())
   {
      hi  = p1;
      hia = GetParent().GetPosition().GetAlt() + mMastHeight;

      ground = mTarget->GetPosition();
      ground.SetAlt(0.0);
   }
   else
   {
      hi     = p2;
      hia    = mTarget->GetPosition().GetAlt();
      ground = GetParent().GetPosition();
      ground.SetAlt(0.0);
   }
   UtVec3d p1v(p1);
   UtVec3d p2v(p2);
   double  w = 400;
   /*   w = h / aspectRatio;
      if (w > 600)
      {
         w /= 2;
         h /= 2;
         if (w > 600)
         {
            w = 600;
         }
      }*/

   double p3[3] = {p1[1] * p2[2] - p1[2] * p2[1],
                   p1[2] * p2[0] - p1[0] * p2[2],
                   p1[0] * p2[1] - p1[1] * p2[0]}; // this vector is normal to a plane defined by the center of the
                                                   // earth, the source, and the target
   // solve for:
   // A * 0 + B * 0 + C * 0 + D = w / 2.0 ... the center of the earth projects to the middle of the image
   // A * p1[0] + B * p1[1] + C * p1[2] + D = 0 ... the source is on the left
   // A * p2[0] + B * p2[1] + C * p2[2] + D = w ... the target is on the right
   // A * p3[0] + B * p3[1] + C * p3[2] + D = w / 2 ... a vector perpendicular to the image from the center of the earth
   // has no effect on the projection
   double D = w / 2.0;
   double C = D * (1.0 - (-p2[1] + p3[1] * p2[0] / p3[0]) / (p1[1] - p3[1] * p1[0] / p3[0])) /
              ((p2[2] - p3[2] * p2[0] / p3[0]) +
               (p1[2] - p3[2] * p1[0] / p3[0]) * (-p2[1] + p3[1] * p2[0] / p3[0]) / (p1[1] - p3[1] * p1[0] / p3[0]));
   double B = (D * (-1 - p1[0] / p2[0]) - C * (p1[2] - p2[2] * p1[0] / p2[0])) / (p1[1] - p2[1] * p1[0] / p2[0]);
   double A = (D - C * p2[2] - B * p2[1]) / p2[0]; // this defines the horizontal projection from ECEF to our image space

   ground.GetECEF(g);
   double o[3] = {g[0] + p3[0], g[1] + p3[1], g[2] + p3[2]};
   double hiy  = 0.95 * h;
   double cy   = -UtEarth::cA * hiy / hia;
   // solve for:
   // E * g[0] + F * g[1] + G * g[2] + H = 0 ... the lower point projected to the ground is zero
   // E * hi[0] + F * hi[1] + G * hi[2] + H = hiy ... the upper point projects to 95% of the image height
   // E * 0 + F * 0 + G * 0 + H = cy ... the center of the earth projects to some calculated (negative height)
   // E * o[0] + F * o[1] + G * o[2] + H = 0 ... the vector perpendicular to the image plane has no effect on y
   double H = cy;
   double G = (hiy + H * ((hi[1] - o[1] * hi[0] / o[0]) / (g[1] - o[1] * g[0] / o[0]) - 1)) /
              ((hi[2] - o[2] * hi[0] / o[0]) -
               (g[2] - o[2] * g[0] / o[0]) * (hi[1] - o[1] * hi[0] / o[0]) / (g[1] - o[1] * g[0] / o[0]));
   double F = (-H - G * g[2] + (H + G * o[2]) * (g[0] / o[0])) / (g[1] - o[1] * g[0] / o[0]); // *
   double E = (-H - G * g[2] - F * g[1]) / g[0];                                              // *

   mImageData.clear();

   QPixmap         pm(w, h);
   QPainter        p(&pm);
   QLinearGradient grad(0, 0, 0, h);
   grad.setColorAt(0, QColor(100, 128, 200));
   grad.setColorAt(1, QColor(200, 200, 200));
   p.fillRect(0, 0, w, h, grad);
   double  x1  = A * p1[0] + B * p1[1] + C * p1[2] + D;
   double  y1  = E * p1[0] + F * p1[1] + G * p1[2] + H;
   double  x2  = A * p2[0] + B * p2[1] + C * p2[2] + D;
   double  y2  = E * p2[0] + F * p2[1] + G * p2[2] + H;
   UtVec3d pmv = p1v * (1 - mBlockage) + p2v * mBlockage;

   QPainterPath mslpath;
   QPainterPath aglpath;
   bool         first = true;
   double       xm    = 0.0;
   double       ym    = 0.0;
   for (auto&& r : mResults)
   {
      UtVec3d m = p1v * (1.0f - r.first) + p2v * r.first;
      double  lla[3];
      double  msl[3];
      double  agl[3];
      UtEllipsoidalEarth::ConvertECEFToLLA(m.GetData(), lla[0], lla[1], lla[2]);
      UtEllipsoidalEarth::ConvertLLAToECEF(lla[0], lla[1], 0, msl);
      UtEllipsoidalEarth::ConvertLLAToECEF(lla[0], lla[1], lla[2] - r.second, agl);

      xm        = A * msl[0] + B * msl[1] + C * msl[2] + D;
      ym        = E * msl[0] + F * msl[1] + G * msl[2] + H;
      double xa = A * agl[0] + B * agl[1] + C * agl[2] + D;
      double ya = E * agl[0] + F * agl[1] + G * agl[2] + H;
      if (first)
      {
         mslpath.moveTo(xm, h - ym);
         aglpath.moveTo(xm, h - ym);
         first = false;
      }
      else
      {
         mslpath.lineTo(xm, h - ym);
      }
      aglpath.lineTo(xa, h - ya);
   }

   grad.setColorAt(0, QColor(170, 120, 40));
   grad.setColorAt(1, QColor(85, 60, 20));
   aglpath.lineTo(xm, h - ym);
   p.setBrush(grad);
   p.setPen(QColor(85, 60, 20));
   p.drawPath(aglpath);
   p.setBrush(QColor(0, 100, 255));
   p.setPen(QColor(0, 50, 196));
   p.drawPath(mslpath);
   // draw the line
   xm = A * pmv.Get(0) + B * pmv.Get(1) + C * pmv.Get(2) + D;
   ym = E * pmv.Get(0) + F * pmv.Get(1) + G * pmv.Get(2) + H;
   p.setPen(QColor(102, 255, 102));
   p.drawLine(x1, h - y1, xm, h - ym);
   p.setPen(QColor(153, 0, 0));
   p.drawLine(xm, h - ym, x2, h - y2);

   QBuffer buffer(&mImageData);
   pm.save(&buffer, "PNG", 100);
   mImageBuilt = true;
}

void TerrainTools::LOS_Ruler::GetParentECEF(double aECEF[3]) const
{
   vespa::VaPosition p = GetParent().GetPosition();
   p.SetAlt(p.GetAlt() + mMastHeight);
   p.GetECEF(aECEF);
}

void TerrainTools::LOS_Ruler::Recalculate()
{
   if (mTarget)
   {
      double secef[3];
      GetParentECEF(secef);
      double tecef[3];
      mTarget->GetPosition().GetECEF(tecef);
      // submit terrain check
      wkfEnv.GetResourceManager().LOS_Request(mRequesterId, UtVec3d(secef), UtVec3d(tecef));
      // query for terrain results
      if (wkfEnv.GetResourceManager().LOS_Result(mRequesterId, mResults))
      {
         mBlockage = 2.0;
         for (auto&& r : mResults)
         {
            if (r.first <= 1.0)
            {
               if (r.second < 0.0f)
               {
                  mBlockage = r.first;
                  break;
               }
            }
         }
         mImageBuilt = false;
      }
      Build();
   }
}

void TerrainTools::LOS_Ruler::EntityMovedCB(vespa::VaEntity* aEntityPtr, bool aMoving)
{
   Recalculate();
}

void TerrainTools::LOS_Ruler::PrivateInitializeCallbacks()
{
   if (!IsInitialized())
   {
      mCallbacks.Add(vespa::VaObserver::EntityMoved.Connect(&LOS_Ruler::EntityMovedCB, this, GetParent().GetUniqueId()));
      mCallbacks.Add(vespa::VaObserver::EntityMoved.Connect(&LOS_Ruler::EntityMovedCB, this, mTarget->GetUniqueId()));
   }
}

void TerrainTools::LOS_Ruler::PrivateLoad()
{
   if (mShapeName.empty())
   {
      static unsigned int uid = 0;
      mShapeName              = "LOS_Tool_" + std::to_string(uid++);
   }

   // Remove previous models
   RemoveShapeFromParent(mShapeName);
   mShapePtr = nullptr;

   UtoRawShape tempShape;
   tempShape.SetReferenceFrame(UtoShape::cFRAME_WORLD_AT_ENTITY);
   tempShape.OverrideRenderBin(1000, false);
   mShapePtr = dynamic_cast<UtoRawShape*>(AddShapeToParent(mShapeName, tempShape));
   assert(nullptr != mShapePtr);

   osg::Group*              grp    = mShapePtr->GetTree();
   osg::ref_ptr<osg::Geode> geode1 = new osg::Geode();
   osg::ref_ptr<osg::Geode> geode2 = new osg::Geode();
   grp->addChild(geode1);
   grp->addChild(geode2);
   mGeometry = new osg::Geometry;
   mGeometry->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   mGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::LineWidth(2.0));
   geode1->addDrawable(mGeometry);
   geode2->addDrawable(mGeometry);

   geode1->getOrCreateStateSet()->setAttributeAndModes(new osg::LineStipple(1, 0xF0F0));
   geode1->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::GREATER));
   geode1->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc());
   geode2->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::LEQUAL));
   geode2->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc());

   Recalculate();
}

void TerrainTools::LOS_Ruler::TargetDeletedCB(vespa::VaEntity* aEntityPtr)
{
   GetParent().DeleteLater(this);
}
