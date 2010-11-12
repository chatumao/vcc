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


#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <gtkmm/dialog.h>
#include <gtkmm/notebook.h>
#include <gtkmm/box.h>
#include <gtkmm/frame.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/label.h>
#include "optionenums.h"

class PreferencesDialog : public Gtk::Dialog {

public:

	// Constructeur
	PreferencesDialog(Gtk::Window &parent);

private:

	// Données membres
	Gtk::Notebook pages;

	// Onglet RAZ
	Gtk::VBox                                   raz_page;
	Gtk::Frame                                  raz_by_toolbar;
	Gtk::VBox                                   raz_by_toolbar_layout;
	Gtk::Label                                  ask_before_raz_label;
	Gtk::RadioButton::Group                     ask_before_raz_group;
	EnumArray<ReinitConfirm, Gtk::RadioButton>  ask_before_raz;
	Gtk::Frame                                  raz_by_keyboard;
	Gtk::VBox                                   raz_by_keyboard_layout;
	Gtk::Label                                  key_combination_label;
	Gtk::RadioButton::Group                     key_combination_group;
	EnumArray<KeyCombination, Gtk::RadioButton> key_combination;
};

#endif
