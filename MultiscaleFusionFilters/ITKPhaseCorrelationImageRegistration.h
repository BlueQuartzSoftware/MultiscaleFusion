// File automatically generated

/*
 * Your License or Copyright can go here
 */

#ifndef _ITKPhaseCorrelationImageRegistration_h_
#define _ITKPhaseCorrelationImageRegistration_h_

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


/**
 * @brief The ITKPhaseCorrelationImageRegistration class. See [Filter documentation](@ref ITKPhaseCorrelationImageRegistration) for details.
 */
class ITKPhaseCorrelationImageRegistration : public ITKImageBase
{
  Q_OBJECT

public:
  SIMPL_SHARED_POINTERS(ITKPhaseCorrelationImageRegistration)
  SIMPL_STATIC_NEW_MACRO(ITKPhaseCorrelationImageRegistration)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ITKPhaseCorrelationImageRegistration, AbstractFilter)

  SIMPL_FILTER_PARAMETER(DataArrayPath, SelectedCellArrayPath)
  Q_PROPERTY(DataArrayPath SelectedCellArrayPath READ getSelectedCellArrayPath WRITE setSelectedCellArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, MovingCellArrayPath)
  Q_PROPERTY(DataArrayPath MovingCellArrayPath READ getMovingCellArrayPath WRITE setMovingCellArrayPath)

  virtual ~ITKPhaseCorrelationImageRegistration();

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  virtual const QString getHumanLabel() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
  */
  virtual const QString getBrandingString() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  virtual const QString getSubGroupName() const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  virtual const QString getGroupName() const override;

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  virtual const QString getCompiledLibraryName() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  virtual const QString getFilterVersion() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  virtual const QUuid getUuid() override;

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  virtual void setupFilterParameters() override;

  /**
   * @brief readFilterParameters Reimplemented from @see AbstractFilter class
   */
  virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

protected:
  ITKPhaseCorrelationImageRegistration();

  /**
   * @brief dataCheckInternal overloads dataCheckInternal in ITKImageBase and calls templated dataCheck
   */
  void virtual dataCheckInternal() override;

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  template <typename InputImageType, typename OutputImageType, unsigned int Dimension> void dataCheck();

  /**
  * @brief filterInternal overloads filterInternal in ITKImageBase and calls templated filter
  */
  void virtual filterInternal() override;

  /**
  * @brief Applies the filter
  */
  template <typename InputImageType, typename OutputImageType, unsigned int Dimension> void filter();

private:
  ITKPhaseCorrelationImageRegistration(const ITKPhaseCorrelationImageRegistration&) = delete; // Copy Constructor Not Implemented
  void operator=(const ITKPhaseCorrelationImageRegistration&);                   // Operator '=' Not Implemented
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif /* _ITKPhaseCorrelationImageRegistration_H_ */
