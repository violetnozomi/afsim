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
{
   wkfEnv.GetMainWindow()->addDockWidget(Qt::RightDockWidgetArea, mDockWidgetPtr);
   mDockWidgetPtr->show();

   const QByteArray autoPlan = qgetenv("NRM_AUTO_PLAN_FILE");
   if (!autoPlan.isEmpty())
   {
      mData.LoadNetworkPlan(autoPlan.constData());
      if (qgetenv("NRM_AUTO_OPEN_PAGE") == "acceptance")
      {
         mDockWidgetPtr->ShowAcceptanceDemo();
      }
      else
      {
         mDockWidgetPtr->ShowNetworkPlan();
      }
      qunsetenv("NRM_AUTO_PLAN_FILE");
      qunsetenv("NRM_AUTO_OPEN_PAGE");
   }

   QMainWindow* centralDockerPtr = wkfEnv.GetMainWindow()->centralWidget();
   if (centralDockerPtr != nullptr)
   {
      mTacticalDockPtr = new QDockWidget(
         QString::fromUtf8("AFSIM通信资源态势"), centralDockerPtr);
      mTacticalDockPtr->setObjectName("NrmTacticalViewDockWidget");
      mTacticalDockPtr->setFeatures(QDockWidget::NoDockWidgetFeatures);
      TacticalView* tacticalViewPtr = new TacticalView(mData, mTacticalDockPtr);
      mTacticalDockPtr->setWidget(tacticalViewPtr);
      connect(tacticalViewPtr, &TacticalView::PlatformSelected,
              mDockWidgetPtr, &DockWidget::ApplyTacticalPlatformSelection);
      connect(tacticalViewPtr, &TacticalView::SelectionTargetChanged,
              mDockWidgetPtr, &DockWidget::SetTacticalSelectionTarget);
      connect(mDockWidgetPtr, &DockWidget::TacticalSelectionRequested,
              tacticalViewPtr, &TacticalView::BeginAssessmentSelection);
      connect(mDockWidgetPtr, &DockWidget::AssessmentPlatformsChanged,
              tacticalViewPtr, &TacticalView::SetAssessmentPlatforms);
      connect(mDockWidgetPtr, &DockWidget::UiScaleChanged,
              tacticalViewPtr, &TacticalView::SetUiScalePercent);
      connect(tacticalViewPtr, &TacticalView::UiScalePercentChanged,
              mDockWidgetPtr, &DockWidget::SetUiScalePercent);
      centralDockerPtr->addDockWidget(Qt::LeftDockWidgetArea, mTacticalDockPtr);
      mTacticalDockPtr->show();
   }
}

void WkNrm::Plugin::GuiUpdate()
{
   mInterfacePtr->ProcessEvents(mData);
}
