#include "examplewindow.h"
#include <iostream>

ExampleWindow::ExampleWindow()
: m_ButtonBox(Gtk::ORIENTATION_VERTICAL),
  m_Button_File("Choose File"),
  m_Button_Folder("Choose Folder")
{
  set_title("Astar");
  //maximize();

  m_ButtonBox.pack_start(m_Button_File);
  m_Button_File.signal_clicked().connect(sigc::mem_fun(*this,
              &ExampleWindow::on_button_file_clicked) );

  add(m_ButtonBox);

  show_all_children();
}

ExampleWindow::~ExampleWindow()
{
}

MapDrawArea::~MapDrawArea()
{
}

MapDrawArea::MapDrawArea()
{
}

MapWindow::MapWindow()
: m_draw()
{
  set_title("Astar Map Window");
  maximize();

  std::string filename = "/home/cameron/Documents/astar/DataSets/set1.dat";
  std::string outputFilename = filename + ".out";
  
  
  m_draw.guiMapData.getPath( filename.c_str(), outputFilename.c_str() );
 
  add(m_draw);

  show_all_children();
}

MapWindow::~MapWindow()
{
}

void ExampleWindow::on_button_file_clicked()
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

  //Handle the response:
  switch(result)
  {
    case(Gtk::RESPONSE_OK):
    {
      std::cout << "Open clicked." << std::endl;	

      //Notice that this is a std::string, not a Glib::ustring.
      // Need to pass this to the astar algorithm
      std::string filename = dialog.get_filename();
      std::cout << "File selected: " <<  filename << std::endl;
      // Going to have to return the path, obstacles, start, and end coordinates to draw the image here
      std::string outputFilename = dialog.get_filename() + ".out";
      //m_map astarMap;
      //astarMap.getPath( filename.c_str(), outputFilename.c_str() );


      // Create child window and plot obstacles
      // TODO: plot path
      MapWindow *popupWin = new MapWindow;
      popupWin->show();

	// Copy the map to the new popup window
	
      
      
      break;
    }
    case(Gtk::RESPONSE_CANCEL):
    {
      std::cout << "Cancel clicked." << std::endl;
      break;
    }
    default:
    {
      std::cout << "Unexpected button clicked." << std::endl;
      break;
    }
  }
}

bool MapDrawArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
  // This is where we draw on the window
  Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();
  const int lesser = MIN(width, height);

  std::vector<m_map::coord> vObstacles = guiMapData.copyObstacles();
  m_map::coord maxXY = guiMapData.copyMaxCoord();
  m_map::coord originCoord = guiMapData.copyStartCoord();
  originCoord.x = int( float(width)*float(originCoord.x)/float(maxXY.x) );
  originCoord.y = int( float(height)*float(originCoord.y)/float(maxXY.y) );
  m_map::coord goalCoord = guiMapData.copyEndCoord();
  goalCoord.x = int( float(width)*float(goalCoord.x)/float(maxXY.x) );
  goalCoord.y = int( float(height)*float(goalCoord.y)/float(maxXY.y) );
  std::vector<m_map::coord> optimalPath = guiMapData.copyOptPath();

  // Draw obstacles
  std::vector<m_map::coord> scaledObsCoord;
  m_map::coord stdCoord;
  // Adjust obstacle values based on window size
  for(std::vector<m_map::coord>::iterator itr=vObstacles.begin();itr!=vObstacles.end();++itr)
  {
	stdCoord.x = int( float(width)*float((*itr).x)/float(maxXY.x) );
        stdCoord.y = int( height*float((*itr).y)/float(maxXY.y) );
	scaledObsCoord.push_back(stdCoord);
  }

  cr->save();
  cr->set_source_rgb(0.0, 0.0, 0.0);
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
  cr->set_source_rgb(1.0, 0.0, 0.0);
  cr->move_to( originCoord.x,originCoord.y );
  cr->line_to( originCoord.x,originCoord.y );
  cr->stroke();
  cr->save();
  cr->set_source_rgb(0.0, 1.0, 0.0);
  cr->move_to( goalCoord.x,goalCoord.y );
  cr->line_to( goalCoord.x,goalCoord.y );
  cr->stroke();


  // Plot the path
  cr->save();
  cr->set_source_rgb(1.0, 0.08, 0.58);
  cr->set_line_width(lesser * 0.005);

  for(std::vector<m_map::coord>::iterator itr=optimalPath.begin();itr != optimalPath.end();++itr)
  {
	cr->move_to( int( float(width)*float((*itr).x)/float(maxXY.x) ),int( height*float((*itr).y)/float(maxXY.y) ));
	cr->line_to( int( float(width)*float((*itr).x)/float(maxXY.x) ),int( height*float((*itr).y)/float(maxXY.y) ) );
  	cr->stroke();
  }
  
  
  return true;
}

// TODO: create a method to communicate filename and output filename from the ExampleWindow to the Drawing Area
// In order to do this, the DrawingArea will probably have to be a child of the ExampleWindow
