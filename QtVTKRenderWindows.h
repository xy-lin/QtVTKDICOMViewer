/*
///////////////////////////////////////////////////////////////////////////////////////

DEMO APPLICATION : Demo for viewing medical image in DICOM format

10/June/2016
Xinyu Lin

Command line Usage :
PD_demo.exe dicom_folder/

The implementation is based on the "FourPaneViewer" code example from www.vtk.org
It depends on Qt5.6 and VTK6.3

/////////////////////////////////////////////////////////////////////////////
*/

#pragma once

#include <vtkSmartPointer.h>

#include <QMainWindow>

// Forward Qt class declarations
class Ui_QtVTKRenderWindows;

class vtkImageShiftScale;
class vtkDICOMImageReader;
class vtkPropPicker;
class vtkResliceImageViewer;
class vtkVolume;

namespace PD
{

class QtVTKRenderWindows : public QMainWindow
{
  Q_OBJECT
public:

  // Constructor
  QtVTKRenderWindows(int argc, char *argv[]);

  virtual ~QtVTKRenderWindows() {}
  
protected:
  vtkSmartPointer<vtkResliceImageViewer> _resliceImageViewer[3];
  
  // Picker to pick pixels
  vtkSmartPointer<vtkPropPicker> _propPickers[3];

public slots:
  void toggleIntensityScaling(int state);

private:

  // Designer form
  Ui_QtVTKRenderWindows *ui;

  bool _isScaling;

  vtkSmartPointer<vtkDICOMImageReader> _reader;

  vtkSmartPointer<vtkImageShiftScale> _shiftScaleFilter;

private:

  void renderAll();

  vtkSmartPointer<vtkVolume> createVolume();

  void createScalingFilter();

  void setupVolumeView();  

  void setupPlaneViews();

  void setupAnnotation();
};

}

