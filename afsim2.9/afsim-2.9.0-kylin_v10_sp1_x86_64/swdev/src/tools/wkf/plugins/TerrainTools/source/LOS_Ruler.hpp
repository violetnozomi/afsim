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

#ifndef LOS_RULER_HPP
#define LOS_RULER_HPP

#include <QPointer>

#include "Uto.hpp"
#include "UtoRawShape.hpp"
#include "VaAttachmentQt.hpp"
#include "VaEntityPtr.hpp"
#include "VaLocationReference.hpp"
#include "WkfEntity.hpp"
#include "WkfPlotDockWidget.hpp"

namespace TerrainTools
{
class LOS_Ruler : public vespa::VaAttachmentQt
{
public:
   LOS_Ruler(wkf::Entity& aSource, wkf::Entity& aTarget, vespa::VaViewer& aViewerPtr);
   ~LOS_Ruler() override;

   bool Pick(vespa::VaViewer*                aViewerPtr,
             int                             aMouseX,
             int                             aMouseY,
             bool                            aToggle,
             bool                            aDoubleClick         = false,
             const vespa::VaAttachmentSubId& aAttachmentSubPartId = 0) override;

   unsigned int GetPickMode() const override { return vespa::VaAttachment::cPICK_ATTACHMENT; }

   void SetSelected(vespa::VaViewer*                aViewerPtr,
                    bool                            aSelected,
                    const vespa::VaAttachmentSubId& aSubId,
                    bool                            aForce = false) override;

   bool        IsHighlightable() override { return false; }
   std::string GetDescription(unsigned int aSubId) override;

   DeleteAction DeleteSelection(vespa::VaViewer* aViewerPtr) override
   {
      return vespa::VaAttachment::cDELETE_ATTACHMENT;
   }

   bool SupportsContextMenu() const override { return true; }
   void BuildContextMenu(QMenu& aMenu, unsigned int aSubPartId) override;

protected:
private:
   void EntityMovedCB(vespa::VaEntity* aEntityPtr, bool aMoving);
   void TargetDeletedCB(vespa::VaEntity* aEntityPtr);

   void PrivateInitializeCallbacks() override;
   void PrivateLoad() override;

   void Build();
   void BuildImage();
   void GetParentECEF(double aECEF[3]) const;

   void Recalculate();
   void ConfigureMastHeight();
   void ShowPlot();
   void UpdatePlottingWidget();

   vespa::EntityPtr<wkf::Entity> mTarget;
   std::string                   mShapeName;
   UtoRawShape*                  mShapePtr{nullptr};
   osg::ref_ptr<osg::Geometry>   mGeometry;
   size_t                        mRequesterId{0};
   std::map<float, float>        mResults;
   bool                          mImageBuilt{false};
   QByteArray                    mImageData;
   double                        mBlockage{2.0};
   double                        mMastHeight{0.0};
   QPointer<wkf::PlotDockWidget> mPlotDockWidgetPtr;
};
} // namespace TerrainTools

VA_DECLARE_OBJECT_TYPE(TerrainTools::LOS_Ruler)
#endif
