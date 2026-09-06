// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2014 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTQTPLOTVERTICALLABEL_HPP
#define UTQTPLOTVERTICALLABEL_HPP

#include "utilqt_export.h"

#include <QLabel>

class QPaintEvent;

//! A vertical label. The text is rotated 90 degrees counter-clockwise. This is
//! used for (for example) the Y-axis label of a plot. No other vertical
//! orientations are currently supported.
//!
//! Based on answers from this page:
//! http://stackoverflow.com/questions/9183050/vertical-qlabel-or-the-equivalent
class UTILQT_EXPORT UtQtVerticalLabel : public QLabel
{
   Q_OBJECT

public:
   explicit UtQtVerticalLabel(QWidget* aParent = nullptr);

protected:
   void  paintEvent(QPaintEvent* aEvent) override;
   QSize sizeHint() const override;
   QSize minimumSizeHint() const override;
};

#endif // UTQTPLOTVERTICALLABEL_HPP
