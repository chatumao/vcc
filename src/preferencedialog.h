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


#ifndef PREFERENCEDIALOG_H_
#define PREFERENCEDIALOG_H_

#include <QDialog>
#include <QTabWidget>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>
#include <QPushButton>
#include <core/options.h>
#include <gui/core/keyboardhandler.h>
#include <gui/widgets/captionwidget.h>
#include <gui/widgets/modifierkeyswidget.h>
#include <gui/widgets/keyboardwidget.h>
#include <gui/widgets/timedurationwidget.h>


/**
 * Dialog to edit user's preferences.
 */
class PreferenceDialog : public QDialog
{
	Q_OBJECT

public:

	/**
	 * Constructor.
	 */
	PreferenceDialog(QWidget *parent=0);

	/**
	 * Load the dialog with the parameters saved in the Params singleton object.
	 */
	void loadParameters();

	/**
	 * Save the new parameters defined from the dialog in the Params singleton object.
	 */
	void saveParameters();

protected:

	/**
	 * Window state-change handler.
	 */
	void changeEvent(QEvent *event) override;

private:

	// Private functions
	QWidget *createKeyboardPage     ();
	QWidget *createTimeDisplayPage  ();
	QWidget *createMiscellaneousPage();
	CaptionWidget *captionWidgetFactory(const QColor &color, const QString &label, int shortcut);
	void refreshKeyboardHandlerActivationState();
	void feedKeyboardSelector();
	std::string retrieveSelectedKeyboard() const;
	void onSelectedKeyboardChanged();
	void onHasNumericKeypadToggled();
	void onModifierKeysChanged    ();
	void onShortcutModeChanged    ();
	void onCaptionToggled(CaptionWidget *widget, int shortcut);
	void onKeyClicked(const std::string &id, Qt::MouseButton button);

	// Tab widget
	QTabWidget *_tabWidget;

	// Keyboard page
	QComboBox          *_keyboardSelector    ;
	QCheckBox          *_hasNumericKeypad    ;
	KeyboardHandler    *_keyboardHandler     ;
	KeyboardWidget     *_keyboardWidget      ;
	ShortcutMap         _shortcutMap         ;
	bool                _captionToggledShunt ;
	CaptionWidget      *_selectedCaption     ;
	int                 _selectedShortcut    ;
	ModifierKeysWidget *_modifierKeysSelector;
	QPushButton        *_shortcutModeSelector;

	// Time display page
	TimeDurationWidget *_delayBeforeDisplaySeconds;
	QCheckBox          *_displayTimeAfterTimeout  ;
	QCheckBox          *_displayBronsteinExtraInfo;
	QCheckBox          *_displayByoYomiExtraInfo  ;

	// Miscellaneous page
	QCheckBox                                     *_showStatusBar    ;
	Enum::array<ResetConfirmation, QRadioButton *> _resetConfirmation;
};

#endif /* PREFERENCEDIALOG_H_ */
