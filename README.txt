
DEMO APPLICATION : Demo for viewing medical image in DICOM format

10/June/2016
Xinyu Lin

Command line Usage :
PD_demo.exe dicom_folder/

The implementation is based on the "FourPaneViewer" code example from www.vtk.org
It depends on Qt5.6 and VTK6.3. 
The bin folder contains pre-build executable and dependent DLLs. It was build using visual studio 2015 community in Windows 10.

A sample data from www.vtk.org is also provided.

For those machines without development environment, visual C++ 2015 redistributable needs to be installed first in order to run the binary. Building the binary from scratch is always recommended.

