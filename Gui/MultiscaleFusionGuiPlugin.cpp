

#include "MultiscaleFusionGuiPlugin.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiscaleFusionGuiPlugin::MultiscaleFusionGuiPlugin()
: MultiscaleFusionPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiscaleFusionGuiPlugin::~MultiscaleFusionGuiPlugin() = default;

#include "MultiscaleFusion/Gui/FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"
