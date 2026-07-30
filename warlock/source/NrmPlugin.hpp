#ifndef NRM_PLUGIN_HPP
#define NRM_PLUGIN_HPP

#include "NrmDataContainer.hpp"
#include "NrmDockWidget.hpp"
#include "NrmSimInterface.hpp"
#include "WkPlugin.hpp"

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
   DockWidget*   mDockWidgetPtr;
};
} // namespace WkNrm

#endif

