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
#ifndef UTQTDOCKAREA_HPP
#define UTQTDOCKAREA_HPP

#include "utilqt_export.h"

#include <functional>

#include <QScrollArea>

class UtQtDockControl;

//! Manages a list of controls which can be docked into a scroll area.
class UTILQT_EXPORT UtQtDockArea : public QScrollArea
{
public:
   friend class UtQtDockControl;

   UtQtDockArea(QWidget* p);
   ~UtQtDockArea() override;
   void             AddControl(UtQtDockControl* aDockControlPtr);
   void             AddControlAfter(UtQtDockControl* aWhereDockControlPtr, UtQtDockControl* aDockControlPtr);
   void             AddControlBefore(UtQtDockControl* aWhereDockControlPtr, UtQtDockControl* aDockControlPtr);
   UtQtDockControl* First() const;
   UtQtDockControl* Find(QString aTitle) const;
   UtQtDockControl* FindByName(const std::string& aName) const;

   QWidget*                             GetScrollArea() { return mScrolledArea; }
   const std::vector<UtQtDockControl*>& GetControls() { return mControls; }
   void                                 ReorderControls(const std::vector<UtQtDockControl*>& aControlOrder);
   std::function<void()>                ControlVisibleChange;

protected:
   void                          WindowMove(QPoint point, UtQtDockControl* aControlPtr);
   void                          ReorderControl(UtQtDockControl* aControl, int y);
   void                          UpdateLayout();
   QWidget*                      mScrolledArea;
   std::vector<UtQtDockControl*> mControls;
};

#endif
