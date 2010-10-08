/******************************************************************************
 *                                                                            *
 *    This file is part of Virtual Chess Clock, a chess clock software        *
 *                                                                            *
 *    Copyright (C) 2010  Yoann Le Montagner <yo35(at)melix(dot)net>          *
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


#include "params.h"
#include <config.h>
#include <cassert>
#include <fstream>
#include <string>
#include <stdexcept>
#include <libintl.h>

Params *gp;

Params::Params(const std::string &prefix_path) :
	icon_reset(prefix_path + "/" + VCC_SHARE_RPATH + "/reset.png"),
	icon_pause(prefix_path + "/" + VCC_SHARE_RPATH + "/pause.png"),
	icon_tctrl(prefix_path + "/" + VCC_SHARE_RPATH + "/tctrl.png"),
	m_prefix_path(prefix_path)
{
	// Cadence
	time_control.set_mode(TimeControl::FISCHER);
	time_control.set_main_time(180*1000);
	time_control.set_increment(  2*1000);

	// Zones actives
	KeyvalList    keyval_left ;
	KeyvalList    keyval_right;
	KeyvalList   *curr_area = 0;
	std::string   path = prefix_path + "/" + VCC_SHARE_RPATH  + "/keymap.txt";
	std::ifstream file;
	file.open(path.c_str());
	if(file.fail())
		throw std::runtime_error(gettext("Unable to open the keyboard configuration file"));
	while(!file.eof()) {
		std::string line;
		getline(file, line);

		// Cas d'une ligne vide
		if(line=="")
			continue;

		// Cas d'un changement de zone
		if(line=="LEFT")
			curr_area = &keyval_left;
		else if(line=="RIGHT")
			curr_area = &keyval_right;

		// Autre cas : on s'attend à un nombre
		else {
			int nb = 0;
			for(unsigned int k=0; k<line.length(); ++k) {
				if(line[k]<'0' || line[k]>'9')
					throw std::runtime_error(gettext("The keyboard configuration file is corrupted"));
				nb = nb*10 + line[k] - '0';
			}
			if(curr_area==0)
				throw std::runtime_error(gettext("The keyboard configuration file is corrupted"));
			curr_area->push_back(nb);
		}
	}
	file.close();
	init_kb_areas(keyval_left, keyval_right);
}

void Params::init_kb_areas(const KeyvalList &area_left, const KeyvalList &area_right) {

	// Chargement des listes de touches gauches et droites
	key_area[0] = aux_init_kb_areas(area_left );
	key_area[1] = aux_init_kb_areas(area_right);

	// On vérifie que l'intersection des deux ensembles est vide
	std::set<Keycode> area_both;
	area_both.insert(key_area[0].begin(), key_area[0].end());
	area_both.insert(key_area[1].begin(), key_area[1].end());
	assert(area_both.size()==key_area[0].size()+key_area[1].size());
}

std::set<Keycode> Params::aux_init_kb_areas(const KeyvalList &src) {
	std::set<Keycode> retval;
	for(KeyvalList::const_iterator it=src.begin(); it!=src.end(); ++it) {
		KeycodeList keycodes = keyval_to_keycodes(*it);
		retval.insert(keycodes.begin(), keycodes.end());
	}
	return retval;
}

const std::string &Params::prefix_path() const {
	return m_prefix_path;
}
