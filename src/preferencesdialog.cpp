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


#include "preferencesdialog.h"
#include "params.h"
#include <gtkmm/stock.h>
#include <translation.h>

// Modèle pour le combo box de sélection du clavier
Gtk::TreeModelColumn<std::string  > PreferencesDialog::KbSelectorModel::code() const { return m_code; }
Gtk::TreeModelColumn<Glib::ustring> PreferencesDialog::KbSelectorModel::name() const { return m_name; }
PreferencesDialog::KbSelectorModel::KbSelectorModel() : Gtk::TreeModelColumnRecord() {
	add(m_code);
	add(m_name);
}

// Constructeur
PreferencesDialog::PreferencesDialog(Gtk::Window &parent) :
	Gtk::Dialog(_("Preferences"), parent, true, true)
{
	// Boutons de réponse
	add_button(Gtk::Stock::OK    , Gtk::RESPONSE_OK    );
	add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	set_default_response(Gtk::RESPONSE_OK);

	// Frame "RAZ par toolbar"
	raz_by_toolbar.set_label(_("Reset through the toolbar"));
	raz_by_toolbar_layout.set_border_width(5);
	raz_by_toolbar_layout.set_spacing(5);
	ask_before_raz_label.set_label(_("Ask for confirmation before resetting"));
	//ask_before_raz_label.set_justify(Gtk::JUSTIFY_LEFT);
	raz_by_toolbar_layout.pack_start(ask_before_raz_label);
	ask_before_raz[RC_ALWAYS       ].set_label(_("Always"                            ));
	ask_before_raz[RC_IF_NOT_PAUSED].set_label(_("Only if the clock is still running"));
	ask_before_raz[RC_NEVER        ].set_label(_("Never"                             ));
	for(ReinitConfirm::iterator k=ReinitConfirm::first(); k.valid(); ++k) {
		ask_before_raz[*k].set_group(ask_before_raz_group);
		raz_by_toolbar_layout.pack_start(ask_before_raz[*k]);
	}
	raz_by_toolbar.add(raz_by_toolbar_layout);

	// Frame "RAZ par clavier"
	raz_by_keyboard.set_label(_("Reset through the keyboard"));
	raz_by_keyboard_layout.set_border_width(5);
  raz_by_keyboard_layout.set_spacing(5);
	key_combination_label.set_label(_("Keys to press to reset"));
	//key_combination_label.set_justify(Gtk::JUSTIFY_LEFT);
	raz_by_keyboard_layout.pack_start(key_combination_label);
	key_combination[DOUBLE_CTRL].set_label(_("CTRL keys (left and right)"));
	key_combination[DOUBLE_MAJ ].set_label(_("MAJ keys (left and right)" ));
	for(KeyCombination::iterator k=KeyCombination::first(); k.valid(); ++k) {
		key_combination[*k].set_group(key_combination_group);
		raz_by_keyboard_layout.pack_start(key_combination[*k]);
	}
	raz_delay_label.set_label(_("Delay"));
	raz_delay_layout.set_spacing(5);
	raz_delay.set_digits(0);
	raz_delay.set_range(0, 2000);
	raz_delay.set_increments(100, 500);
	raz_delay_layout.pack_start(raz_delay_label, Gtk::PACK_SHRINK);
	raz_delay_layout.pack_start(raz_delay);
	raz_by_keyboard_layout.pack_start(raz_delay_layout);
	raz_by_keyboard.add(raz_by_keyboard_layout);

	// Onglet RAZ
	raz_page.set_border_width(5);
	raz_page.set_spacing(5);
	raz_page.pack_start(raz_by_toolbar );
	raz_page.pack_start(raz_by_keyboard);
	pages.append_page(raz_page, _("Reset options"));

	// Onglet keyboard (sauf géométrie)
	kb_selector_label.set_label(_("Keyboard layout"));
	kb_selector_data = Gtk::ListStore::create(kb_selector_model);
	kb_selector.set_model(kb_selector_data);
	kb_selector.signal_changed().connect(sigc::mem_fun(*this, &PreferencesDialog::on_kb_changed));
	kbm_widget.set_nb_areas(2);
	for(Side::iterator k=Side::first(); k.valid(); ++k) {
		kbm_widget.set_color((*k).to_int(), area_selector.color(*k));
	}
	area_selector.signal_changed().connect(sigc::mem_fun(*this, &PreferencesDialog::on_area_changed));

	// Onglet keyboard (géométrie)
	kb_page.set_border_width(5);
	kb_page.set_spacing(5);
	kb_config_layout.set_spacing(5);
	kb_config_layout.pack_start(kb_selector_label, Gtk::PACK_SHRINK);
	kb_selector.pack_start(kb_selector_model.name());
	kb_config_layout.pack_start(kb_selector);
	kb_config_layout.pack_start(area_selector, Gtk::PACK_SHRINK);
	kb_page.pack_start(kbm_widget);
	kb_page.pack_start(kb_config_layout, Gtk::PACK_SHRINK);
	pages.append_page(kb_page, _("Keyboard"));

	// Géométrie générale
	get_vbox()->set_spacing(5);
	get_vbox()->pack_start(pages);
	show_all_children();
}

// Chargement des paramètres
void PreferencesDialog::load_params() {

	// Paramètres RAZ
	ask_before_raz [gp->reinit_confirm()].set_active(true);
	key_combination[gp->reinit_keys   ()].set_active(true);
	raz_delay.set_value(gp->reinit_delay());

	// Liste des claviers
	std::set<std::string> keyboards = gp->keyboards();
	std::string       curr_keyboard = gp->curr_keyboard();
	for(std::set<std::string>::const_iterator it=keyboards.begin(); it!=keyboards.end(); ++it) {
		Glib::ustring kb_name = gp->keyboard_name(*it);
		Gtk::TreeModel::iterator row = kb_selector_data->append();
		(*row)[kb_selector_model.code()] = *it;
		(*row)[kb_selector_model.name()] = kb_name;
		if(curr_keyboard==*it) {
			kb_selector.set_active(row);
		}
	}
}

// Enregistrement des paramètres
void PreferencesDialog::save_params() {
	for(ReinitConfirm::iterator k=ReinitConfirm::first(); k.valid(); ++k) {
		if(ask_before_raz[*k].get_active())
			gp->set_reinit_confirm(*k);
	}
	for(KeyCombination::iterator k=KeyCombination::first(); k.valid(); ++k) {
		if(key_combination[*k].get_active())
			gp->set_reinit_key(*k);
	}
	gp->set_reinit_delay(raz_delay.get_value());
}

// Callback appelé sur un changement de clavier
void PreferencesDialog::on_kb_changed() {
	std::string kb_code = (*kb_selector.get_active())[kb_selector_model.code()];
	kbm_widget.set_keyboard_map(gp->keyboard_map(kb_code));
}

// Changement de la zone active
void PreferencesDialog::on_area_changed() {
	if(area_selector.is_selecting())
		kbm_widget.set_active_area(area_selector.active_side().to_int());
	else
		kbm_widget.set_active_area(-1);
}
