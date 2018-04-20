/*=========================================================================
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *=========================================================================*/

#include "ITKPhaseCorrelationImageRegistration.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

#include "SIMPLib/Geometry/ImageGeom.h"

#include <itkCastImageFilter.h>

#include "SIMPLib/ITK/Dream3DTemplateAliasMacro.h"
#include "SIMPLib/ITK/itkDream3DImage.h"

#include "MultiscaleFusion/MultiscaleFusionConstants.h"
#include "MultiscaleFusion/MultiscaleFusionVersion.h"
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ITKPhaseCorrelationImageRegistration::ITKPhaseCorrelationImageRegistration()
: ITKImageBase()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ITKPhaseCorrelationImageRegistration::~ITKPhaseCorrelationImageRegistration() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKPhaseCorrelationImageRegistration::setupFilterParameters()
{
  FilterParameterVector parameters;

  QStringList linkedProps;
  linkedProps << "NewCellArrayName";
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::Defaults::AnyPrimitive, SIMPL::Defaults::AnyComponentSize, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Fixed Attribute Array to filter", SelectedCellArrayPath, FilterParameter::RequiredArray, ITKPhaseCorrelationImageRegistration, req));
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Moving Attribute Array to filter", MovingCellArrayPath, FilterParameter::RequiredArray, ITKPhaseCorrelationImageRegistration, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKPhaseCorrelationImageRegistration::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayPath(reader->readDataArrayPath("SelectedCellArrayPath", getSelectedCellArrayPath()));
  setMovingCellArrayPath(reader->readDataArrayPath("MovingCellArrayPath", getMovingCellArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename InputPixelType, typename OutputPixelType, unsigned int Dimension> void ITKPhaseCorrelationImageRegistration::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  imageCheck<InputPixelType, Dimension>(getSelectedCellArrayPath());
  imageCheck<InputPixelType, Dimension>(getMovingCellArrayPath());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKPhaseCorrelationImageRegistration::dataCheckInternal()
{
  Dream3DArrayOutputComponentFromDataMacro(this->dataCheck, getMovingCellArrayPath(), getSelectedCellArrayPath(), -4);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

template <typename FixedPixelType, typename MovingPixelType, unsigned int Dimension> void ITKPhaseCorrelationImageRegistration::filter()
{
  typedef itk::Dream3DImage<FixedPixelType, Dimension> FixedImageType;
  typedef itk::Dream3DImage<MovingPixelType, Dimension> MovingImageType;
  //
  // init the registration method
  //

  typedef itk::PhaseCorrelationImageRegistrationMethod< FixedImageType, MovingImageType > RegistrationType;
  typename RegistrationType::Pointer filter = RegistrationType::New();

  typedef itk::PhaseCorrelationOperator< typename itk::NumericTraits< FixedPixelType >::RealType, Dimension >
    OperatorType;
  typename OperatorType::Pointer pcmOperator = OperatorType::New();
  filter->SetOperator( pcmOperator );

  typedef itk::MaxPhaseCorrelationOptimizer<RegistrationType> OptimizerType;
  typename OptimizerType::Pointer pcmOptimizer = OptimizerType::New();
  filter->SetOptimizer( pcmOptimizer );

  typedef itk::InPlaceDream3DDataToImageFilter<FixedPixelType, Dimension> FixedtoITKType;
  typedef itk::InPlaceDream3DDataToImageFilter<MovingPixelType, Dimension> MovingtoITKType;
  DataArrayPath dapMoving = getMovingCellArrayPath();
  DataContainer::Pointer dcMoving = getDataContainerArray()->getDataContainer(dapMoving.getDataContainerName());
  // Create a bridge to wrap an existing DREAM.3D array with an ItkImage container
  typename MovingtoITKType::Pointer toITKMoving = MovingtoITKType::New();
  toITKMoving->SetInput(dcMoving);
  toITKMoving->SetInPlace(true);
  toITKMoving->SetAttributeMatrixArrayName(getMovingCellArrayPath().getAttributeMatrixName().toStdString());
  toITKMoving->SetDataArrayName(getMovingCellArrayPath().getDataArrayName().toStdString());
  filter->SetMovingImage(toITKMoving->GetOutput());
  try
  {
    DataArrayPath dap = getSelectedCellArrayPath();
    DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(dap.getDataContainerName());

    // Create a Bridge to wrap an existing DREAM.3D array with an ItkImage container
    typename FixedtoITKType::Pointer toITK = FixedtoITKType::New();
    toITK->SetInput(dc);
    toITK->SetInPlace(true);
    toITK->SetAttributeMatrixArrayName(getSelectedCellArrayPath().getAttributeMatrixName().toStdString());
    toITK->SetDataArrayName(getSelectedCellArrayPath().getDataArrayName().toStdString());
    itk::Dream3DFilterInterruption::Pointer interruption = itk::Dream3DFilterInterruption::New();
    interruption->SetFilter(this);

    // Set up filter
    filter->SetFixedImage(toITK->GetOutput());
    filter->AddObserver(itk::ProgressEvent(), interruption);
    filter->Update();
  //
  // get the results
  //
    typename RegistrationType::ParametersType parameters
      = filter->GetTransformParameters();
    typename RegistrationType::TransformType::ConstPointer transform
      = filter->GetOutput()->Get();
    QString transformParameters = "Transform Parameters: ";
    for(unsigned int d = 0; d < Dimension; d++)
    {
      transformParameters += QString::number(parameters[d]) + " ";
    }
    notifyWarningMessage(getHumanLabel(), transformParameters, 0);
  } catch(itk::ExceptionObject& err)
  {
    setErrorCondition(-55558);
    QString errorMessage = "ITK exception was thrown while filtering input image: %1";
    notifyErrorMessage(getHumanLabel(), errorMessage.arg(err.GetDescription()), getErrorCondition());
    return;
  }
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKPhaseCorrelationImageRegistration::filterInternal()
{
  Dream3DArrayOutputComponentFromDataMacro(this->filter, getMovingCellArrayPath(), getSelectedCellArrayPath(), -4);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ITKPhaseCorrelationImageRegistration::newFilterInstance(bool copyFilterParameters) const
{
  ITKPhaseCorrelationImageRegistration::Pointer filter = ITKPhaseCorrelationImageRegistration::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKPhaseCorrelationImageRegistration::getHumanLabel() const
{
  return "MultiscaleFusion::Phase Correlation Image Registration";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid ITKPhaseCorrelationImageRegistration::getUuid()
{
  return QUuid("{a0d962b7-9d5c-5abc-a078-1fe795df4663}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKPhaseCorrelationImageRegistration::getCompiledLibraryName() const
{
  return MultiscaleFusionConstants::MultiscaleFusionBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKPhaseCorrelationImageRegistration::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  MultiscaleFusion::Version::Major() << "." << MultiscaleFusion::Version::Minor() << "." << MultiscaleFusion::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKPhaseCorrelationImageRegistration::getGroupName() const
{
  return "Image Processing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKPhaseCorrelationImageRegistration::getSubGroupName() const
{
  return MultiscaleFusionConstants::FilterGroups::MultiscaleFusionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKPhaseCorrelationImageRegistration::getBrandingString() const
{
  return MultiscaleFusionConstants::MultiscaleFusionBaseName;
}
