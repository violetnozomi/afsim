// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// This software is provided 'As Is', with no warranties, either express or
// implied, and Infoscitex will not be liable for any damages arising in any way
// from the use of this software.
//
// WARNING: EXPORT CONTROLLED This document contains technical data whose
// export is restricted by the Arms Export Control Act (Title 22, U.S.C.
// Sec 2751 et seq) or Export Administration Act of 1979, as amended, Title 50
// U.S.C.; App. 2401, et seq.  Violators of these export laws are subject to
// severe criminal penalties. (DoD Directive 5230.25)
// ****************************************************************************

#include "MoverCreatorTabWidget.hpp"

#include <QEvent>
#include <QMouseEvent>
#include <QTabBar>

#include "AerodynamicsWidget.hpp"
#include "AuditErrorDialog.hpp"
#include "AutopilotWidget.hpp"
#include "CalculateAeroDialog.hpp"
#include "ControlsWidget.hpp"
#include "EngineDesignerWidget.hpp"
#include "GeometryWidget.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorTabs.hpp"
#include "MoverCreatorWidget.hpp"
#include "PerformanceWidget.hpp"
#include "Vehicle.hpp"

namespace Designer
{

MoverCreatorTabWidget::MoverCreatorTabWidget(QWidget* aParent)
   : QTabWidget(aParent)
{
   tabBar()->installEventFilter(this);
}

MoverCreatorTabWidget::~MoverCreatorTabWidget()
{

}

bool MoverCreatorTabWidget::eventFilter(QObject* aObj, QEvent* aEvent)
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();

   // No need to check for nullptr
   MoverCreatorWidget* mcWidget = mw.GetMoverCreatorWidget();

   if (aObj == tabBar() && (aEvent->type() == QEvent::MouseButtonPress))
   {
      if ((tabBar()->currentIndex() == Designer::cSETUP_TAB))
      {
         // Take no special action
      }
      else if ((tabBar()->currentIndex() == Designer::cENGINE_TAB))
      {
         EngineDesignerWidget* engineWidget = mcWidget->GetEngineDesignerWidget();
         QStringList errorList = engineWidget->Audit();
         if (errorList.size())
         {
            AuditErrorDialog errorDialog(this);
            errorDialog.AddErrors(errorList);
            errorDialog.exec();
            return true;
         }

         return mcWidget->GetEngineDesignerWidget()->CheckNeedsSave();
      }
      else if ((tabBar()->currentIndex() == Designer::cCONTROLS_TAB))
      {
         // Take no special action
      }
      else if ((tabBar()->currentIndex() == Designer::cGEOMETRY_TAB))
      {
         QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(aEvent);
         int curTabPos = tabBar()->tabAt(mouseEvent->pos());
         if (curTabPos == Designer::cAERODYNAMICS_TAB)
         {
            QStringList errorList = mcWidget->GetVehicle()->Audit();
            if (errorList.size())
            {
               AuditErrorDialog errorDialog(this);
               errorDialog.SetErrorLabel("Error: The following items must be addressed before vehicle aerodynamics can be calculated:");
               errorDialog.AddErrors(errorList);
               errorDialog.exec();
               return true;
            }

            mcWidget->MoveBetweenTabs();

            GeometryWidget* geometryWidget = mcWidget->GetGeometryWidget();
            if (geometryWidget != nullptr)
            {
               geometryWidget->TabTransition(Designer::cAERODYNAMICS_TAB);
            }
         }
         else
         {
            mcWidget->MoveBetweenTabs();
         }

         return false;
      }
      else if ((tabBar()->currentIndex() == Designer::cAERODYNAMICS_TAB))
      {
         mcWidget->MoveBetweenTabs();

         QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(aEvent);
         int curTabPos = tabBar()->tabAt(mouseEvent->pos());
         if (curTabPos == Designer::cPERFORMANCE_TAB)
         {
            AerodynamicsWidget* aeroWidget = mcWidget->GetAerodynamicsWidget();
            if (aeroWidget != nullptr)
            {
               aeroWidget->TabTransition(Designer::cPERFORMANCE_TAB);
            }
         }
         return false;
      }
      else if ((tabBar()->currentIndex() == Designer::cPERFORMANCE_TAB))
      {
         mcWidget->MoveBetweenTabs();
      }
      else if ((tabBar()->currentIndex() == Designer::cAUTOPILOT_TAB))
      {
         mcWidget->MoveBetweenTabs();
      }
      else if ((tabBar()->currentIndex() == Designer::cFLIGHT_TEST_TAB))
      {
         mcWidget->MoveBetweenTabs();
      }
   }
   return false;
}

} // namespace Designer
