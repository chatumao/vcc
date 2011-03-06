/******************************************************************************
 *                                                                            *
 *    This file is part of Virtual Chess Clock, a chess clock software        *
 *                                                                            *
 *    Copyright (C) 2010-2011  Yoann Le Montagner <yo35(at)melix(dot)net>     *
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


#include "dialwidget.h"
#include "strings.h"
#include <translation.h>

DialWidget::DialWidget() : Gtk::DrawingArea() {
	m_bi_timer = 0;
	set_size_request(400, 300);
	Glib::signal_timeout().connect(sigc::mem_fun(*this, &DialWidget::on_timeout_elapses), 150);
}

void DialWidget::set_timer(const BiTimer &bi_timer, const Side &side) {
	m_bi_timer = &bi_timer;
	m_side     =  side    ;
	m_bi_timer->signal_state_changed().connect(sigc::mem_fun(*this, &DialWidget::refresh_widget));
}

// Rafraîchissement automatique du widget à intervals réguliers
bool DialWidget::on_timeout_elapses() {
	if(m_bi_timer==0) {
		return true;
	}
	if(m_bi_timer->time_control().mode()!=HOURGLASS &&
		(m_bi_timer->mode()!=BiTimer::ACTIVE || m_bi_timer->active_side()!=m_side))
	{
		return true;
	}
	refresh_widget();
	return true;
}

// Force le redessin du widget
void DialWidget::refresh_widget() {
	Glib::RefPtr<Gdk::Window> window = get_window();
	if(window == 0)
		return;

	Gtk::Allocation allocation = get_allocation();
	Gdk::Rectangle r(0, 0, allocation.get_width(), allocation.get_height());
	window->invalidate_rect(r, false);
}

// Routine de dessin
bool DialWidget::on_expose_event(GdkEventExpose *event) {
	Glib::RefPtr<Gdk::Window> window = get_window();
	if(window == 0)
		return false;
	Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
	cr->rectangle(event->area.x, event->area.y, event->area.width, event->area.height);
	cr->clip();
	if(m_bi_timer==0) {
		return true;
	}

	// Le fond de l'image
	bool is_active = m_bi_timer->mode()==BiTimer::ACTIVE && m_bi_timer->active_side()==m_side;
	if(is_active)
		cr->set_source_rgb(1.0, 1.0, 0.5);
	else
		cr->set_source_rgb(1.0, 1.0, 1.0);
	cr->paint();

	// Récupération du texte à afficher
	int curr_time, bronstein_extra_delay;
	if(m_bi_timer->time_control().mode()==BRONSTEIN) {
		curr_time = m_bi_timer->get_time_ex(m_side, bronstein_extra_delay);
	}
	else {
		curr_time = m_bi_timer->get_time(m_side);
	}

	// Formattage et affichage
	if(curr_time < 0) {
		cr->set_source_rgb(0.8, 0.0, 0.0);
		std::string  main_txt = _("Flag down");
		std::string extra_txt = _("Since:") + std::string(" ") + format_time(-curr_time);
		draw_two_lines_text(cr, main_txt, extra_txt);
	}
	else {
		cr->set_source_rgb(0.0, 0.0, 0.0);
		std::string txt = format_time(curr_time);
		if(m_bi_timer->time_control().mode()==BRONSTEIN && is_active) {
			std::string extra_txt = bronstein_extra_delay > 0
				? _("Extra period till:") + std::string(" ") + format_time(bronstein_extra_delay)
				: _("Main period");
			draw_two_lines_text(cr, txt, extra_txt);
		}
		else {
			draw_one_line_text(cr, txt);
		}
	}
	return true;
}

// Écrit un texte ne comportant qu'une seule ligne
void DialWidget::draw_one_line_text(Cairo::RefPtr<Cairo::Context> cr, const std::string &txt) {

	// Taille du widget
	Gtk::Allocation allocation = get_allocation();
	double width  = allocation.get_width();
	double height = allocation.get_height();

	// Patron du texte à afficher
	std::string txt_model = get_model_txt(txt);

	// Calcul de la position du texte
	Cairo::TextExtents te;
	double font_size = get_adjusted_font_height(cr, height*0.8, width*0.7, txt_model);
	cr->set_font_size(font_size);
	cr->get_text_extents(txt_model, te);
	double delta_x = (width  - te.width )/2.0;
	double delta_y = (height + te.height)/2.0;

	// Dessin du texte
	cr->begin_new_path();
	cr->move_to(delta_x, delta_y);
	cr->text_path(txt);
	cr->fill_preserve();
}

// Écrit un texte ne comportant une ligne principale + un texte additionnel
void DialWidget::draw_two_lines_text(Cairo::RefPtr<Cairo::Context> cr,
	const std::string &main_txt, const std::string &extra_txt)
{
	// Taille du widget
	Gtk::Allocation allocation = get_allocation();
	double width  = allocation.get_width();
	double height = allocation.get_height();

	// Patrons des textes à afficher
	std::string  main_txt_model = get_model_txt( main_txt);
	std::string extra_txt_model = get_model_txt(extra_txt);

	// Calcul de la position des textes
	Cairo::TextExtents te;
	double delta_x, delta_y;
	double  main_font_size = get_adjusted_font_height(cr, height*0.7 , width*0.7,  main_txt_model);
	double extra_font_size = get_adjusted_font_height(cr, height*0.05, width*0.6, extra_txt_model);

	// Dessin du texte additionnel
	cr->set_font_size(extra_font_size);
	cr->get_text_extents(extra_txt_model, te);
	delta_x = m_side==LEFT ? 20 : width - te.width - 20;
	delta_y = height - 20;
	cr->begin_new_path();
	cr->move_to(delta_x, delta_y);
	cr->text_path(extra_txt);
	cr->fill_preserve();

	// Dessin du texte principal
	cr->set_font_size(main_font_size);
	cr->get_text_extents(main_txt_model, te);
	delta_x = (width  - te.width )/2.0;
	delta_y = (height + te.height)/2.0;
	cr->begin_new_path();
	cr->move_to(delta_x, delta_y);
	cr->text_path(main_txt);
	cr->fill_preserve();
}

// Retourne la taille de police maximale utilisable pour écrire 'txt' dans un
// rectangle de taille 'allocable_height' x 'allocable_width'
double DialWidget::get_adjusted_font_height(Cairo::RefPtr<Cairo::Context> cr,
	int allocable_height, int allocable_width, const std::string &txt)
{
	Cairo::TextExtents te;
	cr->set_font_size(100.0);
	cr->get_text_extents(txt, te);
	double w_scale_factor = allocable_width /te.width ;
	double h_scale_factor = allocable_height/te.height;
	double scale_factor   = h_scale_factor<w_scale_factor ? h_scale_factor : w_scale_factor;
	return 100.0 * scale_factor;
}

// Formate correctement une durée sous la forme h:mm ou m.ss
// Rq : on suppose src >= 0
std::string DialWidget::format_time(int src) {
	assert(src>=0);

	// Conversion du temps exprimé en millisecondes en un temps en secondes
	int rounded_time = (src+499)/1000;

	// Si le temps restant est de moins d'une heure, on affiche m.ss
	if(rounded_time < 60*60) {
		int sec = rounded_time % 60;
		int min = rounded_time / 60;
		std::string txt_min = int_to_string(min);
		std::string txt_sec = int_to_string(sec);
		if(sec < 10) {
			txt_sec = "0" + txt_sec;
		}
		return txt_min + "." + txt_sec;
	}

	// Si le temps restant est de plus d'une heure, on affiche h:mm
	else {
		rounded_time = rounded_time / 60;
		int min  = rounded_time % 60;
		int hour = rounded_time / 60;
		std::string txt_min  = int_to_string(min );
		std::string txt_hour = int_to_string(hour);
		if(min < 10) {
			txt_min = "0" + txt_min;
		}
		return txt_hour + ":" + txt_min;
	}
}

// Remplace les chiffres présents dans 'src' par des '0'
std::string DialWidget::get_model_txt(const std::string &src) {
	std::string retval = src;
	for(unsigned int k=0; k<retval.size(); ++k) {
		if(retval[k]>='1' && retval[k]<='9')
			retval[k] = '0';
	}
	return retval;
}
