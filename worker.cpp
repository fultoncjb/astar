/*
 * worker.cpp
 *
 *  Created on: Feb 17, 2015
 *      Author: cameron
 */

#include "worker.h"
#include "menuwindow.h"

Worker::Worker() { };

void Worker::solve_path(MapWindow *window)
{
	Glib::Threads::Mutex::Lock lock(m_Mutex);
	m_has_stopped = false;

	bool result = window->SolveOptimalPath();

	m_has_stopped = true;
	lock.release();
	window->notify();
}

bool Worker::has_stopped()
{
  Glib::Threads::Mutex::Lock lock(m_Mutex);
  return m_has_stopped;
}
