#ifndef GTKMM_EXAMPLEWINDOW_H
#define GTKMM_EXAMPLEWINDOW_H

#include <gtkmm.h>
#include <astar.h>

// Child window of the main window
class MapDrawArea : public Gtk::DrawingArea
{
	public:
		MapDrawArea();
		virtual ~MapDrawArea();
		m_map guiMapData;
	protected:
		virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
};

class MapWindow : public Gtk::Window
{
	public:
		MapWindow();
		virtual ~MapWindow();
	protected:
		MapDrawArea m_draw;
};

// Main window
class ExampleWindow : public Gtk::Window
{
public:
  ExampleWindow();
  virtual ~ExampleWindow();

protected:
  //Signal handlers:
  void on_button_file_clicked();

  //Child widgets:
  Gtk::ButtonBox m_ButtonBox;
  Gtk::Button m_Button_File;
  Gtk::Button m_Button_Folder;
  //MapDrawArea m_draw;
};

#endif //GTKMM_EXAMPLEWINDOW_H
