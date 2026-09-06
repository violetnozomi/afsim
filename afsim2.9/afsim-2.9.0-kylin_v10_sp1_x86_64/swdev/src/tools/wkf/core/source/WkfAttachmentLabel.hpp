// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WKFATTACHMENTLABEL_HPP
#define WKFATTACHMENTLABEL_HPP

#include "wkf_export.h"

#include <QColor>
#include <QFont>
#include <QString>

#include "UtoRawShape.hpp"
#include "VaAttachment.hpp"
#include "VaDecoratorNode.hpp"
#include "VaEntity.hpp"
#include "VaViewer.hpp"

namespace wkf
{
class WKF_EXPORT AttachmentLabel : public vespa::VaAttachment
{
public:
   AttachmentLabel(vespa::VaEntity& aParent, vespa::VaViewer* aViewerPtr);

   void PrivateLoad() override;

   void SetString(const QString& aString);
   void SetColor(const QColor& aColor);
   void SetFont(const QFont& aFont);
   void Setup(const QString& aString, const QColor& aColor, const QFont& aFont);

private:
   UtoRawShape*                         mShapePtr;
   std::string                          mShapeName;
   osg::ref_ptr<vespa::VaDecoratorNode> mTextNodePtr;
   QString                              mString;
   QColor                               mColor{255, 255, 255};
   QFont                                mFont;
};
} // namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::AttachmentLabel)

#endif
