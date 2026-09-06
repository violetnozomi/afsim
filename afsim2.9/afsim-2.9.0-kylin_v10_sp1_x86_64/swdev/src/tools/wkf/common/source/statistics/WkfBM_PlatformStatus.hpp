// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WKF_BM_PLATFORM_STATUS_HPP
#define WKF_BM_PLATFORM_STATUS_HPP

#include "wkf_common_export.h"

#include <QDialog>

#include "VaEntityPtr.hpp"
#include "WkfBM_PlatformStatusModel.hpp"
#include "WkfBM_PlatformStatusPrefObject.hpp"
#include "WkfBM_RuleSet.hpp"
#include "WkfPlatform.hpp"
#include "misc/WkfExportHelper.hpp"
#include "ui_WkfBM_PlatformStatus.h"

class QMenu;

namespace wkf
{
class Platform;

namespace bm
{
class PlatformPlotDatabase;
const QString cAUX_DATA_PREFIX = "aux_data - ";

class WKF_COMMON_EXPORT PlatformStatus : public QDialog
{
   Q_OBJECT

public:
   using Mode = ColorRule::Mode;

   explicit PlatformStatus(QWidget*                    aParent,
                           PlatformStatusPrefObject&   aPrefObject,
                           const PlatformPlotDatabase* aDatabase) noexcept;

   ~PlatformStatus() noexcept override = default;

   //! Adds a row to the model.
   //! The added platform row is automatically grouped by side and type.
   void AddPlatform(const wkf::Platform* aPlatform) noexcept;

   //! Adds a column to the model.
   //! Takes ownership of aRuleSet.
   void AddRuleSet(wkf::bm::RuleSet* aRuleSet) noexcept;

   //! Clears the current platforms
   void ClearPlatforms() noexcept;

   void ReadUserSettings();
   void WriteUserSettings() const;

signals:
   //! Requests that a new column be added for aux data.
   void AddAuxDataColumn(QString aKey, RuleSet::Mode aMode);

   //! Requests that a new column be added for aux data.
   void AddResourceAuxDataColumn(QString aNumerator, QString aDenominator);

public slots:
   //! For the given platform, rescans all values from RuleSets.
   //! Does nothing if the platform has not been added.
   void PlatformChanged(const wkf::Platform* aPlatform);

   //! For the given RuleSet, rescans all platforms.
   //! Should not be used for RuleSets that have not been added.
   void RuleSetChanged(const wkf::bm::RuleSet* aRuleSet);

   //! Rescans all values for all platforms.
   void RescanAll(double aSimTime);

private slots:
   // void ComboBoxChanged(QString aTitle);

   void RestoreDefaultsClicked();

   void GoToRuleSet(int aIndex);

   void AddRuleSetClicked();
   void RemoveRuleSet(RuleSet* aRuleSet);

   void HideDataColumn(RuleSet* aRuleSet);
   void ShowDataColumn(RuleSet* aRuleSet);

   void CustomContextMenu(const QPoint& aPoint);

   void WriteState(QString aName) const;
   void ReadState(QString aName);

   //! Saves the current rule sets to aSettings.
   Configuration ExportState() const;

   //! Resets the current rule sets to match what is in aSettings.
   void ImportState(const Configuration& aConfiguration);

   //! Adds an action to the item that removes the current column.
   //! aPoint should be relative to *this.
   void RemoveColumnMenuItem(QMenu& aMenu, int aColumn);
   //! Allows the normal context menus for the selected platforms to be accessed from the dialog.
   void BuildPlatformContextMenu(QMenu& aMenu);

   void TableSelectionChanged(const QItemSelection& aSelected, const QItemSelection& aDeselected);

   void PlatformSelectionChanged(const wkf::PlatformList& aSelected, const wkf::PlatformList& aDeselected);

private:
   Ui::WkfBM_PlatformStatus           mUi;
   wkf::ExportHelper                  mExportHelper;
   wkf::bm::PlatformStatusPrefObject& mPrefObject;

   //! The model used by mUi.tableView
   wkf::bm::PlatformStatusModel mModel;

   //! First value is the model index.
   std::vector<RuleSet*> mRuleSets;

   //! Prevents infinite loops in selection changed callbacks.
   bool mInSelectionCallback = false;

   void UpdatePlatform(const wkf::Platform* aPlatform);
   int  CurrentRuleSetIndex() const noexcept;
   //! Returns the index of aRuleSet.
   //! If not found, returns -1.
   int GetRuleSetIndex(const RuleSet* aRuleSet) const noexcept;

   //! Removes all RuleSets that are removable.
   void RemoveRemovableRuleSets();
};
} // namespace bm
} // namespace wkf

#endif
