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
#include <vtkInteractorStyleImage.h>

#include <vector>

class vtkPropPicker;
class vtkCornerAnnotation;
class vtkImageViewer2;

namespace PD
{

class PDVTKInteractorStyleImage : public vtkInteractorStyleImage
{
public:
    static PDVTKInteractorStyleImage* New();
    vtkTypeMacro(PDVTKInteractorStyleImage, vtkInteractorStyleImage);      

public:

    void setImageViewer(vtkImageViewer2* imageViewer);
   
    void setPicker(vtkPropPicker* picker);

    void setAnnotation(vtkCornerAnnotation* annotation);

	void setResolution(double, double, double);

protected:
    
    virtual void OnKeyDown();

    virtual void OnMouseWheelForward();

    virtual void OnMouseWheelBackward();

    virtual void OnMouseMove();

    virtual void OnLeftButtonDown();

private:

    void getIntensity();   

    void moveSliceForward();

    void moveSliceBackward();

private:

    vtkImageViewer2* _imageViewer;

    vtkPropPicker* _picker;      // Pointer to the picker

    vtkCornerAnnotation* _annotation;  // Pointer to the annotation

    int _slice;
    int _minSlice;
    int _maxSlice;

    double _resolutions[3];
};


}

