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

#ifndef RVBAT_DOCKWINDOW_HPP
#define RVBAT_DOCKWINDOW_HPP

#include <memory>

#include "RvBAT_Blackboard.hpp"
#include "RvBAT_Interface.hpp"
#include "RvBAT_MovableView.hpp"
#include "UtMemory.hpp"
#include "WkfDockWidget.hpp"

namespace rv
{
class ResultData;
}

class QComboBox;
class QPushButton;
namespace RvBAT
{
/** Minimum window size in pixels. */
const QSize cMINIMUM_WINDOW_SIZE = QSize(400, 400);

/** Default window size in pixels. */
const QSize cDEFAULT_WINDOW_SIZE = QSize(800, 600);

class DockWindow : public wkf::DockWidget
{
   Q_OBJECT

public:
   explicit DockWindow(const QString& aObjectName, QMainWindow* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

   /** Update the Interface and View.
    * @param aData data to use for updating.
    */
   void Update(const rv::ResultData& aData);

   /** Set whether the window needs an update.
    * @param aState state to set.
    */
   void SetNeedsUpdate(bool aState) { mNeedsUpdate = aState; }

   /** Get if the window needs an update. */
   bool GetNeedsUpdate() { return mNeedsUpdate; }

   /** Called when Plugin::ClearScenario is called. */
   void Reset();

   /** Go to a combo box entry for a platform if it exists.
    * @param aStr string of the combo box entry
    */
   void GoToComboBox(const QString& aStr);
public slots:
   void comboBoxChangedIndex(const QString& aStr);
protected slots:
   /** Change event for a QWidget. This will be used to monitor theme changes.
    * @param aEvent event to check for a style change
    */
   virtual void changeEvent(QEvent* aEvent) override;

protected:
   /** Resize event for QDialog window.
    * @param aEvent Resize event to use
    */
   void resizeEvent(QResizeEvent* aEvent) override;

   /** Build the context menu.
    * @param aMenu menu to add to
    * @param aPos position to add the menu
    */
   bool BuildContextMenu(QMenu& aMenu, const QPoint& aPos) override;

   /** Toggle showing the blackboard tab to the right. */
   void ToggleBlackboard();

private:
   QComboBox* mPlatformComboBoxPtr; ///< Combo Box for selecting platform
   QComboBox* mSceneComboBoxPtr;    ///< Combo Box for selecting the scene(abt or fsm) to view for a platform
   std::unique_ptr<Interface>   mDataInterfacePtr;    ///< Pointer to the Interface.
   bool                         mThemeChanged{false}; ///< Has the theme changed?
   std::unique_ptr<MovableView> mViewPtr;             ///< Movable View Pointer
   std::unique_ptr<Blackboard>  mBlackboardPtr;       ///< Pointer to the Blackboard.
   QPushButton*                 mBlackboardButton;    ///< Pointer to the Blackboard button.
   bool                         mNeedsUpdate{false};  ///< True if the window needs to update.
};
} // namespace RvBAT
#endif // RVBAT_DOCKWINDOW_HPP
