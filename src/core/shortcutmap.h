/******************************************************************************
 *                                                                            *
 *    This file is part of Virtual Chess Clock, a chess clock software        *
 *                                                                            *
 *    Copyright (C) 2010-2014 Yoann Le Montagner <yo35(at)melix(dot)net>      *
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


#ifndef SHORTCUTMAP_H_
#define SHORTCUTMAP_H_

#include <map>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <wrappers/signals.h>


/**
 * Associate the keys of the keyboard (identified by a string ID) to an abstract
 * shortcut or function (identified by an integer).
 *
 * Two shortcuts may be associated to each key: one is said to be the "low-shortcut",
 * one is said to be the "high-shortcut". The shortcut or the action that is triggered
 * when the key is pressed depend on the state of modifier keys.
 */
class ShortcutMap
{
public:

	/**
	 * Constructor.
	 */
	ShortcutMap() {}

	/**
	 * Copy constructor.
	 */
	ShortcutMap(const ShortcutMap &rhs);

	/**
	 * Copy operator.
	 */
	ShortcutMap &operator=(const ShortcutMap &rhs);

	/**
	 * Signal sent when the shortcut map changes.
	 */
	sig::connection connect_changed(const sig::signal<void()>::slot_type &slot) const
	{
		return _signal_changed.connect(slot);
	}

	/**
	 * Return the index of the low-position shortcut associated to the key having the ID `id`.
	 * If this ID does not refer to a key, or if no shortcut is associated to it,
	 * 0 is returned.
	 */
	int shortcut_low(const std::string &id) const
	{
		auto it = _shortcut_low.find(id);
		return it==_shortcut_low.end() ? 0 : it->second;
	}

	/**
	 * Return the index of the high-position shortcut associated to the key having the ID `id`.
	 * If this ID does not refer to a key, or if no shortcut is associated to it,
	 * 0 is returned.
	 */
	int shortcut_high(const std::string &id) const
	{
		auto it = _shortcut_high.find(id);
		return it==_shortcut_high.end() ? 0 : it->second;
	}

	/**
	 * Return the index of the (either low- or high-position depending on the flag `high_position`)
	 * shortcut associated to the key having the ID `id`.
	 */
	int shortcut(const std::string &id, bool high_position) const
	{
		return high_position ? shortcut_high(id) : shortcut_low(id);
	}

	/**
	 * Set the index of the low-position shortcut associated to the key having the ID `id`.
	 */
	void set_shortcut_low(const std::string &id, int value);

	/**
	 * Set the index of the high-position shortcut associated to the key having the ID `id`.
	 */
	void set_shortcut_high(const std::string &id, int value);

	/**
	 * Set the index of the (either low- or high-position depending on the flag `high_position`)
	 * shortcut associated to the key having the ID `id`.
	 */
	void set_shortcut(const std::string &id, bool high_position, int value)
	{
		high_position ? set_shortcut_high(id, value) : set_shortcut_low(id, value);
	}

	/**
	 * Load the shortcut map from a file.
	 * @returns `*this`
	 */
	ShortcutMap &load(const std::string &path);

	/**
	 * Load the shortcut map from a property tree.
	 * @returns `*this`
	 */
	ShortcutMap &load(const boost::property_tree::ptree &data);

	/**
	 * Save the shortcut map to a file.
	 * @returns `*this`
	 */
	const ShortcutMap &save(const std::string &path) const;

	/**
	 * Save the shortcut map to a property tree.
	 * @returns `*this`
	 */
	const ShortcutMap &save(boost::property_tree::ptree &data) const;

private:

	// Private members
	mutable sig::signal<void()> _signal_changed;
	std::map<std::string, int>  _shortcut_low  ;
	std::map<std::string, int>  _shortcut_high ;
};

#endif /* SHORTCUTMAP_H_ */
