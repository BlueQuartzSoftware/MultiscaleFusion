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


#pragma once

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

#include "ITKImageProcessing/ITKImageProcessingFilters/ITKImageBase.h"

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/SIMPLib.h"

#include "itkLinearInterpolateImageFunction.h"
#include "itkPhaseCorrelationImageRegistrationMethod.h"
#include "itkMaxPhaseCorrelationOptimizer.h"

#include "MultiscaleFusion/MultiscaleFusionDLLExport.h"

/**
 * @brief The ITKPhaseCorrelationImageRegistration class. See [Filter documentation](@ref ITKPhaseCorrelationImageRegistration) for details.
 */
class MultiscaleFusion_EXPORT ITKPhaseCorrelationImageRegistration : public ITKImageBase
{
  Q_OBJECT
  PYB11_CREATE_BINDINGS(ITKPhaseCorrelationImageRegistration SUPERCLASS AbstractFilter)
  PYB11_PROPERTY(DataArrayPath SelectedCellArrayPath READ getSelectedCellArrayPath WRITE setSelectedCellArrayPath)
  PYB11_PROPERTY(DataArrayPath MovingCellArrayPath READ getMovingCellArrayPath WRITE setMovingCellArrayPath)

public:
  SIMPL_SHARED_POINTERS(ITKPhaseCorrelationImageRegistration)
  SIMPL_FILTER_NEW_MACRO(ITKPhaseCorrelationImageRegistration)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ITKPhaseCorrelationImageRegistration, AbstractFilter)

  SIMPL_FILTER_PARAMETER(DataArrayPath, SelectedCellArrayPath)
  Q_PROPERTY(DataArrayPath SelectedCellArrayPath READ getSelectedCellArrayPath WRITE setSelectedCellArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, MovingCellArrayPath)
  Q_PROPERTY(DataArrayPath MovingCellArrayPath READ getMovingCellArrayPath WRITE setMovingCellArrayPath)

  virtual ~ITKPhaseCorrelationImageRegistration();

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  const QString getHumanLabel() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
  */
  const QString getBrandingString() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getSubGroupName() const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getGroupName() const override;

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  const QString getCompiledLibraryName() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  const QString getFilterVersion() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  const QUuid getUuid() override;

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  void setupFilterParameters() override;

  /**
   * @brief readFilterParameters Reimplemented from @see AbstractFilter class
   */
  void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

protected:
  ITKPhaseCorrelationImageRegistration();

  /**
   * @brief dataCheckInternal overloads dataCheckInternal in ITKImageBase and calls templated dataCheck
   */
  void dataCheckInternal() override;

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  template <typename InputImageType, typename OutputImageType, unsigned int Dimension> void dataCheck();

  /**
  * @brief filterInternal overloads filterInternal in ITKImageBase and calls templated filter
  */
  void filterInternal() override;

  /**
  * @brief Applies the filter
  */
  template <typename InputImageType, typename OutputImageType, unsigned int Dimension> void filter();

public:
  ITKPhaseCorrelationImageRegistration(const ITKPhaseCorrelationImageRegistration&) = delete; // Copy Constructor Not Implemented
  ITKPhaseCorrelationImageRegistration(ITKPhaseCorrelationImageRegistration&&) = delete;      // Move Constructor Not Implemented
  ITKPhaseCorrelationImageRegistration& operator=(const ITKPhaseCorrelationImageRegistration&) = delete; // Copy Assignment Not Implemented
  ITKPhaseCorrelationImageRegistration& operator=(ITKPhaseCorrelationImageRegistration&&) = delete;      // Move Assignment Not Implemented
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

