#include "menuwindow.h"
#include <iostream>
#include <sys/time.h>
#include <gtkmm/box.h>

MapDrawArea::~MapDrawArea()
{
}

MapDrawArea::MapDrawArea()
{
	guiMapData.MapState = UNINITIALIZED;
}

SpinnerWindow::SpinnerWindow()
: SpinnerBox(Gtk::ORIENTATION_VERTICAL),
  spinner(),
  WaitLabel("Please wait...")
{
	spinner.start();
	SpinnerBox.pack_start(spinner,Gtk::PACK_EXPAND_PADDING,0);
	SpinnerBox.pack_end(WaitLabel,Gtk::PACK_EXPAND_PADDING,0);
	add(SpinnerBox);
	show_all_children();
	set_size_request(150,25);
	set_title("Solving");
	hide();
}

MapWindow::MapWindow()
: m_draw(),
  m_VertParentBox(Gtk::ORIENTATION_VERTICAL),
  m_HorzChildBox(Gtk::ORIENTATION_HORIZONTAL),
  m_Button_File(),
  m_Button_Solve(),
  m_WorkerThread(NULL),
  DialogWindow()
{
	// Add the text for the file button and picture
	m_Button_File.add_pixlabel("open.gif","Choose file");
	m_Button_Solve.add_pixlabel("worker.gif","Solve path");

	// Add a buffer border to the window
	set_border_width(10);

	// Set title, maximize window
	set_title("Astar");
	maximize();

	// Set the minimum window size
	set_size_request(400,300);

	// Connect button handlers
	m_Button_File.signal_clicked().connect(sigc::mem_fun(*this,
		  &MapWindow::on_button_file_clicked) );
	m_Button_Solve.signal_clicked().connect(sigc::mem_fun(*this,
			  &MapWindow::on_button_solve_clicked) );

	// Connect dispatcher for worker thread
	m_Dispatcher.connect(sigc::mem_fun(*this,
			  &MapWindow::on_notification_from_worker_thread));

	// Disable the Solve button
	m_Button_Solve.set_sensitive(false);

	// Add a vertical box
	add(m_VertParentBox);
	m_VertParentBox.show();

	// Horizontal box contains the buttons
	m_HorzChildBox.pack_end(m_Button_File,Gtk::PACK_EXPAND_PADDING,0);
	m_HorzChildBox.pack_end(m_Button_Solve,Gtk::PACK_EXPAND_PADDING,0);

	// Vertical box contains the horizontal box and the drawing area
	m_VertParentBox.pack_start(m_HorzChildBox,false,false,10);
	m_VertParentBox.pack_start(m_draw);

	// Show
	show_all_children();
}

bool MapWindow::SolveOptimalPath()
{
	timeval t1,t2;
	gettimeofday(&t1,NULL);
	bool result = m_draw.guiMapData.SolveOptimalPath( inputFilename.c_str(), outputFilename.c_str() );
	gettimeofday(&t2,NULL);
	if(result)
	{
		std::cout << "Map successfully solved in: " << (double)t2.tv_sec-(double)t1.tv_sec+((double)t2.tv_usec)/1e6-((double)t1.tv_usec/1e6) << " seconds";
		std::cout << std::endl;
	}
	return result;
}

void MapDrawArea::DrawObstacles(const Cairo::RefPtr<Cairo::Context>& cr)
{
	// This is where we draw on the window
	Gtk::Allocation allocation = get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();
	const int lesser = MIN(width, height);

	// We should be able to just store the obstacles and path once
	// Do need to update based on the window size
	std::vector<Coord> vObstacles = guiMapData.copyObstacles();
	Coord maxXY = guiMapData.copyMaxCoord();
	Coord originCoord = guiMapData.copyStartCoord();
	Coord goalCoord = guiMapData.copyEndCoord();

	// These have to be updated each iteration
	originCoord.x = int( float(width)*float(originCoord.x)/float(maxXY.x) );
	originCoord.y = int( float(height)*float(originCoord.y)/float(maxXY.y) );
	goalCoord.x = int( float(width)*float(goalCoord.x)/float(maxXY.x) );
	goalCoord.y = int( float(height)*float(goalCoord.y)/float(maxXY.y) );

	// Draw obstacles
	std::vector<Coord> scaledObstacleCoord;
	std::vector<Coord> rawObstacleCoord = guiMapData.copyObstacles();
	Coord stdCoord;

	// Adjust obstacle values based on window size
	for(std::vector<Coord>::const_iterator itr=rawObstacleCoord.begin();itr!=rawObstacleCoord.end();++itr)
	{
		stdCoord.x = int( float(width)*float(itr->x)/float(maxXY.x) );
		stdCoord.y = int( height*float(itr->y)/float(maxXY.y) );
		scaledObstacleCoord.push_back(stdCoord);
	}

	cr->save();
	cr->set_source_rgb(0.0, 0.0, 0.0);	// black for obstacles
	cr->set_line_width(lesser * 0.005);
	cr->set_line_cap(Cairo::LINE_CAP_ROUND);

	// Plot obstacles
	for(std::vector<Coord>::iterator itr=scaledObstacleCoord.begin();itr != scaledObstacleCoord.end();++itr)
	{
		cr->move_to( itr->x,itr->y );
		cr->line_to( itr->x,itr->y );
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
}

void MapDrawArea::DrawOptimalPath(const Cairo::RefPtr<Cairo::Context>& cr)
{
	// This is where we draw on the window
	Gtk::Allocation allocation = get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();
	const int lesser = MIN(width, height);
	const Coord maxXY = guiMapData.copyMaxCoord();

	// Copy the optimal path to the draw area
	std::vector<Coord> optimalPath = guiMapData.copyOptPath();

	// Plot the path
	cr->save();
	cr->set_source_rgb(1.0, 0.08, 0.58);	// pink for path
	cr->set_line_width(lesser * 0.005);
	cr->set_line_cap(Cairo::LINE_CAP_ROUND);

	for(std::vector<Coord>::iterator itr=optimalPath.begin();itr != optimalPath.end();++itr)
	{
		cr->move_to( int( float(width)*float(itr->x)/float(maxXY.x) ),int( height*float(itr->y)/float(maxXY.y)));
		cr->line_to( int( float(width)*float(itr->x)/float(maxXY.x) ),int( height*float(itr->y)/float(maxXY.y)));
		cr->stroke();
	}
}

void MapDrawArea::ClearDrawingArea(const Cairo::RefPtr<Cairo::Context>& cr)
{
	cr->set_source_rgb(255,255,255);
	cr->paint();
}

MapWindow::~MapWindow()
{
}

// Handler for popup dialog
void MapWindow::on_button_file_clicked()
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

	m_draw.guiMapData.MapState = UNINITIALIZED;

	//Handle the response:
	switch(result)
	{
		case(Gtk::RESPONSE_OK):
		{
			// Clear out the obstacles and optimal path
			m_draw.guiMapData.ClearMap();

			// Set the file names
			inputFilename = dialog.get_filename().c_str();
			outputFilename = inputFilename + ".out";
			outputFilename = outputFilename.c_str();

			// Initialize the obstacles and start/goal coordinates
			m_draw.guiMapData.InitMap(inputFilename);
			if(m_draw.guiMapData.MapInitialized)
			{
				m_draw.guiMapData.MapState = OBSTACLES_ONLY;
				// Enable the Solve button
				m_Button_Solve.set_sensitive(true);
			}
			else
				m_draw.guiMapData.MapState = UNINITIALIZED;
			break;
		}

		default:
			break;
	}
}

// Button handler for Solve button
void MapWindow::on_button_solve_clicked()
{
	// Show the spinner
	DialogWindow.show();

	// Spawn a new worker thread
	if( !m_WorkerThread)
		m_WorkerThread = Glib::Threads::Thread::create(sigc::bind(sigc::mem_fun(worker, &Worker::solve_path), this));
}

// Draw the obstacles, start point, end point, final path
bool MapDrawArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	switch(guiMapData.MapState)
	{
		case UNINITIALIZED:
			ClearDrawingArea(cr);
			break;
		case OBSTACLES_ONLY:
			DrawObstacles(cr);
			break;
		case FULL_MAP:
			DrawObstacles(cr);
			DrawOptimalPath(cr);
			break;
	}

	return true;
}

// Function executed after the map has been solved
void MapWindow::on_notification_from_worker_thread()
{
	if (m_WorkerThread && worker.has_stopped())
	{
		// Work is done.
		DialogWindow.StopSpinner();
		m_WorkerThread->join();
		m_WorkerThread = NULL;

		// Update buttons
		m_Button_Solve.set_sensitive(false);
		m_Button_File.set_sensitive(true);

		// Force the on_draw function
		m_draw.queue_draw();

		// Hide the spinner window
		DialogWindow.hide();
	}
}

void MapWindow::notify()
{
  m_Dispatcher.emit();
}
