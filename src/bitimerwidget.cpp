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


#include "bitimerwidget.h"
#include <QPainter>
#include <QTimer>


// Constructor.
BiTimerWidget::BiTimerWidget(QWidget *parent) : QWidget(parent), _biTimer(nullptr),
	_showLabels(false),
	_displayTimeAfterTimeout(true), _displayBronsteinExtraInfo(true), _displayByoYomiExtraInfo(true)
{
	_timer = new QTimer(this);
	_timer->setInterval(100);
	connect(_timer, &QTimer::timeout, this, &BiTimerWidget::onTimeoutEvent);
	_timer->start();
}


// Throw an exception if no timer is binded to the widget.
void BiTimerWidget::ensureTimerBinded() const
{
	if(_biTimer==nullptr) {
		throw std::invalid_argument("No timer is currently binded to the widget.");
	}
}


// Bind a timer to the widget.
void BiTimerWidget::bindTimer(const BiTimer &biTimer)
{
	// If both the BiTimer object is unchanged, nothing to do.
	if(_biTimer==&biTimer) {
		return;
	}

	// Perform the suitable connection/disconnection operations.
	auto rawConnection = biTimer.connect_state_changed(boost::bind(&BiTimerWidget::onTimerStateChanged, this));
	_connection.reset(new sig::scoped_connection(rawConnection));
	_biTimer = &biTimer;

	// Refresh the widget.
	update();
}


// Un-bind the timer currently binded, if any.
void BiTimerWidget::unbindTimer()
{
	// Nothing to do if no timer is currently binded.
	if(_biTimer==nullptr) {
		return;
	}

	// Otherwise, disconnect the timer and refresh the widget.
	_connection.reset();
	_biTimer = nullptr;
	update();
}


// Set the left or right label.
void BiTimerWidget::setLabel(Side side, const QString &value)
{
	_label[side] = value;
	update();
}


// Set whether the labels are displayed or not.
void BiTimerWidget::setShowLabels(bool value)
{
	_showLabels = value;
	update();
}


// Set the minimal remaining time before seconds is displayed.
void BiTimerWidget::setDelayBeforeDisplaySeconds(const TimeDuration &value)
{
	_delayBeforeDisplaySeconds = value;
	update();
}


// Set whether the time should be displayed after timeout.
void BiTimerWidget::setDisplayTimeAfterTimeout(bool value)
{
	_displayTimeAfterTimeout = value;
	update();
}


// Set whether extra-information is displayed in Bronstein-mode.
void BiTimerWidget::setDisplayBronsteinExtraInfo(bool value)
{
	_displayBronsteinExtraInfo = value;
	update();
}


// Set whether extra-information is displayed in byo-yomi-mode.
void BiTimerWidget::setDisplayByoYomiExtraInfo(bool value)
{
	_displayByoYomiExtraInfo = value;
	update();
}


// Size hints.
QSize BiTimerWidget::minimumSizeHint() const { return QSize(500, 200); }
QSize BiTimerWidget::sizeHint       () const { return QSize(800, 300); }


// Widget rendering method.
void BiTimerWidget::paintEvent(QPaintEvent *)
{
	// Create the painter object.
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setPen(Qt::NoPen);

	// Special paint for the case where no timer is binded: draw a red cross.
	if(_biTimer==nullptr) {
		painter.setBrush(Qt::white);
		painter.drawRect(0, 0, width(), height());
		QPen crossPen(Qt::red);
		crossPen.setWidth(10);
		painter.setPen(crossPen);
		painter.drawLine(0, 0, width()-1, height()-1);
		painter.drawLine(0, height()-1, width()-1, 0);
		return;
	}

	// Define the left and right areas.
	Enum::array<Side, int> x, w;
	w[Side::LEFT ] = width()/2;
	w[Side::RIGHT] = width() - w[Side::LEFT];
	x[Side::LEFT ] = 0;
	x[Side::RIGHT] = w[Side::LEFT];

	// Background.
	for(auto it=Enum::cursor<Side>::first(); it.valid(); ++it) {
		painter.setBrush(_biTimer->active_side()==*it ? QColor(255,255,128) : Qt::white);
		painter.drawRect(x[*it], 0, w[*it], height());
	}

	// Current time
	TimeDuration currentTime = _biTimer->time(/* TODO _side */ Side::LEFT);
	long seconds = to_seconds(currentTime);

	// Draw the current time
	QFont font = painter.font();
	font.setPointSize(40);
	painter.setFont(font);
	painter.setPen(Qt::black);
	painter.drawText(0, 0, width(), height(), Qt::AlignCenter, QString::number(seconds));
}


// Handler for the timer state-change event.
void BiTimerWidget::onTimerStateChanged()
{
	update();
}


// Handler periodically called by the internal QTimer object.
void BiTimerWidget::onTimeoutEvent()
{
	if(_biTimer==nullptr || !_biTimer->is_active()) {
		return;
	}
	update();
}
