#ifndef GTKMM_MENUWINDOW_H
#define GTKMM_MENUWINDOW_H

#include <gtkmm.h>
#include <astar.h>

// Child area of the popup window MapWindow
class MapDrawArea : public Gtk::DrawingArea
{
	public:
		MapDrawArea();
		virtual ~MapDrawArea();

		m_map guiMapData;
	protected:
		virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
};

// Main window
class MenuWindow : public Gtk::Window
{
	public:
  
		// Child window of the main window
		class MapWindow : public Gtk::Window
		{
		public:
			MapWindow();
	
			// Constructor for MenuWindow input
			explicit MapWindow(const MenuWindow &e);
			virtual ~MapWindow();
		protected:
			MapDrawArea m_draw;
			//MapErrorWindow m_error;
			
		};
		MenuWindow();
		virtual ~MenuWindow();

	protected:
		//Signal handlers
		void on_button_file_clicked();

		//Child widgets
		Gtk::ButtonBox m_ButtonBox;
		Gtk::Button m_Button_File;

		// Chosen filename for input JSON file
		std::string inputFilename;
		std::string outputFilename;
};

#endif //GTKMM_MENUWINDOW_H
