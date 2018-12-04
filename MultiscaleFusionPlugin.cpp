/* © 2018 Kitware Inc. Provided under Apache 2.0 license. */

#include "MultiscaleFusion/MultiscaleFusionVersion.h"
#include "MultiscaleFusionPlugin.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/IFilterFactory.hpp"

#include "MultiscaleFusion/MultiscaleFusionConstants.h"

// Include the MOC generated file for this class
#include "moc_MultiscaleFusionPlugin.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiscaleFusionPlugin::MultiscaleFusionPlugin()
: m_Version(MultiscaleFusion::Version::Package())
, m_CompatibilityVersion(MultiscaleFusion::Version::Package())
, m_Vendor("Kitware Inc.")
, m_URL("http://www.github.com/bluequartzsoftware/MultiscaleFusion")
, m_Location("")
, m_Description("")
, m_Copyright("© 2018 Kitware Inc.")
, m_Filters(QList<QString>()) // Initialize MultiscaleFusion's List of Dependencies Here
, m_DidLoad(false)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiscaleFusionPlugin::~MultiscaleFusionPlugin() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiscaleFusionPlugin::getPluginName()
{
  return (MultiscaleFusionConstants::MultiscaleFusionPluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiscaleFusionPlugin::getVersion()
{
  return m_Version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiscaleFusionPlugin::getCompatibilityVersion()
{
  return m_CompatibilityVersion;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiscaleFusionPlugin::getVendor()
{
  return m_Vendor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiscaleFusionPlugin::getURL()
{
  return m_URL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiscaleFusionPlugin::getLocation()
{
  return m_Location;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiscaleFusionPlugin::getDescription()
{
  /* PLEASE UPDATE YOUR PLUGIN'S DESCRIPTION FILE.
  It is located at MultiscaleFusion/Resources/MultiscaleFusion/MultiscaleFusionDescription.txt */

  QFile licenseFile(":/MultiscaleFusion/MultiscaleFusionDescription.txt");
  QFileInfo licenseFileInfo(licenseFile);
  QString text = "<<--Description was not read-->>";

  if(licenseFileInfo.exists())
  {
    if(licenseFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QTextStream in(&licenseFile);
      text = in.readAll();
    }
  }
  return text;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiscaleFusionPlugin::getCopyright()
{
  return m_Copyright;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiscaleFusionPlugin::getLicense()
{
  /* PLEASE UPDATE YOUR PLUGIN'S LICENSE FILE.
  It is located at MultiscaleFusion/Resources/MultiscaleFusion/MultiscaleFusionLicense.txt */

  QFile licenseFile(":/MultiscaleFusion/MultiscaleFusionLicense.txt");
  QFileInfo licenseFileInfo(licenseFile);
  QString text = "<<--License was not read-->>";

  if(licenseFileInfo.exists())
  {
    if(licenseFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QTextStream in(&licenseFile);
      text = in.readAll();
    }
  }
  return text;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMap<QString, QString> MultiscaleFusionPlugin::getThirdPartyLicenses()
{
  QMap<QString, QString> licenseMap;
  QList<QString> fileStrList;
  fileStrList.push_back(":/ThirdParty/HDF5.txt");
  fileStrList.push_back(":/ThirdParty/Boost.txt");
  fileStrList.push_back(":/ThirdParty/Qt.txt");
  fileStrList.push_back(":/ThirdParty/Qwt.txt");

  for(QList<QString>::iterator iter = fileStrList.begin(); iter != fileStrList.end(); iter++)
  {
    QFile file(*iter);
    QFileInfo licenseFileInfo(file);

    if(licenseFileInfo.exists())
    {
      if(file.open(QIODevice::ReadOnly | QIODevice::Text))
      {
        QTextStream in(&file);
        licenseMap.insert(licenseFileInfo.baseName(), in.readAll());
      }
    }
  }

  return licenseMap;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MultiscaleFusionPlugin::getDidLoad()
{
  return m_DidLoad;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiscaleFusionPlugin::setDidLoad(bool didLoad)
{
  m_DidLoad = didLoad;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiscaleFusionPlugin::setLocation(QString filePath)
{
  m_Location = filePath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiscaleFusionPlugin::writeSettings(QSettings& prefs)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiscaleFusionPlugin::readSettings(QSettings& prefs)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiscaleFusionPlugin::getPluginFileName()
{
  return MultiscaleFusionConstants::MultiscaleFusionPluginFile;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiscaleFusionPlugin::getPluginDisplayName()
{
  return MultiscaleFusionConstants::MultiscaleFusionPluginDisplayName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiscaleFusionPlugin::getPluginBaseName()
{
  return MultiscaleFusionConstants::MultiscaleFusionBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiscaleFusionPlugin::registerFilterWidgets(FilterWidgetManager* fwm)
{
}

#include "MultiscaleFusionFilters/RegisterKnownFilters.cpp"
