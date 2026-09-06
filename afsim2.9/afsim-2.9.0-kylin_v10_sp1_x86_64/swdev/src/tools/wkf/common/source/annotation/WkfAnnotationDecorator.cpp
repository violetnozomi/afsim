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

#include "WkfAnnotationDecorator.hpp"

Annotation::AnnotationDecorator::AnnotationDecorator(vespa::VaEntity& aParent,
                                                     vespa::VaViewer* aViewer,
                                                     const QImage&    aImage,
                                                     const QString&   aImageDescription)
   : wkf::AttachmentDecorator(aParent, aViewer, aImage, aImageDescription)
{
   SetType<AnnotationDecorator>();
}

Annotation::AnnotationDecorator::AnnotationDecorator(vespa::VaEntity& aParent, vespa::VaViewer* aViewer, const QString& aText)
   : wkf::AttachmentDecorator(aParent, aViewer, aText)
{
   SetType<AnnotationDecorator>();
}
