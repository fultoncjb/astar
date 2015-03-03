/*
 * worker.h
 *
 *  Created on: Feb 16, 2015
 *      Author: cameron
 */

#ifndef WORKER_H_
#define WORKER_H_

#include<gtkmm.h>

class MapWindow;

// Worker class for parallel processing in GUI
class Worker {
public:
	Worker();

	// Solve path thread function
	void solve_path(MapWindow *window);
	bool has_stopped();

protected:

	// Needed for shared data
	Glib::Threads::Mutex m_Mutex;

	// Shared data
	bool m_has_stopped;
};


#endif /* WORKER_H_ */
