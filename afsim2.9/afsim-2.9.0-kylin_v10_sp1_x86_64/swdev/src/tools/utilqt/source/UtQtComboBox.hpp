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
#ifndef UTQTCOMBOBOX_HPP
#define UTQTCOMBOBOX_HPP

#include "utilqt_export.h"

#include <QComboBox>
#include <QtCore/QTimer>
class QWheelEvent;

#include "UtCallback.hpp"

//! A QComboBox which supports a 'auto pop-up' mode where the menu pops up on mouse-over.
//! Also supports disabling of the mouse wheel, useful when a combo box is in a scroll area
class UTILQT_EXPORT UtQtComboBox : public QComboBox
{
public:
   explicit UtQtComboBox(QWidget* parent = nullptr);

   void mouseMoveEvent(QMouseEvent*) override;

   //! Executes prior to showing the popup
   //! @note only happens if auto pop-up is enabled and if the view is not visible
   UtCallbackListN<void()> BeginPopup;

   void SetAutoPopup(bool aAutoPopup);

   void DisableMouseWheel();

   //! Prevent the mouse wheel from scrolling the combo box when it isn't focus.
   void wheelEvent(QWheelEvent* event) override;

protected:
   void CheckMouseProximity();

   QTimer mCheckMouseTimer;
   bool   mAutoPopup;
   bool   mDisableMouseWheel;
};
#endif
