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

#include "ui_QtVTKRenderWindows.h"
#include "PDVTKInteractorStyleImage.h"

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkResliceImageViewer.h>
#include <vtkDICOMImageReader.h>
#include <vtkProperty.h>
#include <vtkImageData.h>
#include <vtkCommand.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkInteractorStyleImage.h>
#include <vtkAssemblyPath.h>
#include <vtkObjectFactory.h>
#include <vtkImageActor.h>
#include <vtkCornerAnnotation.h>
#include <vtkImageCast.h>
#include <vtkMath.h>
#include <vtkTextProperty.h>
#include <vtkVolume.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkImageShiftScale.h>
#include <vtkPropPicker.h>
#include "QtVTKRenderWindows.h"

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL);
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL);

using namespace PD;

QtVTKRenderWindows::QtVTKRenderWindows(int argc, char *argv[])
{
	std::string dicomFolder = argv[1];
  
    this->ui = new Ui_QtVTKRenderWindows;
	this->ui->setupUi(this);

	_reader = vtkSmartPointer< vtkDICOMImageReader >::New();
	
	_reader->SetDirectoryName(dicomFolder.c_str());
	_reader->Update();
	
	setupPlaneViews();
	
	setupVolumeView();

	createScalingFilter();	
	
	connect(this->ui->toggleIntensityScale, SIGNAL(stateChanged(int)), this, SLOT(toggleIntensityScaling(int)));
	
	_isScaling = this->ui->toggleIntensityScale->isChecked();

	renderAll();
};



void QtVTKRenderWindows::toggleIntensityScaling(int state)
{
	_isScaling = (state == 2);

	for (int i = 0; i < 3; i++)
	{
		_resliceImageViewer[i]->SetInputData( _isScaling ? _shiftScaleFilter->GetOutput() : _reader->GetOutput());
		
		_resliceImageViewer[i]->Modified();	
	}

	renderAll();
}



void QtVTKRenderWindows::renderAll()
{
	for (int i = 0; i < 3; i++)
	{
		_resliceImageViewer[i]->Render();
	}
	this->ui->view3->GetRenderWindow()->Render();
}



vtkSmartPointer<vtkVolume> QtVTKRenderWindows::createVolume()
{
	vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
	vtkSmartPointer<vtkFixedPointVolumeRayCastMapper> volumeMapper = vtkSmartPointer<vtkFixedPointVolumeRayCastMapper>::New();

	volumeMapper->SetInputConnection(_reader->GetOutputPort());

	vtkSmartPointer<vtkColorTransferFunction>volumeColor = vtkSmartPointer<vtkColorTransferFunction>::New();
	volumeColor->AddRGBPoint(0, 0.0, 0.0, 0.0);
	volumeColor->AddRGBPoint(500, 1.0, 0.5, 0.3);
	volumeColor->AddRGBPoint(1000, 1.0, 0.5, 0.3);
	volumeColor->AddRGBPoint(1150, 1.0, 1.0, 0.9);

	vtkSmartPointer<vtkPiecewiseFunction> volumeScalarOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
	volumeScalarOpacity->AddPoint(0, 0.00);
	volumeScalarOpacity->AddPoint(500, 0.15);
	volumeScalarOpacity->AddPoint(1000, 0.15);
	volumeScalarOpacity->AddPoint(1150, 0.85);

	vtkSmartPointer<vtkPiecewiseFunction> volumeGradientOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
	volumeGradientOpacity->AddPoint(0, 0.0);
	volumeGradientOpacity->AddPoint(90, 0.5);
	volumeGradientOpacity->AddPoint(100, 1.0);

	vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
	volumeProperty->SetColor(volumeColor);
	volumeProperty->SetScalarOpacity(volumeScalarOpacity);
	volumeProperty->SetGradientOpacity(volumeGradientOpacity);
	volumeProperty->SetInterpolationTypeToLinear();
	volumeProperty->ShadeOn();
	volumeProperty->SetAmbient(0.4);
	volumeProperty->SetDiffuse(0.6);
	volumeProperty->SetSpecular(0.2);

	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);

	return volume;
}



void QtVTKRenderWindows::setupVolumeView()
{
	vtkSmartPointer< vtkRenderer > ren = vtkSmartPointer< vtkRenderer >::New();

	this->ui->view4->GetRenderWindow()->AddRenderer(ren);

	vtkRenderWindowInteractor *iren = this->ui->view4->GetInteractor();

	vtkSmartPointer<vtkVolume> volume = createVolume();

	ren->AddViewProp(volume);
}



void QtVTKRenderWindows::createScalingFilter()
{
	double* range = _reader->GetOutput()->GetScalarRange();

	_shiftScaleFilter = vtkSmartPointer<vtkImageShiftScale>::New();

	_shiftScaleFilter->SetInputData(_reader->GetOutput());
	_shiftScaleFilter->SetShift(-range[0]);

	double scale = VTK_UNSIGNED_CHAR_MAX / (range[1] - range[0]);

	_shiftScaleFilter->SetScale(scale);
	_shiftScaleFilter->SetOutputScalarTypeToUnsignedChar();

	_shiftScaleFilter->Update();
}



void QtVTKRenderWindows::setupPlaneViews()
{
	for (int i = 0; i < 3; i++)
	{
		QVTKWidget* qtvtkWidget = nullptr;
		_resliceImageViewer[i] = vtkSmartPointer< vtkResliceImageViewer >::New();				

		switch (i)
		{
		case 0:
			qtvtkWidget = this->ui->view1;
			break;
		case 1:
			qtvtkWidget = this->ui->view2;
			break;
		case 2:
			qtvtkWidget = this->ui->view3;
			break;
		default:
			break;
		}

		qtvtkWidget->SetRenderWindow(_resliceImageViewer[i]->GetRenderWindow());
		_resliceImageViewer[i]->SetupInteractor(qtvtkWidget->GetRenderWindow()->GetInteractor());

		vtkSmartPointer<PDVTKInteractorStyleImage> interactorStyle = vtkSmartPointer<PDVTKInteractorStyleImage>::New();

		interactorStyle->setImageViewer(_resliceImageViewer[i]);
		qtvtkWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle(interactorStyle);

		_resliceImageViewer[i]->SetInputData(_reader->GetOutput());

		switch (i)
		{
		case 0:
			_resliceImageViewer[i]->SetSliceOrientationToYZ();
			break;
		case 1:
			_resliceImageViewer[i]->SetSliceOrientationToXZ();
			break;
		case 2:
			_resliceImageViewer[i]->SetSliceOrientationToXY();
			break;
		default:
			break;
		}

		double color[3] = { 0, 0, 0 };

		color[i] = 1;
		color[0] /= 4.0;
		color[1] /= 4.0;
		color[2] /= 4.0;

		_resliceImageViewer[i]->GetRenderer()->SetBackground(color);

		qtvtkWidget->show();
	}

	setupAnnotation();
}



void QtVTKRenderWindows::setupAnnotation()
{
	double* resolution = _reader->GetPixelSpacing();

	for (int i = 0; i < 3; i++)
	{
		// Picker to pick pixels

		_propPickers[i] = vtkSmartPointer<vtkPropPicker>::New();
		_propPickers[i]->PickFromListOn();

		// Give the picker a prop to pick

		vtkImageActor* imageActor = _resliceImageViewer[i]->GetImageActor();

		_propPickers[i]->AddPickList(imageActor);

		// disable interpolation, so we can see each pixel

		imageActor->InterpolateOff();

		// Annotate the image with window/level and mouse over pixel information

		vtkSmartPointer<vtkCornerAnnotation> cornerAnnotation = vtkSmartPointer<vtkCornerAnnotation>::New();

		cornerAnnotation->SetLinearFontScaleFactor(2);
		cornerAnnotation->SetNonlinearFontScaleFactor(1);
		cornerAnnotation->SetMaximumFontSize(20);
		cornerAnnotation->SetText(0, "Off Image");
		cornerAnnotation->SetText(3, "<window>\n<level>");

		cornerAnnotation->GetTextProperty()->SetColor(1, 0, 0);

		_resliceImageViewer[i]->GetRenderer()->AddViewProp(cornerAnnotation);

		PDVTKInteractorStyleImage* pdInteractorStyleImage = static_cast<PDVTKInteractorStyleImage*>(_resliceImageViewer[i]->GetRenderWindow()->GetInteractor()->GetInteractorStyle());		

		pdInteractorStyleImage->setAnnotation(cornerAnnotation);
		pdInteractorStyleImage->setPicker(_propPickers[i]);
		pdInteractorStyleImage->setResolution(resolution[0], resolution[1], resolution[2]);
	}
}