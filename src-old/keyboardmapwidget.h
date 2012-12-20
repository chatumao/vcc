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


#ifndef KEYBOARDMAPWIDGET_H
#define KEYBOARDMAPWIDGET_H

#include <gtkmm/drawingarea.h>
#include "keyboardmap.h"
#include "areamap.h"
#include "side.h"

class KeyboardMapWidget : public Gtk::DrawingArea {

public:

	// Divers
	KeyboardMapWidget();
	void set_keyboard_map(const KeyboardMap &kbm);
	const Gdk::Color &color(const Side &side) const;

	// Affichage pavé numérique
	bool display_kp() const;
	void set_display_kp(bool src);

	// Région active
	bool is_active  () const;
	Side active_side() const;
	void set_active_side(const Side &side);
	void unset_active_side();

	// Régions
	const AreaMap &get_area_map() const;
	void set_area_map(const AreaMap &src);
	void clear_areas();

	// Action sur une touche clavier
	void on_key_action(Keyval keyval, bool is_press);

protected:

	// Routine de dessin
	virtual bool on_expose_event(GdkEventExpose *event);

	// Événements clavier et souris
	virtual bool on_button_press_event(GdkEventButton *event);

private:

	// Force le redessin du widget
	void refresh_widget();

	// Recherche de touche par position géométrique
	int lookup_key_at(double x, double y) const;

	// Routines de dessin
	void draw_key_text (unsigned int idx);
	double compute_text_size(double xl, double xr, double yt, double yb,
		const Glib::ustring &txt, double expected_relative_font_size);
	void make_text(double xl, double xr, double yt, double yb,
		const Glib::ustring &txt, double font_size);
	void draw_key_shape(unsigned int idx);
	void make_rectangle(int x0, int y0, int dx, int dy);
	void make_polygone(const std::list<int> &xs, const std::list<int> &ys);
	void small_arc(double xc, double yc, double r0, double angle1, double angle2);
	double x_conv  (int    x) const;
	double y_conv  (int    y) const;
	int    x_deconv(double x) const;
	int    y_deconv(double y) const;
	static double sgn(double src);
	static double abs(double src);
	static double min(double s1, double s2);

	// Données membres
	const KeyboardMap          *m_kbm        ;
	std::vector<bool>           m_keydown    ;
	AreaMap                     m_keyarea    ;
	EnumArray<Side, Gdk::Color> m_color      ;
	bool                        m_display_kp ;
	bool                        m_is_active  ;
	Side                        m_active_side;

	// Objets liés au dessin du widget
	Cairo::RefPtr<Cairo::Context> cr;
	double delta_x;
  double delta_y;
  double scale_x;
  double scale_y;
	double margin ;
	double radius ;
	double sml_pad;
	double big_pad;
};

#endif
