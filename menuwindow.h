#ifndef GTKMM_MENUWINDOW_H
#define GTKMM_MENUWINDOW_H

#include <gtkmm.h>
#include "astar.h"
#include<string.h>

// Child area of the popup window MapWindow
class MapDrawArea : public Gtk::DrawingArea
{
	public:
		MapDrawArea();
		virtual ~MapDrawArea();
		m_map guiMapData;

	protected:
		virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

		void DrawObstacles(const Cairo::RefPtr<Cairo::Context>& cr);
		void DrawOptimalPath(const Cairo::RefPtr<Cairo::Context>& cr);
		void ClearDrawingArea(const Cairo::RefPtr<Cairo::Context>& cr);
};

// Child window of the main window
class MapWindow : public Gtk::Window
{
public:
	MapWindow();
	virtual ~MapWindow();
protected:
	MapDrawArea m_draw;

	//Child widgets
	Gtk::Box m_VertParentBox;

	// m_vertParentBox child widgets
	Gtk::Box m_HorzChildBox;

	// m_HorzChildBox child widgets
	Gtk::Button m_Button_File;
	Gtk::Button m_Button_Solve;

	//Signal handlers
	void on_button_file_clicked();
	void on_button_solve_clicked();

	// Chosen filename for input JSON file
	std::string inputFilename;
	std::string outputFilename;

	// Functions
	bool SolveOptimalPath();

};

#endif //GTKMM_MENUWINDOW_H
