#ifndef GTKMM_MENUWINDOW_H
#define GTKMM_MENUWINDOW_H

#include <gtkmm.h>
#include "astar.h"
#include<string.h>
#include "worker.h"

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

// Spinner window showing a spinner while astar is executing
class SpinnerWindow : public Gtk::Window
{
public:
	void StopSpinner() { spinner.stop(); };
	void StartSpinner() { spinner.start(); };
	SpinnerWindow();
protected:
	Gtk::Box SpinnerBox;
	Gtk::Spinner spinner;
	Glib::RefPtr< Gtk::TextBuffer > WaitBuffer;
	Gtk::Label WaitLabel;
};

// Child window of the main window
class MapWindow : public Gtk::Window
{
public:
	MapWindow();
	virtual ~MapWindow();

	bool SolveOptimalPath();

	// Used by the worker thread
	void notify();

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
	void update_buttons();

	// Chosen filename for input JSON file
	std::string inputFilename;
	std::string outputFilename;

	// Functions

	// Threading
	Worker worker;
	Glib::Dispatcher m_Dispatcher;
	Glib::Threads::Thread *m_WorkerThread;
	void on_notification_from_worker_thread();

	// Pointers for popup window
	SpinnerWindow DialogWindow;
};

#endif //GTKMM_MENUWINDOW_H
