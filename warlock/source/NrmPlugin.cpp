#include "NrmPlugin.hpp"

#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"

#include <QDockWidget>
#include <QMainWindow>

WKF_PLUGIN_DEFINE_SYMBOLS(
   WkNrm::Plugin,
   "Network Resource Manager",
   "Displays the framework status for AFSIM communication resource collection, assessment, and recommendation.",
   "warlock")

WkNrm::Plugin::Plugin(const QString& aPluginName, size_t aUniqueId)
   : warlock::PluginT<SimInterface>(aPluginName, aUniqueId)
   , mDockWidgetPtr(new DockWidget(mData, wkfEnv.GetMainWindow()))
   , mTacticalViewPtr(new TacticalView(mData, wkfEnv.GetMainWindow()))
{
   wkfEnv.GetMainWindow()->addDockWidget(Qt::RightDockWidgetArea, mDockWidgetPtr);
   mDockWidgetPtr->show();

   QMainWindow* centralDockerPtr = wkfEnv.GetMainWindow()->centralWidget();
   if (centralDockerPtr != nullptr)
   {
      auto* tacticalDockPtr = new QDockWidget("AFSIM Operational Network View", centralDockerPtr);
      tacticalDockPtr->setObjectName("NrmTacticalViewDockWidget");
      tacticalDockPtr->setFeatures(QDockWidget::NoDockWidgetFeatures);
      tacticalDockPtr->setWidget(mTacticalViewPtr);
      centralDockerPtr->addDockWidget(Qt::LeftDockWidgetArea, tacticalDockPtr);
      tacticalDockPtr->show();
   }
}

void WkNrm::Plugin::GuiUpdate()
{
   mInterfacePtr->ProcessEvents(mData);
}
