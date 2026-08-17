#ifndef NRM_PLUGIN_HPP
#define NRM_PLUGIN_HPP

#include "NrmDataContainer.hpp"
#include "NrmDockWidget.hpp"
#include "NrmSimInterface.hpp"
#include "NrmTacticalView.hpp"
#include "UtQtUiPointer.hpp"
#include "WkPlugin.hpp"

#include <QDockWidget>

namespace WkNrm
{
class Plugin : public warlock::PluginT<SimInterface>
{
   Q_OBJECT

public:
   Plugin(const QString& aPluginName, size_t aUniqueId);

protected:
   void GuiUpdate() override;

private:
   DataContainer mData;
   ut::qt::UiPointer<DockWidget>   mDockWidgetPtr;
   ut::qt::UiPointer<QDockWidget>  mTacticalDockPtr;
};
} // namespace WkNrm

#endif
