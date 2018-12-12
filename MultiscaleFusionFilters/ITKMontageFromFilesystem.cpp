/*
 * Your License or Copyright can go here
 */

#include "ITKMontageFromFilesystem.h"

#include <fstream>

//#include "itkImageFileWriter.h" //for debugging
#include "itkStreamingImageFilter.h"
#include "itkTileMergeImageFilter.h"
#include "itkTileMontage.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Geometry/ImageGeom.h"
//#include "SIMPLib/ITK/itkBridge.h"
#include "SIMPLib/Utilities/FilePathGenerator.h"

#include "MultiscaleFusion/MultiscaleFusionConstants.h"
#include "MultiscaleFusion/MultiscaleFusionVersion.h"

#include "SIMPLib/ITK/itkGetComponentsDimensions.h"
#include "SIMPLib/ITK/itkInPlaceImageToDream3DDataFilter.h"

#define ITK_IMAGE_READER_CLASS_NAME ITKMontageFromFilesystem

#include "SIMPLib/ITK/itkImageReaderHelper.cpp"

#include <itkBMPImageIOFactory.h>
#include <itkBioRadImageIOFactory.h>
#include <itkGE4ImageIOFactory.h>
#include <itkGE5ImageIOFactory.h>
#include <itkGiplImageIOFactory.h>
#include <itkJPEGImageIOFactory.h>
#include <itkMRCImageIOFactory.h>
#include <itkMetaImageIOFactory.h>
#include <itkNiftiImageIOFactory.h>
#include <itkNrrdImageIOFactory.h>
#include <itkPNGImageIOFactory.h>
#include <itkStimulateImageIOFactory.h>
#include <itkTIFFImageIOFactory.h>
#include <itkVTKImageIOFactory.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ITKMontageFromFilesystem::ITKMontageFromFilesystem()
: AbstractFilter()
{
  m_MontageSize = {3, 3, 1};

  m_InputFileListInfo.FileExtension = QString("tif");

  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ITKMontageFromFilesystem::~ITKMontageFromFilesystem() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKMontageFromFilesystem::initialize()
{
  setErrorCondition(0);
  setWarningCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKMontageFromFilesystem::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SIMPL_NEW_INT_VEC3_FP("Montage Size", MontageSize, FilterParameter::Parameter, ITKMontageFromFilesystem));
  parameters.push_back(SIMPL_NEW_FILELISTINFO_FP("Input File List", InputFileListInfo, FilterParameter::Parameter, ITKMontageFromFilesystem));

  parameters.push_back(SIMPL_NEW_STRING_FP("Data Container", DataContainerName, FilterParameter::CreatedArray, ITKMontageFromFilesystem));
  // parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Attribute Matrix", CellAttributeMatrixName, FilterParameter::CreatedArray, ITKMontageFromFilesystem));
  parameters.push_back(SIMPL_NEW_STRING_FP("Meta Data Attribute Matrix", MetaDataAttributeMatrixName, FilterParameter::CreatedArray, ITKMontageFromFilesystem));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKMontageFromFilesystem::dataCheck() // plagiarized from DREAM3D_Plugins/ITKImageProcessing/ITKImageProcessingFilters/ImportImageMontage.cpp
{
  setErrorCondition(0);
  setWarningCondition(0);
  initialize();

  QString ss;

  if(m_InputFileListInfo.InputPath.isEmpty())
  {
    ss = QObject::tr("The input directory must be set");
    setErrorCondition(-13);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0)
  {
    return;
  }

  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  m->setGeometry(image);

  QVector<size_t> tDims(1, m_InputFileListInfo.EndIndex - m_InputFileListInfo.StartIndex + 1);
  QVector<size_t> cDims(1, 1);
  getDataContainerArray()->getDataContainer(getDataContainerName())->createNonPrereqAttributeMatrix(this, getMetaDataAttributeMatrixName(), tDims, AttributeMatrix::Type::MetaData);
  if(getErrorCondition() < 0)
  {
    return;
  }

  QFileInfo tileConfiguration(QDir(m_InputFileListInfo.InputPath), "TileConfiguration.txt");

  if(tileConfiguration.exists())
  {
    QString tileConfigPath = tileConfiguration.absoluteFilePath();
    QString ss = QObject::tr("Found %1 file. Using it and ignoring InputFileList").arg(tileConfigPath);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    PositionTableType pt;
    FilenameTableType ft;
    loadTileConfiguration(m_InputFileListInfo.InputPath.toStdString(), m_MontageSize.x, m_MontageSize.y, pt, ft);
  }
  else
  {
    bool hasMissingFiles = false;
    bool orderAscending = false;

    if(m_InputFileListInfo.Ordering == 0)
    {
      orderAscending = true;
    }
    else if(m_InputFileListInfo.Ordering == 1)
    {
      orderAscending = false;
    }

    // Now generate all the file names the user is asking for and populate the table
    QVector<QString> fileList = FilePathGenerator::GenerateFileList(m_InputFileListInfo.StartIndex, m_InputFileListInfo.EndIndex, m_InputFileListInfo.IncrementIndex, hasMissingFiles, orderAscending,
                                                                    m_InputFileListInfo.InputPath, m_InputFileListInfo.FilePrefix, m_InputFileListInfo.FileSuffix, m_InputFileListInfo.FileExtension,
                                                                    m_InputFileListInfo.PaddingDigits);

    if(fileList.empty())
    {
      QString ss = QObject::tr("No files have been selected for import. Have you set the input directory?");
      setErrorCondition(-11);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
    else
    {
      setFileName(fileList[0]);
      QFileInfo fi(fileList[0]);
      DataArrayPath dap(getDataContainerName(), getCellAttributeMatrixName(), fi.baseName());
      readImage(dap, true); // will add an attribute array if successful
	  if (getErrorCondition() == -5) // If there is an error related to ITK ImageIO factories, register them and try again
	  {
		  setErrorCondition(0); // Reset error condition
		  registerImageIOFactories();
		  readImage(dap, true);
	  }
      if(getErrorCondition() >= 0)
      {
        // Remove the attribute array that we don't need at this point
        AttributeMatrix::Pointer am = m->getAttributeMatrix(getCellAttributeMatrixName());
        am->removeAttributeArray(fi.baseName());
      }

      AttributeMatrix::Pointer mdAttrMat = getDataContainerArray()->getDataContainer(getDataContainerName())->getAttributeMatrix(getMetaDataAttributeMatrixName());
      size_t availableFileCount = 0;
      for(QVector<QString>::iterator filepath = fileList.begin(); filepath != fileList.end(); ++filepath)
      {
        QString imageFName = *filepath;
        QFileInfo fi(imageFName);
        if(fi.exists())
        {
          availableFileCount++;
        }
      }
      mdAttrMat->setTupleDimensions(QVector<size_t>(1, availableFileCount));

      QVector<size_t> cDims(1, 1);

      for(QVector<QString>::iterator filepath = fileList.begin(); filepath != fileList.end(); ++filepath)
      {
        QString imageFName = *filepath;
        QFileInfo fi(imageFName);
        if(!fi.exists())
        {
          continue;
        }
        QStringList splitFilePaths = imageFName.split('/');
        QString fileName = splitFilePaths[splitFilePaths.size() - 1];
        splitFilePaths = fileName.split('.');
        DataArrayPath path(getDataContainerName(), getCellAttributeMatrixName(), splitFilePaths[0]);
        getDataContainerArray()->createNonPrereqArrayFromPath<UInt8ArrayType, ITKMontageFromFilesystem, uint8_t>(this, path, 0, cDims);
        if(getErrorCondition() < 0)
        {
          return;
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKMontageFromFilesystem::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKMontageFromFilesystem::setCancel(bool value)
{
  itk::ProcessObject* filter = m_CurrentFilter; //make a local copy before comparison in case of data multi-threaded data race
  if(value && filter) // request cancellation of the operation
  {
    filter->SetAbortGenerateData(true);
  }
  AbstractFilter::setCancel(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKMontageFromFilesystem::execute()
{
  initialize();
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  AttributeMatrix::Pointer attrMat = m->getAttributeMatrix(getCellAttributeMatrixName());

  PositionTableType posTable;
  FilenameTableType filesTable;

  QFileInfo tileConfiguration(QDir(m_InputFileListInfo.InputPath), "TileConfiguration.txt");
  if(tileConfiguration.exists())
  {
    QString tileConfigPath = tileConfiguration.absoluteFilePath();
    QString ss = QObject::tr("Found %1 file. Using it and ignoring InputFileList").arg(tileConfigPath);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    loadTileConfiguration(m_InputFileListInfo.InputPath.toStdString(), m_MontageSize.x, m_MontageSize.y, posTable, filesTable);
  }
  else
  {
    bool hasMissingFiles = false;
    bool orderAscending = false;

    if(m_InputFileListInfo.Ordering == 0)
    {
      orderAscending = true;
    }
    else if(m_InputFileListInfo.Ordering == 1)
    {
      orderAscending = false;
    }

    // Now generate all the file names the user is asking for and populate the table
    QVector<QString> fileList = FilePathGenerator::GenerateFileList(m_InputFileListInfo.StartIndex, m_InputFileListInfo.EndIndex, m_InputFileListInfo.IncrementIndex, hasMissingFiles, orderAscending,
                                                                    m_InputFileListInfo.InputPath, m_InputFileListInfo.FilePrefix, m_InputFileListInfo.FileSuffix, m_InputFileListInfo.FileExtension,
                                                                    m_InputFileListInfo.PaddingDigits);

    if(fileList.empty())
    {
      QString ss = QObject::tr("No files have been selected for import. Have you set the input directory?");
      setErrorCondition(-11);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
    if(hasMissingFiles)
    {
      QString ss = QObject::tr("Some files from the list are missing.");
      setErrorCondition(-12);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
    if(fileList.size() != m_MontageSize.x * m_MontageSize.y)
    {
      QString ss = QObject::tr("Montage size is %1x%2=%3, but InputFileList has %4 entries.")
          .arg(m_MontageSize.x, m_MontageSize.y, m_MontageSize.x * m_MontageSize.y).arg(fileList.size());
      setErrorCondition(-19);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    QVector<QString>::iterator filepath = fileList.begin();
    filesTable.resize(m_MontageSize.y);
    posTable.resize(m_MontageSize.y);
    for(int y = 0; y < m_MontageSize.y; y++)
    {
      filesTable[y].resize(m_MontageSize.x);
      posTable[y].resize(m_MontageSize.x);
      for(int x = 0; x < m_MontageSize.x; x++)
      {
        assert(filepath != fileList.end());
        QString imageFName = *filepath;
        QFileInfo fi(imageFName);
        if(!fi.exists())
        {
          QString ss = QObject::tr("%1 does not exist").arg(imageFName);
          setErrorCondition(-11);
          notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
          return;
        }

        filesTable[y][x] = imageFName.toStdString();

        PointType p;
        p.Fill(0.0);
        posTable[y][x] = p;

        ++filepath; //get next filename from the list
      }
    }
  }
  //either way, posTable and filesTable are now filled

  if(getCancel())
  {
    return;
  }

  // TODO: read image information for filesTable[0][0]
  // update the expected overlap if needed
  // and instantiate doMontage with appropriate type

  doMontage<unsigned short>(posTable, filesTable);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ITKMontageFromFilesystem::loadTileConfiguration(std::string dirPath, unsigned xSize, unsigned ySize, PositionTableType& pos, FilenameTableType& files)
{
  std::string fileName = dirPath + "/TileConfiguration.txt";
  std::ifstream fStage(fileName);
  if(!fStage)
  {
    QString ss = QObject::tr("%1 exists but could not be opened for reading").arg(QString::fromStdString(fileName));
    setErrorCondition(-17);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  std::string temp;
  std::getline(fStage, temp); // throw away header
  std::getline(fStage, temp); // throw away header
  std::getline(fStage, temp); // throw away header
  std::getline(fStage, temp); // throw away header

  // read coordinates from files
  files.resize(ySize);
  pos.resize(ySize);
  for(unsigned y = 0; y < ySize; y++)
  {
    files[y].resize(xSize);
    pos[y].resize(xSize);
    for(unsigned x = 0; x < xSize; x++)
    {
      std::getline(fStage, temp, ';');
      if(!fStage)
      {
        QString ss = QObject::tr("Could not read information for tile %1 (%2,%3)").arg(x + y * xSize, x, y);
        setErrorCondition(-18);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }
      files[y][x] = std::string(dirPath + std::string("/") + temp);
      std::getline(fStage, temp, '(');

      PointType p;
      fStage >> p[0];
      fStage.ignore();
      fStage >> p[1];
      pos[y][x] = p;
      std::getline(fStage, temp); // throw away rest of line
    }
  }
}

// streamSubdivisions of 1 disables streaming (higher memory useage, less cluttered debug output)
template <typename PixelType, typename AccumulatePixelType>
void ITKMontageFromFilesystem::doMontage(const PositionTableType& tilePositions, const FilenameTableType& filenames, int peakMethodToUse, unsigned streamSubdivisions)
{
  using ScalarPixelType = typename itk::NumericTraits<PixelType>::ValueType;
  constexpr unsigned Dimension = 2;
  using PointType = itk::Point<double, Dimension>;
  using VectorType = itk::Vector<double, Dimension>;
  using TransformType = itk::TranslationTransform<double, Dimension>;
  using ScalarImageType = itk::Image<ScalarPixelType, Dimension>;
  using OriginalImageType = itk::Image<PixelType, Dimension>; // possibly RGB instead of scalar
  using ImageTypePointer = typename ScalarImageType::Pointer;
  using PCMType = itk::PhaseCorrelationImageRegistrationMethod<ScalarImageType, ScalarImageType>;
  typename ScalarImageType::SpacingType sp;
  sp.Fill(1.0); // assume unit spacing
  //itk::ObjectFactoryBase::RegisterFactory(itk::TxtTransformIOFactory::New());

  using PeakInterpolationType = typename itk::MaxPhaseCorrelationOptimizer<PCMType>::PeakInterpolationMethod;
  using PeakFinderUnderlying = typename std::underlying_type<PeakInterpolationType>::type;
  auto peakMethod = static_cast<PeakFinderUnderlying>(peakMethodToUse);

  unsigned xSize = filenames[0].size();
  unsigned ySize = filenames.size();
  unsigned x1 = 1;
  unsigned y1 = 1;
  if (xSize < 2)
  {
    x1 = 0;
  }
  if(ySize < 2)
  {
    y1 = 0;
  }
  PointType originAdjustment = tilePositions[x1][y1] - tilePositions[0][0];

  using MontageType = itk::TileMontage<ScalarImageType>;
  typename MontageType::Pointer montage = MontageType::New();
  montage->SetMontageSize({xSize, ySize});
  montage->GetModifiablePCM()->SetPaddingMethod(PCMType::PaddingMethod::MirrorWithExponentialDecay);
  montage->GetModifiablePCMOptimizer()->SetPeakInterpolationMethod(static_cast<PeakInterpolationType>(peakMethod));
  montage->SetOriginAdjustment(originAdjustment);
  montage->SetForcedSpacing(sp);

  typename MontageType::TileIndexType ind;
  for(unsigned y = 0; y < ySize; y++)
  {
    ind[1] = y;
    for(unsigned x = 0; x < xSize; x++)
    {
      ind[0] = x;
      montage->SetInputTile(ind, filenames[y][x]);
    }
  }

  notifyStatusMessage(getHumanLabel(), "Doing the tile registrations");
  //itk::SimpleFilterWatcher fw(montage, "montage");
  montage->Update();
  notifyStatusMessage(getHumanLabel(), "Finished the tile registrations");

  for(unsigned y = 0; y < ySize; y++)
  {
    ind[1] = y;
    for(unsigned x = 0; x < xSize; x++)
    {
      ind[0] = x;
      const TransformType* regTr = montage->GetOutputTransform(ind);
      VectorType tr = regTr->GetOffset(); // ignore tile's translation for now
    }
  }

  // write generated mosaic
  using Resampler = itk::TileMergeImageFilter<OriginalImageType, AccumulatePixelType>;
  typename Resampler::Pointer resampleF = Resampler::New();
  //itk::SimpleFilterWatcher fw2(resampleF, "resampler");
  if(true)
  {
    resampleF->SetMontage(montage);
  }
  else
  {
    resampleF->SetMontageSize({xSize, ySize});
    resampleF->SetOriginAdjustment(originAdjustment);
    resampleF->SetForcedSpacing(sp);
    for(unsigned y = 0; y < ySize; y++)
    {
      ind[1] = y;
      for(unsigned x = 0; x < xSize; x++)
      {
        ind[0] = x;
        resampleF->SetInputTile(ind, filenames[y][x]);
        resampleF->SetTileTransform(ind, montage->GetOutputTransform(ind));
      }
    }
  }

  using Dream3DImageType = itk::Dream3DImage<PixelType, Dimension>;
  using StreamingFilterType = itk::StreamingImageFilter<OriginalImageType, Dream3DImageType>;
  typename StreamingFilterType::Pointer streamingFilter = StreamingFilterType::New();
  streamingFilter->SetInput(resampleF->GetOutput());
  streamingFilter->SetNumberOfStreamDivisions(streamSubdivisions);

  notifyStatusMessage(getHumanLabel(), "Resampling tiles into the stitched image");
  //// resampleF->Update();
  //using WriterType = itk::ImageFileWriter<OriginalImageType>;
  //typename WriterType::Pointer w = WriterType::New();
  //w->SetInput(resampleF->GetOutput());
  //// resampleF->DebugOn(); //generate an image of contributing regions
  //// MetaImage format supports streaming
  //w->SetFileName("C:/a/Dream3D.mha");
  //w->UseCompressionOn();
  //w->SetNumberOfStreamDivisions(streamSubdivisions);
  //w->Update();

  streamingFilter->Update();
  notifyStatusMessage(getHumanLabel(), "Finished resampling tiles");
  notifyStatusMessage(getHumanLabel(), "Converting into DREAM3D data structure");

  QString imageFName = QString::fromStdString(filenames[0][0]);
  QRegExp backslashOrSlash("(\\\\|\\/)"); // backslash or slash
  QStringList splitFilePaths = imageFName.split(backslashOrSlash, QString::SkipEmptyParts);
  unsigned index = splitFilePaths.size() >= 2 ? splitFilePaths.size() - 2 : 0;
  QString fileName = splitFilePaths[index];
  splitFilePaths = fileName.split('.');
  DataArrayPath dataArrayPath(getDataContainerName(), getCellAttributeMatrixName(), splitFilePaths[0]);
  DataContainer::Pointer container = getDataContainerArray()->getDataContainer(dataArrayPath.getDataContainerName());
  if(container.get() == nullptr)
  {
    setErrorCondition(-4);
    notifyErrorMessage(getHumanLabel(), "Container not found.", getErrorCondition());
    return;
  }

  using ToDream3DType = itk::InPlaceImageToDream3DDataFilter<PixelType, Dimension>;
  typename ToDream3DType::Pointer toDream3DFilter = ToDream3DType::New();
  toDream3DFilter->SetInput(streamingFilter->GetOutput());
  toDream3DFilter->SetInPlace(true);
  toDream3DFilter->SetAttributeMatrixArrayName(dataArrayPath.getAttributeMatrixName().toStdString());
  toDream3DFilter->SetDataArrayName(dataArrayPath.getDataArrayName().toStdString());
  toDream3DFilter->SetDataContainer(container);
  toDream3DFilter->Update();

}

void ITKMontageFromFilesystem::registerImageIOFactories()
{
	itk::JPEGImageIOFactory::RegisterOneFactory();
	itk::NrrdImageIOFactory::RegisterOneFactory();
	itk::PNGImageIOFactory::RegisterOneFactory();
	itk::TIFFImageIOFactory::RegisterOneFactory();
	itk::JPEGImageIOFactory::RegisterOneFactory();
	itk::BMPImageIOFactory::RegisterOneFactory();
	itk::MetaImageIOFactory::RegisterOneFactory();
	itk::NiftiImageIOFactory::RegisterOneFactory();
	itk::GiplImageIOFactory::RegisterOneFactory();
	itk::VTKImageIOFactory::RegisterOneFactory();
	itk::StimulateImageIOFactory::RegisterOneFactory();
	itk::BioRadImageIOFactory::RegisterOneFactory();
	itk::GE4ImageIOFactory::RegisterOneFactory();
	itk::GE5ImageIOFactory::RegisterOneFactory();
	itk::MRCImageIOFactory::RegisterOneFactory();
#ifdef ITK_IMAGE_PROCESSING_HAVE_SCIFIO
	itk::SCIFIOImageIOFactory::RegisterOneFactory();
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ITKMontageFromFilesystem::newFilterInstance(bool copyFilterParameters) const
{
  ITKMontageFromFilesystem::Pointer filter = ITKMontageFromFilesystem::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKMontageFromFilesystem::getCompiledLibraryName() const
{
  return MultiscaleFusionConstants::MultiscaleFusionBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKMontageFromFilesystem::getBrandingString() const
{
  return "MultiscaleFusion";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKMontageFromFilesystem::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << MultiscaleFusion::Version::Major() << "." << MultiscaleFusion::Version::Minor() << "." << MultiscaleFusion::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKMontageFromFilesystem::getGroupName() const
{
  return SIMPL::FilterGroups::ReconstructionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKMontageFromFilesystem::getSubGroupName() const
{
  return "MultiscaleFusion";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ITKMontageFromFilesystem::getHumanLabel() const
{
  return "ITK Montage From Filesystem";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid ITKMontageFromFilesystem::getUuid()
{
  return QUuid("{848d5eb2-ec42-11e8-8eb2-f2801f1b9fd1}");
}
