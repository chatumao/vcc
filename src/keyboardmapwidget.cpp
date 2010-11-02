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


#include "keyboardmapwidget.h"
#include <cassert>

KeyboardMapWidget::KeyboardMapWidget() : Gtk::DrawingArea() {
	m_kbm = 0;
	set_size_request(600, 400);
}

void KeyboardMapWidget::set_keyboard_map(const KeyboardMap &kbm) {
	m_kbm = &kbm;
	refresh_widget();
}

void KeyboardMapWidget::refresh_widget() {
	Glib::RefPtr<Gdk::Window> window = get_window();
	if(window == 0)
		return;

	Gtk::Allocation allocation = get_allocation();
	Gdk::Rectangle r(0, 0, allocation.get_width(), allocation.get_height());
	window->invalidate_rect(r, false);
}

bool KeyboardMapWidget::on_expose_event(GdkEventExpose *event) {
	Glib::RefPtr<Gdk::Window> window = get_window();
	if(window == 0)
		return false;
	cr = window->create_cairo_context();
	cr->rectangle(event->area.x, event->area.y, event->area.width, event->area.height);
	cr->clip();

	Gtk::Allocation allocation = get_allocation();
	double width  = allocation.get_width();
	double height = allocation.get_height();

	// Le fond de l'image
	cr->set_source_rgb(0.8, 1.0, 0.8);
	cr->paint();
	if(m_kbm==0)
		return true;

	// Transformation
	delta_x =  0.1*width ;
	delta_y =  0.9*height;
	scale_x =  0.8*width  / static_cast<double>(m_kbm->line_width());
	scale_y = -0.8*height / static_cast<double>(m_kbm->nb_lines());
	margin  = 0.007*(height < width ? height : width);
	radius  = 0.02 *(height < width ? height : width);

	// Touches
	cr->set_source_rgb(0.0, 0.0, 0.0);
	for(unsigned idx=0; idx<m_kbm->keys().size(); ++idx) {
		draw_key(idx);
	}
	return true;
}

// Dessine une touche
void KeyboardMapWidget::draw_key(unsigned int idx) {
	assert(m_kbm!=0);
	assert(idx<m_kbm->keys().size());

	// Cas d'une touche simple
	if(m_kbm->keys()[idx].x_lines.size()==1) {
		int x0 = m_kbm->keys()[idx].x_lines.front().pos;
		int y0 = m_kbm->keys()[idx].bottom_line;
		int dx = m_kbm->keys()[idx].x_lines.front().width;
		int dy = 1;
		make_rectangle(x0, y0, dx, dy);
	}

	// Cas d'une touche située à cheval sur plusieurs lignes
	else {
		std::list<int> xs;
		std::list<int> ys;
		for(KeySizeList::const_iterator it=m_kbm->keys()[idx].x_lines.begin();
			it!=m_kbm->keys()[idx].x_lines.end(); ++it)
		{
			xs.push_back(it->pos);
		}
		for(KeySizeList::const_reverse_iterator it=m_kbm->keys()[idx].x_lines.rbegin();
			it!=m_kbm->keys()[idx].x_lines.rend(); ++it)
		{
			xs.push_back(it->pos + it->width);
		}
		int first_x = xs.back();
		xs.pop_back();
		xs.push_front(first_x);
		for(unsigned int dy=0; dy<m_kbm->keys()[idx].x_lines.size(); ++dy) {
			ys.push_back(m_kbm->keys()[idx].bottom_line + dy);
		}
		for(unsigned int dy=m_kbm->keys()[idx].x_lines.size(); dy>0; --dy) {
			ys.push_back(m_kbm->keys()[idx].bottom_line + dy);
		}
		make_polygone(xs, ys);
	}

	cr->set_source_rgb(1.0, 1.0, 1.0);
	cr->fill_preserve();
	//cr->set_source_rgb(1.0, 0.0, 0.0);
	//cr->stroke_preserve();
}

// Dessine un rectangle échencré
void KeyboardMapWidget::make_rectangle(int x0, int y0, int dx, int dy) {
	double xl = x_conv(x0)    + margin;
	double xr = x_conv(x0+dx) - margin;
	double yb = y_conv(y0)    - margin;
	double yt = y_conv(y0+dy) + margin;
	cr->begin_new_path();
	cr->move_to(xl+radius, yb);
	cr->arc(xl+radius, yb-radius, radius, M_PI_2, M_PI);   // SO
	cr->line_to(xl, yt+radius);
	cr->arc(xl+radius, yt+radius, radius, -M_PI, -M_PI_2); // NO
	cr->line_to(xr-radius, yt);
	cr->arc(xr-radius, yt+radius, radius, -M_PI_2, 0.0);   // NE
	cr->line_to(xr, yb-radius);
	cr->arc(xr-radius, yb-radius, radius, 0.0, M_PI_2);    // SE
	cr->close_path();
}

// Dessine un polygone échencré
void KeyboardMapWidget::make_polygone(const std::list<int> &xs, const std::list<int> &ys) {
	assert(xs.size()==ys.size());
	assert(xs.size()>=2);

	// Les indices pairs sont des x (arètes verticales),
	// les indices impairs des y (arètes horizontales)
	std::vector<double> aretes(2*xs.size()+2);
	std::list<int>::const_iterator x = xs.begin();
	std::list<int>::const_iterator y = ys.begin();
	for(unsigned int idx=0; idx<xs.size(); ++idx) {
		aretes[2*idx  ] = x_conv(*x);
		aretes[2*idx+1] = y_conv(*y);
		++x;
		++y;
	}
	aretes[2*xs.size()  ] = aretes[0];
	aretes[2*xs.size()+1] = aretes[1];

	// Corrections margin
	std::vector<double> aretes_margin = aretes;
	for(unsigned int idx=0; idx<xs.size(); ++idx) {
		aretes_margin[2*idx+1] += -margin * sgn(aretes[2*idx  ] - aretes[2*idx+2]);
		aretes_margin[2*idx+2] +=  margin * sgn(aretes[2*idx+1] - aretes[2*idx+3]);
	}
	aretes_margin[0] = aretes_margin[2*xs.size()];
	aretes_margin[2*xs.size()+1] = aretes_margin[1];

	// Rayon ajusté du début de l'arète
	std::vector<double> radius_aj(2*xs.size()+2);
	for(unsigned int idx=0; idx<=2*xs.size()+1; ++idx) {
		radius_aj[idx] = radius;
	}
	for(unsigned int idx=1; idx<=2*xs.size(); ++idx) {
		radius_aj[idx] = min(radius_aj[idx], abs(aretes_margin[idx-1]-aretes_margin[idx+1])/2);
	}
	radius_aj[0] = radius_aj[2*xs.size()];
	radius_aj[2*xs.size()+1] = radius_aj[1];
	for(unsigned int idx=2; idx<=2*xs.size()+1; ++idx) {
		radius_aj[idx] = min(radius_aj[idx], abs(aretes_margin[idx]-aretes_margin[idx-2])/2);
	}
	radius_aj[0] = radius_aj[2*xs.size()  ];
	radius_aj[1] = radius_aj[2*xs.size()+1];

	// Coordonnées de stop des arètes
	// Rq : inversion de la parité
	std::vector<double> start_at(2*xs.size()+2);
	std::vector<double> stop_at (2*xs.size()+2);
	for(unsigned int idx=1; idx<=2*xs.size(); ++idx) {
		start_at[idx] = aretes_margin[idx-1] - radius_aj[idx]  *sgn(aretes[idx-1]-aretes[idx+1]);
		stop_at [idx] = aretes_margin[idx+1] + radius_aj[idx+1]*sgn(aretes[idx-1]-aretes[idx+1]);
	}
	start_at[0] = start_at[2*xs.size()];
	stop_at [0] = stop_at [2*xs.size()];
	start_at[2*xs.size()+1] = start_at[1];
	stop_at [2*xs.size()+1] = stop_at [1];

	// Angles de départ au début de l'arète
	std::vector<double> angle_dep(2*xs.size()+2);
	for(unsigned int idx=0; idx<xs.size(); ++idx) {
		angle_dep[2*idx+1] = (aretes[2*idx+2] < aretes[2*idx  ]) ? 0.0 : -M_PI;
		angle_dep[2*idx+2] = (aretes[2*idx+3] < aretes[2*idx+1]) ? M_PI_2 : -M_PI_2;
	}
	angle_dep[0] = angle_dep[2*xs.size()];
	angle_dep[2*xs.size()+1] = angle_dep[1];

	// Angles d'arrivée au début de l'arète
	std::vector<double> angle_arr(angle_dep);
	for(unsigned int idx=0; idx<xs.size(); ++idx) {
		angle_arr[2*idx+2] +=  M_PI_2*sgn(aretes[2*idx+3]-aretes[2*idx+1])*sgn(aretes[2*idx+2]-aretes[2*idx  ]);
		angle_arr[2*idx+3] += -M_PI_2*sgn(aretes[2*idx+3]-aretes[2*idx+1])*sgn(aretes[2*idx+4]-aretes[2*idx+2]);
	}
	angle_arr[0] = angle_arr[2*xs.size()  ];
	angle_arr[1] = angle_arr[2*xs.size()+1];

	// Tracé
	cr->begin_new_path();
	cr->move_to(aretes_margin[0], stop_at[0]);
	for(unsigned int idx=0; idx<xs.size(); ++idx) {
		small_arc(start_at[2*idx+1], stop_at[2*idx], radius_aj[2*idx+1],
			angle_dep[2*idx+1], angle_arr[2*idx+1]);
		cr->line_to(stop_at [2*idx+1], aretes_margin[2*idx+1]);
		small_arc(stop_at[2*idx+1], start_at[2*idx+2], radius_aj[2*idx+2],
			angle_dep[2*idx+2], angle_arr[2*idx+2]);
		cr->line_to(aretes_margin[2*idx+2], stop_at [2*idx+2]);
	}
	cr->close_path();
}

// Arc de cercle
void KeyboardMapWidget::small_arc(double xc, double yc, double r0, double angle1, double angle2) {
	if(angle1 < angle2)
		cr->arc(xc, yc, r0, angle1, angle2);
	else
		cr->arc_negative(xc, yc, r0, angle1, angle2);
}

// Conversions x et y
double KeyboardMapWidget::x_conv(int x) const { return static_cast<double>(x)*scale_x + delta_x; }
double KeyboardMapWidget::y_conv(int y) const { return static_cast<double>(y)*scale_y + delta_y; }

// Fonctions maths
double KeyboardMapWidget::sgn(double src) {
	return src==0 ? 0 : (src>0 ? 1 : -1);
}
double KeyboardMapWidget::abs(double src) {
	return src<0 ? -src : src;
}
double KeyboardMapWidget::min(double s1, double s2) {
	return s1 < s2 ? s1 : s2;
}
