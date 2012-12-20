/******************************************************************************
 *                                                                            *
 *    This file is part of Virtual Chess Clock, a chess clock software        *
 *                                                                            *
 *    Copyright (C) 2010-2012 Yoann Le Montagner <yo35(at)melix(dot)net>      *
 *                                                                            *
 *    This program is free software: you can redistribute it and/or modify    *
 *    it under the terms of the GNU General Public License as published by    *
 *    the Free Software Foundation, either version 3 of the License, or       *
 *    (at your option) any later version.                                     *
 *                                                                            *
 *    This program is distributed in the hope that it will be useful,         *
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *    GNU General Public License for more details.                            *
 *                                                                            *
 *    You should have received a copy of the GNU General Public License       *
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 *                                                                            *
 ******************************************************************************/


#include "timer.h"
#include <glibmm/main.h>

Timer::Timer() : Glib::Object() {
	m_mode = PAUSED;
	m_time = 0;
}

void Timer::set_mode(Mode new_mode) {
	if(m_mode==new_mode)
		return;

	if(m_mode != Timer::PAUSED)
		m_time = get_time();
	if(new_mode != Timer::PAUSED)
		gettimeofday(&m_start_at, 0);
	m_mode = new_mode;
}

Timer::Mode Timer::get_mode() const {
	return m_mode;
}

void Timer::set_time(int new_time) {
	if(m_mode != PAUSED) {
		m_mode = PAUSED;
	}
	m_time = new_time;
}

int Timer::get_time() const {

	// Si on est en pause, la valeur du timer est stockée dans m_time
	if(m_mode==PAUSED)
		return m_time;

	// Sinon, il faut calculer la valeur courante à partir de la date de lancement
	// du timer (m_start_at) et de la valeur du timer à cet instant (m_time)
	else {
		Timestamp now = get_timestamp_now();
		int diff = difftime(now, m_start_at);
		if(m_mode==INCREMENT)
			return m_time + diff;
		else
			return m_time - diff;
	}
}
