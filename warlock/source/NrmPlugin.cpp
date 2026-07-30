#include "NrmPlugin.hpp"

#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(
   WkNrm::Plugin,
   "Network Resource Manager",
   "Displays the framework status for AFSIM communication resource collection, assessment, and recommendation.",
   "warlock")

WkNrm::Plugin::Plugin(const QString& aPluginName, size_t aUniqueId)
   : warlock::PluginT<SimInterface>(aPluginName, aUniqueId)
   , mDockWidgetPtr(new DockWidget(mData, wkfEnv.GetMainWindow()))
{
   wkfEnv.GetMainWindow()->addDockWidget(Qt::RightDockWidgetArea, mDockWidgetPtr);
   mDockWidgetPtr->hide();
}

void WkNrm::Plugin::GuiUpdate()
{
   mInterfacePtr->ProcessEvents(mData);
}

