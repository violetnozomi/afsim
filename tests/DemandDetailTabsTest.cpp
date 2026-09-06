/**
 * @file DemandDetailTabsTest.cpp
 * @brief Verifies that demand details use one large table per secondary tab.
 */

#include "NrmDemandDetailTabs.hpp"

#include <cassert>

#include <QApplication>
#include <QTableWidget>
#include <QWidget>

int main(int argc, char** argv)
{
   qputenv("QT_QPA_PLATFORM", "offscreen");
   QApplication application(argc, argv);

   QWidget root;
   root.resize(1200, 700);
   WkNrm::DemandDetailTabs tabs(&root);
   tabs.setGeometry(0, 0, 1200, 700);
   QTableWidget demandTable;
   QTableWidget matchTable;
   QTableWidget gapTable;
   QTableWidget recommendationTable;
   QTableWidget evidenceTable;
   tabs.SetPages(&demandTable, &matchTable, &gapTable, &recommendationTable,
                 &evidenceTable);

   root.show();
   application.processEvents();

   assert(tabs.count() == 5);
   assert(tabs.tabText(0) == QString::fromUtf8("业务需求"));
   assert(tabs.tabText(1) == QString::fromUtf8("匹配结果"));
   assert(tabs.tabText(2) == QString::fromUtf8("约束差距"));
   assert(tabs.tabText(3) == QString::fromUtf8("调整建议"));
   assert(tabs.tabText(4) == QString::fromUtf8("技术依据"));
   assert(tabs.currentWidget() == &demandTable);
   assert(demandTable.isVisible());
   assert(!matchTable.isVisible());
   assert(demandTable.height() > 500);

   tabs.ShowEvaluation(false, false);
   assert(tabs.currentWidget() == &matchTable);

   tabs.ShowEvaluation(true, false);
   assert(tabs.currentWidget() == &gapTable);

   tabs.ShowEvaluation(true, true);
   assert(tabs.currentWidget() == &recommendationTable);

   tabs.ShowTechnicalEvidence();
   assert(tabs.currentWidget() == &evidenceTable);

   tabs.ShowDemands();
   assert(tabs.currentWidget() == &demandTable);
   return 0;
}
