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

#include <QApplication>
#include "QtVTKRenderWindows.h"

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		cout << "Usage: " << argv[0] << " DATADIR/headsq/quarter" << endl;

		return EXIT_FAILURE;
	}

	// QT Stuff
	QApplication app(argc, argv);

	PD::QtVTKRenderWindows qtVTKRenderWindows(argc, argv);
	qtVTKRenderWindows.show();

	return app.exec();
}
