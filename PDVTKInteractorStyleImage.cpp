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

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkCellPicker.h>
#include <vtkProperty.h>
#include <vtkImageData.h>
#include <vtkPropPicker.h>
#include <vtkPointHandleRepresentation3D.h>
#include <vtkPointHandleRepresentation2D.h>
#include <vtkAssemblyPath.h>
#include <vtkMath.h>
#include <vtkObjectFactory.h>
#include <vtkImageActor.h>
#include <vtkCornerAnnotation.h>
#include <vtkImageCast.h>
#include <vtkInteractorStyle.h>
#include <vtkImageViewer2.h>
#include <vtkRenderWindowInteractor.h>

#include "PDVTKInteractorStyleImage.h"

using namespace PD;

vtkStandardNewMacro(PDVTKInteractorStyleImage);

// Template for image value reading
template<typename T>
void vtkValueMessageTemplate(vtkImageData* image, int* position, std::string& message)
{
    T* tuple = ((T*)image->GetScalarPointer(position));
    if (tuple == NULL)
        return;

    int components = image->GetNumberOfScalarComponents();

    for (int c = 0; c < components; ++c)
    {
        message += vtkVariant(tuple[c]).ToString();
        if (c != (components - 1))
        {
            message += ", ";
        }
    }
    message += " )";
}



void PDVTKInteractorStyleImage::setImageViewer(vtkImageViewer2* imageViewer) 
{
    _imageViewer = imageViewer;

    _minSlice = imageViewer->GetSliceMin();
    _maxSlice = imageViewer->GetSliceMax();

    _slice = _minSlice;

    _picker = NULL;
    _annotation = NULL;
}



void PDVTKInteractorStyleImage::setPicker(vtkPropPicker *picker)
{
    this->_picker = picker;
}



void PDVTKInteractorStyleImage::setAnnotation(vtkCornerAnnotation *annotation)
{
    this->_annotation = annotation;
}



void PDVTKInteractorStyleImage::moveSliceForward() 
{
    if(_slice < _maxSlice) 
    {
        ++_slice;

        _imageViewer->SetSlice(_slice);
        _imageViewer->Render();
    }
}



void PDVTKInteractorStyleImage::moveSliceBackward() 
{
    if(_slice > _minSlice) 
    {
        --_slice;

        _imageViewer->SetSlice(_slice);  
        _imageViewer->Render();
    }
}



void PDVTKInteractorStyleImage::OnKeyDown() 
{
    std::string key = this->GetInteractor()->GetKeySym();

    if(key.compare("Up") == 0) 
    {      
        moveSliceForward();
    }
    else if(key.compare("Down") == 0) 
    {       
        moveSliceBackward();
    }

    // forward event
    vtkInteractorStyleImage::OnKeyDown();
}



void PDVTKInteractorStyleImage::OnMouseWheelForward() 
{   
    moveSliceForward();      
}



void PDVTKInteractorStyleImage::OnMouseWheelBackward() 
{  
    moveSliceBackward();      
}



void PDVTKInteractorStyleImage::OnMouseMove()
{
    getIntensity();
}



void PDVTKInteractorStyleImage::OnLeftButtonDown()
{
    std::cout << "Override pressed left mouse button." << std::endl;
}   



void PDVTKInteractorStyleImage::getIntensity()
{
    vtkRenderWindowInteractor *interactor = this->_imageViewer->GetRenderWindow()->GetInteractor();
    vtkRenderer* renderer = this->_imageViewer->GetRenderer();
    vtkImageActor* actor = this->_imageViewer->GetImageActor();
    vtkImageData* image = this->_imageViewer->GetInput();
    vtkInteractorStyle *style = vtkInteractorStyle::SafeDownCast(interactor->GetInteractorStyle());

#if VTK_MAJOR_VERSION <= 5
    image->Update();
#endif

    int* pPos = interactor->GetEventPosition();

    if (pPos == NULL)    
        return;    

    // Pick at the mouse location provided by the interactor
    
	this->_picker->Pick(pPos[0], pPos[1], 0.0, renderer);

    // There could be other props assigned to this picker, so make sure we picked the image actor

    vtkAssemblyPath* path = this->_picker->GetPath();
    bool validPick = false;

    if (path)
    {
        vtkCollectionSimpleIterator sit;
        path->InitTraversal(sit);
        vtkAssemblyNode *node;
        for (int i = 0; i < path->GetNumberOfItems() && !validPick; ++i)
        {
            node = path->GetNextNode(sit);
            if (actor == vtkImageActor::SafeDownCast(node->GetViewProp()))
            {
                validPick = true;
            }
        }
    }

    if (!validPick)
    {
        this->_annotation->SetText(0, "Off Image");
        interactor->Render();
        return;
    }

    // Get the world coordinates of the pick
    double pos[3];
    this->_picker->GetPickPosition(pos);

    int image_coordinate[3];

    image_coordinate[0] = vtkMath::Round(pos[0]) / _resolutions[0];
    image_coordinate[1] = vtkMath::Round(pos[1]) / _resolutions[1];
    image_coordinate[2] = vtkMath::Round(pos[2]) / _resolutions[2];

    std::string message = "Location: ( ";
    message += vtkVariant(image_coordinate[0]).ToString();
    message += ", ";
    message += vtkVariant(image_coordinate[1]).ToString();
    message += ", ";
    message += vtkVariant(image_coordinate[2]).ToString();
    message += " )\nValue: ( ";

    switch (image->GetScalarType())
    {
        vtkTemplateMacro((vtkValueMessageTemplate<VTK_TT>(image, image_coordinate, message)));

    default:
        return;
    }

    this->_annotation->SetText(0, message.c_str());

    interactor->Render();	
}



void PDVTKInteractorStyleImage::setResolution(double x, double y, double z)
{
	_resolutions[0] = x;
	_resolutions[1] = y;
	_resolutions[2] = z;
}