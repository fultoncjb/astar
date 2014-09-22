 #include "menuwindow.h"
#include <iostream>
#include <sys/time.h>

MenuWindow::MenuWindow()
: m_ButtonBox(Gtk::ORIENTATION_VERTICAL),
  m_Button_File("Choose File")
{
	set_title("Astar");

	m_ButtonBox.pack_start(m_Button_File);
	m_Button_File.signal_clicked().connect(sigc::mem_fun(*this,
	      &MenuWindow::on_button_file_clicked) );

	add(m_ButtonBox);
	m_ButtonBox.show();
	m_Button_File.show();
	//show_all_children();
}

MenuWindow::~MenuWindow()
{
}

MapDrawArea::~MapDrawArea()
{
}

MapDrawArea::MapDrawArea()
{
}

MenuWindow::MapWindow::MapWindow()
: m_draw()
{
  
}

MenuWindow::MapWindow::MapWindow(const MenuWindow &e)
: m_draw()
{
	// Set title, maximize window
	set_title(e.inputFilename);
	maximize();

	// Start timer
	timeval t1,t2;
	gettimeofday(&t1,NULL);

	// Solve the path
	bool result = m_draw.guiMapData.getPath( e.inputFilename.c_str(), e.outputFilename.c_str() );

	// Stop timer
	gettimeofday(&t2,NULL);

	// Print solving time only when the map was solved
	std::stringstream messageString;
	if(result)
		messageString << "Map successfully solved in: " << (double)t2.tv_sec-(double)t1.tv_sec+((double)t2.tv_usec)/1e6-((double)t1.tv_usec/1e6) << " seconds";
	// Map not solvable
	else
	{
		messageString << "There is no possible path for the robot to reach the given goal coordinate from the given start coordinate. Please check your map file.";
	}

	// Popup message dialog
	Gtk::MessageDialog dlg( messageString.str() );
	dlg.run();

	// Drawing area for the map
	add(m_draw);

	// Show
	show_all_children();
}



MenuWindow::MapWindow::~MapWindow()
{
}

// Handler for popup dialog
void MenuWindow::on_button_file_clicked()
{
	Gtk::FileChooserDialog dialog("Please choose a file",
	  Gtk::FILE_CHOOSER_ACTION_OPEN);
	dialog.set_transient_for(*this);

	//Add response buttons the the dialog:
	dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	dialog.add_button("_Open", Gtk::RESPONSE_OK);

	//Add filters, so that only certain file types can be selected:

	Glib::RefPtr<Gtk::FileFilter> filter_text = Gtk::FileFilter::create();
	filter_text->set_name("Text/Data files");
	filter_text->add_mime_type("text/plain");
	dialog.add_filter(filter_text);

	Glib::RefPtr<Gtk::FileFilter> filter_any = Gtk::FileFilter::create();
	filter_any->set_name("Any files");
	filter_any->add_pattern("*");
	dialog.add_filter(filter_any);

	//Show the dialog and wait for a user response:
	int result = dialog.run();
	//dialog.hide();

	//Handle the response:
	switch(result)
	{
		case(Gtk::RESPONSE_OK):
		{

			//Notice that this is a std::string, not a Glib::ustring.
			// Need to pass this to the astar algorithm
			inputFilename = dialog.get_filename();
			outputFilename = inputFilename + ".out";

			// Create child window and plot obstacles
			MapWindow *popupWin = new MapWindow(*this);
			popupWin->show();

			break;
		}
		/*case(Gtk::RESPONSE_CANCEL):
		{
			std::cout << "Cancel clicked." << std::endl;
			break;
		}*/
		default:
		{
			std::cout << "Unexpected button clicked." << std::endl;
			break;
		}
	}
}

// Draw the obstacles, start point, end point, final path
bool MapDrawArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	// This is where we draw on the window
	Gtk::Allocation allocation = get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();
	const int lesser = MIN(width, height);

	// We should be able to just store the obstacles and path once
	// Do need to update based on the window size
	const std::vector<m_map::coord> vObstacles = guiMapData.copyObstacles();
	const std::vector<m_map::coord> optimalPath = guiMapData.copyOptPath();
	const m_map::coord maxXY = guiMapData.copyMaxCoord();
	m_map::coord originCoord = guiMapData.copyStartCoord();
	m_map::coord goalCoord = guiMapData.copyEndCoord();

	// These have to be updated each iteration
	originCoord.x = int( float(width)*float(originCoord.x)/float(maxXY.x) );
	originCoord.y = int( float(height)*float(originCoord.y)/float(maxXY.y) );
	goalCoord.x = int( float(width)*float(goalCoord.x)/float(maxXY.x) );
	goalCoord.y = int( float(height)*float(goalCoord.y)/float(maxXY.y) );


	// Draw obstacles
	std::vector<m_map::coord> scaledObsCoord;
	m_map::coord stdCoord;

	// Adjust obstacle values based on window size
	for(std::vector<m_map::coord>::const_iterator itr=vObstacles.begin();itr!=vObstacles.end();++itr)
	{
		stdCoord.x = int( float(width)*float((*itr).x)/float(maxXY.x) );
		stdCoord.y = int( height*float((*itr).y)/float(maxXY.y) );
		scaledObsCoord.push_back(stdCoord);
	}

	cr->save();
	cr->set_source_rgb(0.0, 0.0, 0.0);	// black for obstacles
	cr->set_line_width(lesser * 0.005);
	cr->set_line_cap(Cairo::LINE_CAP_ROUND);

	// Plot obstacles
	for(std::vector<m_map::coord>::iterator itr=scaledObsCoord.begin();itr != scaledObsCoord.end();++itr)
	{
		cr->move_to( (*itr).x,(*itr).y );
		cr->line_to( (*itr).x,(*itr).y );
		cr->stroke();
	}

	// Plot start/end coord
	cr->save();
	cr->set_line_width(lesser * 0.015);
	cr->set_source_rgb(1.0, 0.0, 0.0);	// red for start point
	cr->move_to( originCoord.x,originCoord.y );
	cr->line_to( originCoord.x,originCoord.y );
	cr->stroke();
	cr->save();
	cr->set_source_rgb(0.0, 1.0, 0.0);	// green for end point
	cr->move_to( goalCoord.x,goalCoord.y );
	cr->line_to( goalCoord.x,goalCoord.y );
	cr->stroke();

	// Plot the path
	cr->save();
	cr->set_source_rgb(1.0, 0.08, 0.58);	// pink for path
	cr->set_line_width(lesser * 0.005);

	for(std::vector<m_map::coord>::const_iterator itr=optimalPath.begin();itr != optimalPath.end();++itr)
	{
		cr->move_to( int( float(width)*float((*itr).x)/float(maxXY.x) ),int( height*float((*itr).y)/float(maxXY.y) ));
		cr->line_to( int( float(width)*float((*itr).x)/float(maxXY.x) ),int( height*float((*itr).y)/float(maxXY.y) ) );
		cr->stroke();
	}

	return true;
}
