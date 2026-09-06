// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef ANNOTATION_DECORATOR_HPP
#define ANNOTATION_DECORATOR_HPP

#include "WkfAttachmentDecorator.hpp"

namespace Annotation
{
class AnnotationDecorator : public wkf::AttachmentDecorator
{
public:
   AnnotationDecorator(vespa::VaEntity& aParent,
                       vespa::VaViewer* aViewer,
                       const QImage&    aImage,
                       const QString&   aImageDescription);

   AnnotationDecorator(vespa::VaEntity& aParent, vespa::VaViewer* aViewer, const QString& aText);

   bool GetExportable() const { return mExportable; }
   void SetExportable(bool aExportable) { mExportable = aExportable; }

private:
   bool mExportable{true};
};
} // namespace Annotation

VA_DECLARE_OBJECT_TYPE(Annotation::AnnotationDecorator)

#endif // !ANNOTATION_DECORATOR_HPP
