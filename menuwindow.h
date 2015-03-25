#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include "astar.h"
#include<string.h>
#include <iostream>
#include <gtkmm/box.h>

class MapWindow;
class SpinnerWindow;

// Worker class for keeping timer
class TimerWorker {
public:
	TimerWorker();

	// Solve path thread function
	bool has_stopped();
	void has_stopped(bool stopped);
	void start_timer();
	void get_timer(double *timer_var);
	void keep_time(SpinnerWindow *window);

protected:

	// Needed for shared data
	Glib::Threads::Mutex m_Mutex;

	// Shared data
	bool m_has_stopped = false;
	timeval BeginTimer,EndTimer;
	double m_time_elapsed = 0.0;
};

// Spinner window showing a spinner while astar is executing
class SpinnerWindow : public Gtk::Window
{
public:
	// Functions
	void StopSpinner() { spinner.stop(); };
	void StartSpinner() { spinner.start(); };
	void spawn_timer_thread();
	SpinnerWindow();

	void notify();				// Used by worker thread
	void update_timer();
	void notification();

protected:
	// Data objects
	Gtk::Box SpinnerBox;
	Gtk::Spinner spinner;
	Glib::RefPtr< Gtk::TextBuffer > WaitBuffer;
	Gtk::Label WaitLabel;
	Glib::RefPtr< Gtk::TextBuffer > TimeBuffer;
	Gtk::Label TimeElapsed;

	// Thread to keep track of the time
	TimerWorker timer_thread;
	Glib::Threads::Thread *m_TimerWorkerThread;
	Glib::Dispatcher m_Dispatcher;
};

// Worker class for parallel processing in GUI
class Worker {
public:
	Worker();

	// Solve path thread function
	void solve_path(MapWindow *window);
	bool has_stopped();
	bool get_solution_exists() { return m_solution_exists; }

protected:

	// Needed for shared data
	Glib::Threads::Mutex m_Mutex;

	// Shared data
	bool m_has_stopped = false;
	bool m_solution_exists = true;
};

// Child area of the popup window MapWindow
class MapDrawArea : public Gtk::DrawingArea
{
	public:
		// Data objects
		m_map guiMapData;

		// Functions
		MapDrawArea();
		virtual ~MapDrawArea();

	protected:
		// Functions
		virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
		void DrawObstacles(const Cairo::RefPtr<Cairo::Context>& cr);
		void DrawOptimalPath(const Cairo::RefPtr<Cairo::Context>& cr);
		void ClearDrawingArea(const Cairo::RefPtr<Cairo::Context>& cr);
};

// Child window of the main window
class MapWindow : public Gtk::Window
{
public:

	// Functions
	MapWindow();
	virtual ~MapWindow();
	bool SolveOptimalPath();
	void notify();				// Used by the worker thread

protected:
	// Data objects
	/* Child widgets */
	MapDrawArea m_draw;
	Gtk::Box m_VertParentBox;
	/* m_vertParentBox child widgets */
	Gtk::Box m_HorzChildBox;
	/* m_HorzChildBox child widgets */
	Gtk::Button m_Button_File;
	Gtk::Button m_Button_Solve;
	/* Chosen filename for input JSON file */
	std::string inputFilename;
	std::string outputFilename;
	/* Threading */
	Worker worker;
	Glib::Dispatcher m_Dispatcher;
	Glib::Threads::Thread *m_WorkerThread;
	/* Child spinner window */
	SpinnerWindow DialogWindow;

	// Functions
	/* Signal handlers */
	void on_button_file_clicked();
	void on_button_solve_clicked();
	void update_buttons();
	/* Threading */
	void on_notification_from_worker_thread();
	void update_timer();
};

#endif //GTKMM_MENUWINDOW_H
