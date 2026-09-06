/**
 * @file OperationalDetailTabsTest.cpp
 * @brief Verifies that operator summaries are the default while raw evidence remains available.
 */

#include "NrmOperationalDetailTabs.hpp"

#include <cassert>

#include <QApplication>
#include <QTableWidget>
#include <QWidget>

int main(int argc, char** argv)
{
   qputenv("QT_QPA_PLATFORM", "offscreen");
   QApplication application(argc, argv);

   QWidget root;
   root.resize(1100, 650);
   WkNrm::OperationalDetailTabs tabs(&root);
   tabs.setGeometry(0, 0, 1100, 650);
   QTableWidget summaryTable;
   QTableWidget detailTable;
   tabs.SetPages(&summaryTable, &detailTable,
                 QString::fromUtf8("运行摘要"),
                 QString::fromUtf8("技术明细"));

   root.show();
   application.processEvents();

   assert(tabs.count() == 2);
   assert(tabs.tabText(0) == QString::fromUtf8("运行摘要"));
   assert(tabs.tabText(1) == QString::fromUtf8("技术明细"));
   assert(tabs.currentWidget() == &summaryTable);
   assert(summaryTable.isVisible());
   assert(!detailTable.isVisible());
   assert(summaryTable.height() > 500);

   tabs.ShowTechnicalDetails();
   assert(tabs.currentWidget() == &detailTable);
   tabs.ShowSummary();
   assert(tabs.currentWidget() == &summaryTable);
   return 0;
}
