#include "NrmPlugin.hpp"

#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"

#include <QDockWidget>
#include <QByteArray>
#include <QMainWindow>

WKF_PLUGIN_DEFINE_SYMBOLS(
   WkNrm::Plugin,
   "网络资源管理器",
   "显示AFSIM通信资源采集、评估与建议功能的运行状态。",
   "warlock")

WkNrm::Plugin::Plugin(const QString& aPluginName, size_t aUniqueId)
   : warlock::PluginT<SimInterface>(aPluginName, aUniqueId)
   , mDockWidgetPtr(new DockWidget(mData, wkfEnv.GetMainWindow()))
   , mTacticalViewPtr(new TacticalView(mData, wkfEnv.GetMainWindow()))
{
   wkfEnv.GetMainWindow()->addDockWidget(Qt::RightDockWidgetArea, mDockWidgetPtr);
   mDockWidgetPtr->show();

   const QByteArray autoPlan = qgetenv("NRM_AUTO_PLAN_FILE");
   if (!autoPlan.isEmpty())
   {
      mData.LoadNetworkPlan(autoPlan.constData());
      mDockWidgetPtr->ShowNetworkPlan();
      qunsetenv("NRM_AUTO_PLAN_FILE");
   }

   QMainWindow* centralDockerPtr = wkfEnv.GetMainWindow()->centralWidget();
   if (centralDockerPtr != nullptr)
   {
      auto* tacticalDockPtr = new QDockWidget(QString::fromUtf8("AFSIM通信资源态势"), centralDockerPtr);
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
