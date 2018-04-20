#pragma once

#include "MultiscaleFusion/MultiscaleFusionPlugin.h"

class MultiscaleFusionGuiPlugin : public MultiscaleFusionPlugin
{
  Q_OBJECT
  Q_INTERFACES(ISIMPLibPlugin)
  Q_PLUGIN_METADATA(IID "net.bluequartz.dream3d.MultiscaleFusionGuiPlugin")

public:
  MultiscaleFusionGuiPlugin();
  ~MultiscaleFusionGuiPlugin() override;

public:
  MultiscaleFusionGuiPlugin(const MultiscaleFusionGuiPlugin&) = delete;            // Copy Constructor Not Implemented
  MultiscaleFusionGuiPlugin(MultiscaleFusionGuiPlugin&&) = delete;                 // Move Constructor
  MultiscaleFusionGuiPlugin& operator=(const MultiscaleFusionGuiPlugin&) = delete; // Copy Assignment Not Implemented
  MultiscaleFusionGuiPlugin& operator=(MultiscaleFusionGuiPlugin&&) = delete;      // Move Assignment Not Implemented
};
