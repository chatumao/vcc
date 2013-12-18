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


#ifndef KEYBOARDMAP_H_
#define KEYBOARDMAP_H_

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>
#include <boost/property_tree/ptree.hpp>


/**
 * Physical representation of a keyboard.
 */
class KeyboardMap
{
public:


	/**
	 * Physical description of a single key.
	 */
	class KeyDescriptor
	{
	public:

		/**
		 * Constructor.
		 */
		KeyDescriptor();

		/**
		 * Load the key-descriptor from a property tree.
		 * @returns `*this`
		 */
		KeyDescriptor &load(const boost::property_tree::ptree &data);

		/**
		 * ID of the key.
		 */
		const std::string &id() const { return _id; }

		/**
		 * Label of the key.
		 */
		const std::string &label() const { return _label; }

		/**
		 * Scan-code on the current platform.
		 */
		std::uint32_t scan_code() const
		{
#ifdef OS_IS_UNIX
			return _scan_code_unix;
#endif
#ifdef OS_IS_WINDOWS
			return _scan_code_windows;
#endif
		}

		/**
		 * Scan-code on a Unix platform.
		 */
		std::uint32_t scan_code_unix() const { return _scan_code_unix; }

		/**
		 * Scan-code on a Windows platform.
		 */
		std::uint32_t scan_code_windows() const { return _scan_code_windows; }

		/**
		 * Whether the given key lies in the numeric key-pad.
		 */
		bool in_numeric_keypad() const { return _in_numeric_keypad; }

		/**
		 * Index of the lowest key-line the key belongs to.
		 */
		std::size_t line_bottom() const { return _line_bottom; }

		/**
		 * Index of the highest key-line the key belongs to.
		 */
		std::size_t line_top() const { return _line_top; }

		/**
		 * Number of key-lines the key lies on.
		 */
		std::size_t line_extent() const { return _line_bottom-_line_top+1; }

		/**
		 * X-coordinate of the key in the given key-line.
		 */
		int x(std::size_t line) const { return _per_line_x[line-_line_top]; }

		/**
		 * Width of the key in the given key-line.
		 */
		int width(std::size_t line) const { return _per_line_width[line-_line_top]; }

	private:

		// Private members
		std::string      _id               ;
		std::string      _label            ;
		std::uint32_t    _scan_code_unix   ;
		std::uint32_t    _scan_code_windows;
		bool             _in_numeric_keypad;
		std::size_t      _line_bottom      ;
		std::size_t      _line_top         ;
		std::vector<int> _per_line_x       ;
		std::vector<int> _per_line_width   ;
	};


	/**
	 * Constructor.
	 */
	KeyboardMap();

	/**
	 * Load the keyboard map from a file.
	 * @returns `*this`
	 */
	KeyboardMap &load(const std::string &path);

	/**
	 * Load the keyboard map from a property tree.
	 * @returns `*this`
	 */
	KeyboardMap &load(const boost::property_tree::ptree &data);

	/**
	 * ID of the keyboard map.
	 */
	const std::string &id() const { return _id; }

	/**
	 * Human-readable name of the keyboard map (localized if possible, otherwise in English).
	 */
	const std::string &name() const { return _name; }

	/**
	 * Number of key-lines of the keyboard.
	 */
	std::size_t line_count() const { return _line_height.size(); }

	/**
	 * Y-coordinate of the given key-line.
	 */
	int line_y(std::size_t line) const { return _line_y[line]; }

	/**
	 * Height of the given key-line.
	 */
	int line_height(std::size_t line) const { return _line_height[line]; }

	/**
	 * Number of keys.
	 */
	std::size_t key_count() const { return _keys.size(); }

	/**
	 * Return the key descriptor corresponding to the given index.
	 */
	const KeyDescriptor &key(std::size_t idx) const { return _keys[idx]; }

	/**
	 * Total height of the keyboard.
	 */
	int total_height() const { return _total_height; }

	/**
	 * Total width of the keyboard, with of without the numeric keypad.
	 */
	int total_width(bool with_numeric_keypad) const { return with_numeric_keypad ? _total_width_with_nkp : _total_width_without_nkp; }

private:

	// Private functions
	static std::vector<int> parse_int_list(const std::string &data, std::size_t expected_count, int default_value);

	// Private members
	std::string                _id         ;
	std::string                _name       ;
	std::vector<int>           _line_y     ;
	std::vector<int>           _line_height;
	std::vector<KeyDescriptor> _keys       ;
	int _total_height           ;
	int _total_width_with_nkp   ;
	int _total_width_without_nkp;
};

#endif /* KEYBOARDMAP_H_ */
