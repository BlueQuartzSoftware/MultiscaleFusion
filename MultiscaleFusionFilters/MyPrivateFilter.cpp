/*
 * Your License or Copyright can go here
 */

#include "MyPrivateFilter.h"

#include "SIMPLib/Common/Constants.h"



#include "MultiscaleFusion/MultiscaleFusionConstants.h"
#include "MultiscaleFusion/MultiscaleFusionVersion.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MyPrivateFilter::MyPrivateFilter() :
  AbstractFilter()
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MyPrivateFilter::~MyPrivateFilter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MyPrivateFilter::initialize()
{
  setErrorCondition(0);
  setWarningCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MyPrivateFilter::setupFilterParameters()
{
  FilterParameterVector parameters;

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MyPrivateFilter::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);
  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MyPrivateFilter::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MyPrivateFilter::execute()
{
  initialize();
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  if (getCancel() == true) { return; }

  if (getWarningCondition() < 0)
  {
    QString ss = QObject::tr("Some warning message");
    setWarningCondition(-88888888);
    notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
  }

  if (getErrorCondition() < 0)
  {
    QString ss = QObject::tr("Some error message");
    setErrorCondition(-99999999);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MyPrivateFilter::newFilterInstance(bool copyFilterParameters) const
{
  MyPrivateFilter::Pointer filter = MyPrivateFilter::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MyPrivateFilter::getCompiledLibraryName() const
{ 
  return MultiscaleFusionConstants::MultiscaleFusionBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MyPrivateFilter::getBrandingString() const
{
  return "MultiscaleFusion";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MyPrivateFilter::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  MultiscaleFusion::Version::Major() << "." << MultiscaleFusion::Version::Minor() << "." << MultiscaleFusion::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MyPrivateFilter::getGroupName() const
{ 
  return SIMPL::FilterGroups::Unsupported; 
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MyPrivateFilter::getSubGroupName() const
{ 
  return "MultiscaleFusion"; 
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MyPrivateFilter::getHumanLabel() const
{ 
  return "MyPrivateFilter"; 
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid MyPrivateFilter::getUuid()
{
  return QUuid("{52780b6d-e26b-549b-9768-34e883228168}");
}

