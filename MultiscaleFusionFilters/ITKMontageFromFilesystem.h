/*
 * Your License or Copyright can go here
 */

#pragma once

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/FilterParameters/FileListInfoFilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/ITK/itkImageReaderHelper.h"
#include "SIMPLib/SIMPLib.h"

#include "itkImageFileReader.h"

#include <istream>

#include "MultiscaleFusion/MultiscaleFusionDLLExport.h"

/**
 * @brief The ITKMontageFromFilesystem class. See [Filter documentation](@ref itkmontagefromfilesystem) for details.
 */
class MultiscaleFusion_EXPORT ITKMontageFromFilesystem : public AbstractFilter
{
  Q_OBJECT
  PYB11_CREATE_BINDINGS(ITKMontageFromFilesystem SUPERCLASS AbstractFilter)
  PYB11_PROPERTY(IntVec3_t MontageSize READ getMontageSize WRITE setMontageSize)
  // PYB11_PROPERTY(FloatVec3_t Origin READ getOrigin WRITE setOrigin)
  // PYB11_PROPERTY(FloatVec3_t Resolution READ getResolution WRITE setResolution)
  PYB11_PROPERTY(FileListInfo_t InputFileListInfo READ getInputFileListInfo WRITE setInputFileListInfo)
  PYB11_PROPERTY(QString DataContainerName READ getDataContainerName WRITE setDataContainerName)
  PYB11_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)
  PYB11_PROPERTY(QString MetaDataAttributeMatrixName READ getMetaDataAttributeMatrixName WRITE setMetaDataAttributeMatrixName)

public:
  SIMPL_SHARED_POINTERS(ITKMontageFromFilesystem)
  SIMPL_FILTER_NEW_MACRO(ITKMontageFromFilesystem)
  SIMPL_TYPE_MACRO_SUPER(ITKMontageFromFilesystem, AbstractFilter)

  ITK_DISALLOW_COPY_AND_ASSIGN(ITKMontageFromFilesystem);
  ~ITKMontageFromFilesystem() override;

  SIMPL_FILTER_PARAMETER(IntVec3_t, MontageSize)
  Q_PROPERTY(IntVec3_t MontageSize READ getMontageSize WRITE setMontageSize)

  SIMPL_FILTER_PARAMETER(FileListInfo_t, InputFileListInfo)
  Q_PROPERTY(FileListInfo_t InputFileListInfo READ getInputFileListInfo WRITE setInputFileListInfo)

  SIMPL_FILTER_PARAMETER(QString, DataContainerName)
  Q_PROPERTY(QString DataContainerName READ getDataContainerName WRITE setDataContainerName)

  SIMPL_FILTER_PARAMETER(QString, CellAttributeMatrixName)
  Q_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

  SIMPL_FILTER_PARAMETER(QString, MetaDataAttributeMatrixName)
  Q_PROPERTY(QString MetaDataAttributeMatrixName READ getMetaDataAttributeMatrixName WRITE setMetaDataAttributeMatrixName)

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  const QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
   */
  const QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  const QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  const QUuid getUuid() override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  const QString getHumanLabel() const override;

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  void setupFilterParameters() override;

  /**
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;

  /**
   * @brief preflight Reimplemented from @see AbstractFilter class
   */
  void preflight() override;

signals:
  /**
   * @brief updateFilterParameters Emitted when the Filter requests all the latest Filter parameters
   * be pushed from a user-facing control (such as a widget)
   * @param filter Filter instance pointer
   */
  void updateFilterParameters(AbstractFilter* filter);

  /**
   * @brief parametersChanged Emitted when any Filter parameter is changed internally
   */
  void parametersChanged();

  /**
   * @brief preflightAboutToExecute Emitted just before calling dataCheck()
   */
  void preflightAboutToExecute();

  /**
   * @brief preflightExecuted Emitted just after calling dataCheck()
   */
  void preflightExecuted();

protected:
  ITKMontageFromFilesystem();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief Cancel the operation
   */
  void setCancel(bool value) override;

  static constexpr unsigned Dimension = 2;
  using PointType = itk::Point<double, Dimension>;
  using PositionTableType = std::vector<std::vector<PointType>>;
  using FilenameTableType = std::vector<std::vector<std::string>>;

  /**
   * @brief Loads montage input configuration from a TileConfiguration.txt file
   */
  void loadTileConfiguration(std::string dirPath, unsigned xSize, unsigned ySize, PositionTableType& pos, FilenameTableType& files);

  /**
   * @brief Does the registrations and resampling into a big image.
   *
   * peakMethodToUse: 0=none, 1=parabolic, 2=cosine
   */
  template <typename PixelType, typename AccumulatePixelType = typename itk::NumericTraits<PixelType>::AccumulateType>
  void doMontage(const PositionTableType& tilePositions, const FilenameTableType& filenames, int peakMethodToUse=1, unsigned streamSubdivisions = 1);

private:
  itk::ProcessObject* m_CurrentFilter = nullptr;

  /**
   * @brief Include the declarations of the ITKImageReader helper functions that are common
   * to a few different filters across different plugins.
   */
  ITK_IMAGE_READER_HELPER_ImageDataArrayName() ITK_IMAGE_READER_HELPER_DECL()
};
