/*
 *	This files defines a class that configures and asynchronously manages an 
 *	LCD display device.
 *
 *	***************************************************************************
 *
 *	File: LCDDisplay.h
 *	Date: September 30, 2021
 *	Version: 1.0
 *	Author: Michael Brodsky
 *	Email: mbrodskiis@gmail.com
 *	Copyright (c) 2012-2021 Michael Brodsky
 *
 *	***************************************************************************
 *
 *  This file is part of "Pretty Good" (Pg). "Pg" is free software:
 *	you can redistribute it and/or modify it under the terms of the
 *	GNU General Public License as published by the Free Software Foundation,
 *	either version 3 of the License, or (at your option) any later version.
 *
 *  This file is distributed in the hope that it will be useful, but
 *	WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *	along with this file. If not, see <http://www.gnu.org/licenses/>.
 *
 *	**************************************************************************
 *
 *	Description:
 *
 *		The `LCDDisplay' allows rapid configuration and simple management of 
 *		an lcd display device. It supports formatted printing and synchronous 
 *		or asynchronous operation. The display hardware is controlled via the 
 *		Arduino `LiquidCrystal' api.
 *
 *	**************************************************************************/

#if !defined __PG_LCDDISPLAY_H
# define __PG_LCDDISPLAY_H 20210928L

# include "cstdio"
# include "cstring"
# include "cstdlib"
# include "array"
# include "system/boards.h"
# include "interfaces/icomponent.h"	// `icomponent' interface.
# include "interfaces/iclockable.h"
# include "utilities/Timer.h"
# include <LiquidCrystal.h>

# if defined __PG_HAS_NAMESPACES

namespace pg
{
	// Asynchronous LCD display manager.
	template<uint8_t Cols, uint8_t Rows>
	class LCDDisplay : public iclockable, public icomponent 
	{
	public:
		// Aggregates properties about a display field.
		struct Field
		{
			uint8_t		col_;		// column index.
			uint8_t		row_;		// row index.
			const char* label_;		// label string.
			const char* fmt_;		// format string.
			bool		visible_;	// is visible.

			friend bool operator==(const Field& lhs, const Field& rhs) 
			{ return lhs.col_ == rhs.col_ && lhs.row_ == rhs.row_; }
			friend bool operator!=(const Field& lhs, const Field& rhs) 
			{ return !(lhs == rhs); }
			friend bool operator<(const Field& lhs, const Field& rhs) 
			{ return lhs.row_ < rhs.row_ || (lhs.row_ == rhs.row_ && lhs.col_ < rhs.col_); }
			friend bool operator>(const Field& lhs, const Field& rhs) 
			{ return (rhs < lhs); }
			friend bool operator<=(const Field& lhs, const Field& rhs) 
			{ !(rhs < lhs); }
			friend bool operator>=(const Field& lhs, const Field& rhs) 
			{ return !(lhs < rhs); }
		};

		// Encapsulates a collection of display fields.
		class Screen
		{
		public:
			using container_type = typename std::ArrayWrapper<Field*>;
			using iterator = typename container_type::iterator;
			using const_iterator = typename container_type::const_iterator;

		public:
			// Constructs an empty screen.
			Screen() = default;
			// Constructs a screen from an array of fields and label.
			template<std::size_t N>
			explicit Screen(Field* (&)[N], const char* = nullptr);
			// Constructs a screen from an field pointer and size, and label.
			Screen(Field* [], std::size_t, const char* = nullptr);
			// Constructs a screen from a range of fields and label.
			explicit Screen(Field**, Field**, const char* = nullptr);
			// Constructs a screen from a list of fields.
			explicit Screen(std::initializer_list<Field*>, const char* = nullptr);
			// Constructs a screen from a container of fields.			
			explicit Screen(const container_type&, const char* = nullptr);

		public:
			// Sets the screen fields collection from an array.
			template<std::size_t N>
			void fields(Field* (&)[N]);
			// Sets the screen fields collection from a pointer and size.
			void fields(Field* [], std::size_t);
			// Sets the screen fields collection from a range.
			void fields(Field**, Field**);
			// Sets the screen fields collection from a list.
			void fields(std::initializer_list<Field*>);
			// Sets the screen fields collection from a container.
			void fields(container_type&);
			// Returns the current fields collection as a container.
			const container_type& fields() const;
			// Sets the screen label.
			void label(const char*);
			// Returns the current screen label.
			const char*	label() const;
			// Sets the active field.
			void active_field(Field*);
			// Returns the currently active field.
			const Field* active_field() const;
			// Advances to the next field in the collection.
			const Field* next();
			// Advances to the previous field in the collection.
			const Field* prev();

		public:
			// Checks whether two screens are the same.
			friend bool operator==(const Screen& lhs, const Screen& rhs) { return &rhs == &lhs; }

		private:
			const char*		label_;		// Display label.
			container_type	fields_;	// Current fields collection.
			iterator		current_;	// Currently active field.
		};

		// Enumerates the valid display cursors.
		enum class Cursor
		{
			None = 0,	// No cursor displayed.
			Underline,	// Underline cursor displayed.
			Block		// Block cursor displayed.
		};

		// Enumerates the valid display modes.
		enum class Mode
		{
			Normal = 0, // Normal display mode.
			Edit		// Editting mode.
		};

		using timer_type = Timer<std::chrono::milliseconds>;
		using duration = typename timer_type::duration;
		using callback_type = pg::callback<void>::type;
		static constexpr uint8_t cols() { return Cols; }
		static constexpr uint8_t rows() { return Rows; }
		using iterator = typename Screen::const_iterator;

	private:
		// Enumerates valid update events.
		enum class Update : uint8_t
		{
			None = 0x00,	// No event.
			Cursor = 0x01,	// Cursor change event.
			Field = 0x02,	// Field change event.
			Display = 0x04,	// Display/noDisplay event.
			Print = 0x08,	// Print to display event.
			Clear = 0x10	// Clear display event.
		};

		// Type that encapsulates update events encoded as a bitfield.
		class Event
		{
		public:
			explicit Event(Update u = Update::None) : val_(static_cast<uint8_t>(u)) {}

		public:
			// Sets the specified event in the bitfield.
			void set(Update u) { val_ |= static_cast<uint8_t>(u); }
			// Clears the specified event in the bitfield.
			void clr(Update u) { val_ &= ~(static_cast<uint8_t>(u)); }
			// Checks whether the specified event is currently set.
			uint8_t operator==(Update u) { return val_ & static_cast<uint8_t>(u); }
			// Assigns the specified event, overwriting the previous value in the bitfield.
			void operator=(Update u) { val_ = static_cast<uint8_t>(u); }

		private:
			uint8_t val_;	// The current update events, encoded as a bitfield.
		};

	public:
		// Constructs an LCDDisplay.
		explicit LCDDisplay(LiquidCrystal*, callback_type = nullptr, Screen* = nullptr);
		// Move constructor.
		LCDDisplay(LCDDisplay&&) = default;
		// No copy constructor.
		LCDDisplay(const LCDDisplay&) = delete;
		// No copy assignment operator.
		LCDDisplay& operator=(const LCDDisplay&) = delete;

	public:
		// Refreshes the lcd display device.
		template<class ...Ts>
		void			refresh(Ts&& ...args);
		// Sets the currently active screen.
		void			screen(Screen*);
		// Returns the currently active screen.
		const Screen*	screen() const;
		// Sets the display cursor.
		void			cursor(Cursor);
		// Returns the current display cursor.
		Cursor			cursor() const;
		// Sets the current display/nodisplay device setting.
		void			show(bool);
		// Returns the current display/nodisplay device setting.
		bool			show() const;
		// Clear the display device.
		void			clear();
		// Sets the display device blink interval.
		void			blink(duration = duration());
		// Returns the current display device blink interval.
		duration		blink();
		// Indicates the display device needs to be updated.
		void			update();
		// Sets the display mode.
		void			mode(Mode);
		// Returns the current display mode.
		Mode			mode() const;

		void			next();

		void			prev();

	private:
		// Calls the client callback function.
		void clock() override;
		// Writes one formatted field and value to the display device.
		template<class T>
		void write_all(char* buf, iterator it, T&& arg);
		// Writes two or more formatted fields and values to the display device.
		template<class T, class ...Ts>
		void write_all(char* buf, iterator it, T&& arg, Ts&& ...args);
		// Primary template for writing a formatted value.
		template<class T>
		void write_value(char* buf, const Field* it, const T& arg);
		// Template specialization for writing a formatted float value.
		void write_value(char* buf, const Field* it, const float& arg);
		// Template specialization for writing a formatted double value.
		void write_value(char* buf, const Field* it, const double& arg);
		// Template specialization for writing a formatted long double value.
		void write_value(char* buf, const Field* it, const long double& arg);
		// Writes a field label to the display device.
		inline void write_label(const Field* it);
		// Sets the update cursor event.
		void set_cursor_event();
		// Sets the update display values event.
		void set_update_event();
		// Sets the update field event.
		void set_field_event();
		// Sets the update display visibility event.
		void set_display_event();
		// Sets the clear display event.
		void set_clear_event();

	private:
		LiquidCrystal*	lcd_;		// LCD display device
		Screen*			screen_;	// Current `Screen' object.
		Cursor			cursor_;	// Current display cursor setting.
		bool			display_;	// Current display/nodisplay setting.
		Mode			mode_;		// Current operating mode setting.
		Event			event_;		// Currently pending update events.
		timer_type		timer_;		// Display blink timer.
		callback_type	callback_;	// Client callback.
	};

#pragma region screen

	template<uint8_t Cols, uint8_t Rows>
	template<std::size_t N>
	LCDDisplay<Cols,Rows>::Screen::Screen(Field* (&fields)[N], const char* label)
		: fields_(fields), label_(label), current_(std::begin(fields_)) 
	{}

	template<uint8_t Cols, uint8_t Rows>
	LCDDisplay<Cols, Rows>::Screen::Screen(Field* fields[], std::size_t n, const char* label) :
		fields_(fields, n), label_(label), current_(std::begin(fields_))
	{}

	template<uint8_t Cols, uint8_t Rows>
	LCDDisplay<Cols, Rows>::Screen::Screen(Field** first, Field** last, const char* label) :
		fields_(first, last), label_(label), current_(std::begin(fields_))
	{}

	template<uint8_t Cols, uint8_t Rows>
	LCDDisplay<Cols, Rows>::Screen::Screen(std::initializer_list<Field*> il, const char* label) :
		fields_(const_cast<Field**>(il.begin()), il.size()), label_(label), current_(std::begin(fields_))
	{}

	template<uint8_t Cols, uint8_t Rows>
	LCDDisplay<Cols, Rows>::Screen::Screen(const container_type& fields, const char* label) : 
		fields_(fields), label_(label), current_(std::begin(fields_))
	{

	}

	template<uint8_t Cols, uint8_t Rows>
	template<std::size_t N>
	void LCDDisplay<Cols, Rows>::Screen::fields(Field* (&fields)[N])
	{
		fields_ = container_type(fields);
		current_ = std::begin(fields_);
	}

	template<uint8_t Cols, uint8_t Rows>
	void LCDDisplay<Cols, Rows>::Screen::fields(Field* fields[], std::size_t n)
	{
		fields_ = container_type(fields, n);
		current_ = std::begin(fields_);
	}

	template<uint8_t Cols, uint8_t Rows>
	void LCDDisplay<Cols, Rows>::Screen::fields(Field** first, Field** last)
	{ 
		fields_ = container_type(first, last); 
		current_ = std::begin(fields_);
	}

	template<uint8_t Cols, uint8_t Rows>
	void LCDDisplay<Cols, Rows>::Screen::fields(std::initializer_list<Field*> il)
	{
		fields_ = container_type(const_cast<Field**>(il.begin()), il.size());
		current_ = std::begin(fields_);
	}

	template<uint8_t Cols, uint8_t Rows>
	void LCDDisplay<Cols, Rows>::Screen::fields(container_type& fields)
	{
		fields_ = fields;
	}

	template<uint8_t Cols, uint8_t Rows>
	const typename LCDDisplay<Cols, Rows>::Screen::container_type&
		LCDDisplay<Cols, Rows>::Screen::fields() const
	{ 
		return fields_; 
	}

	template<uint8_t Cols, uint8_t Rows>
	void LCDDisplay<Cols, Rows>::Screen::label(const char* lab)
	{ 
		label_ = lab; 
	}

	template<uint8_t Cols, uint8_t Rows>
	const char* LCDDisplay<Cols, Rows>::Screen::label() const
	{ 
		return label_; 
	}

	template<uint8_t Cols, uint8_t Rows>
	void LCDDisplay<Cols, Rows>::Screen::active_field(Field* field)
	{
		current_ = std::find(fields_.begin(), fields_.end(), field);
	}

	template<uint8_t Cols, uint8_t Rows>
	const typename LCDDisplay<Cols, Rows>::Field*
		LCDDisplay<Cols, Rows>::Screen::active_field() const
	{
		return *current_;
	}

	template<uint8_t Cols, uint8_t Rows>
	const typename LCDDisplay<Cols, Rows>::Field* LCDDisplay<Cols, Rows>::Screen::next()
	{
		if (++current_ == std::end(fields_))
			current_ = std::begin(fields_);

		return *current_;
	}

	template<uint8_t Cols, uint8_t Rows>
	const typename LCDDisplay<Cols, Rows>::Field* LCDDisplay<Cols, Rows>::Screen::prev()
	{
		if (current_ == std::begin(fields_))
			current_ = std::end(fields_);

		return *--current_;
	}

#pragma endregion
#pragma region display 

	template<uint8_t Cols, uint8_t Rows>
	LCDDisplay<Cols, Rows>::LCDDisplay(LiquidCrystal* lcd, callback_type cb, Screen* screen) :
		lcd_(lcd), callback_(cb), screen_(screen), cursor_(), display_(true), mode_(), event_(), timer_() 
	{}

	template<uint8_t Cols, uint8_t Rows>
	template<class ...Ts>
	void LCDDisplay<Cols, Rows>::refresh(Ts&& ...args)
	{
		// Device is refreshed all at once with any pending updates.

		if (mode_ == Mode::Normal)
			event_.set(Update::Print);	// Always refresh display in Normal mode.
		else
			set_field_event();
		if (timer_.expired())			// Handle blinking display and timer.
		{
			display_ = !display_;
			set_display_event();
			timer_.reset();
		}
		if (event_ == Update::Display)	// Set display/noDisplay.
			display_ ? lcd_->display() : lcd_->noDisplay();
		if (event_ == Update::Clear)	// Clear display.
			lcd_->clear();
		if (event_ == Update::Cursor)	// Set display cursor.
		{
			switch (cursor_)
			{
			case Cursor::None:
				lcd_->noCursor();
				lcd_->noBlink();
				break;
			case Cursor::Block:
				lcd_->noCursor();
				lcd_->blink();
				break;
			case Cursor::Underline:
				lcd_->cursor();
				lcd_->noBlink();
				break;
			default:
				break;
			}
		}
		if (event_ == Update::Print)	// Update display fields and values.
		{
			char buf[Cols * Rows + 1] = { '\0' };

			lcd_->home();
			lcd_->print(screen_->label());
			iterator it = std::begin(screen_->fields());
			write_all(buf, it, std::forward<Ts>(args)...);
		}
		if (event_ == Update::Field)	// Position cursor at current field.
			lcd_->setCursor(screen_->active_field()->col_, screen_->active_field()->row_);
		event_ = Update::None;			// Reset the Update Event flags.
	}

	template<uint8_t Cols, uint8_t Rows>
	void LCDDisplay<Cols, Rows>::screen(Screen* scr)
	{
		set_update_event();
		scr->active_field(*std::begin(scr->fields())); // bloated
		screen_ = scr;
	}

	template<uint8_t Cols, uint8_t Rows>
	const typename LCDDisplay<Cols, Rows>::Screen* 
		LCDDisplay<Cols, Rows>::screen() const
	{
		return screen_;
	}

	template<uint8_t Cols, uint8_t Rows>
	void LCDDisplay<Cols, Rows>::cursor(Cursor crsr)
	{
		cursor_ = crsr;
		set_cursor_event();
	}

	template<uint8_t Cols, uint8_t Rows>
	typename LCDDisplay<Cols, Rows>::Cursor LCDDisplay<Cols, Rows>::cursor() const
	{
		return cursor_;
	}

	template<uint8_t Cols, uint8_t Rows>
	void LCDDisplay<Cols, Rows>::show(bool value)
	{
		display_ = value;
		set_display_event();
	}

	template<uint8_t Cols, uint8_t Rows>
	bool LCDDisplay<Cols, Rows>::show() const
	{
		return display_;
	}

	template<uint8_t Cols, uint8_t Rows>
	void LCDDisplay<Cols, Rows>::clear()
	{
		set_clear_event();
	}

	template<uint8_t Cols, uint8_t Rows>
	void LCDDisplay<Cols, Rows>::blink(duration interval)
	{
		if (interval.count() != 0)
			timer_.start(interval);	// Blink display on interval != 0, ...
		else
		{
			timer_.stop();			// ... otherwise stop blinking.
			set_display_event();
		}
	}

	template<uint8_t Cols, uint8_t Rows>
	typename LCDDisplay<Cols, Rows>::duration LCDDisplay<Cols, Rows>::blink()
	{
		return timer_.interval();
	}

	template<uint8_t Cols, uint8_t Rows>
	void LCDDisplay<Cols, Rows>::update()
	{
		set_update_event();
	}

	template<uint8_t Cols, uint8_t Rows>
	void LCDDisplay<Cols, Rows>::mode(Mode m)
	{
		mode_ = m;
	}

	template<uint8_t Cols, uint8_t Rows>
	typename LCDDisplay<Cols, Rows>::Mode LCDDisplay<Cols, Rows>::mode() const
	{
		return mode_;
	}

	template<uint8_t Cols, uint8_t Rows>
	void LCDDisplay<Cols, Rows>::next()
	{
		screen_->next();
		set_field_event();
	}

	template<uint8_t Cols, uint8_t Rows>
	void LCDDisplay<Cols, Rows>::prev()
	{
		screen_->prev();
		set_field_event();
	}

	template<uint8_t Cols, uint8_t Rows>
	void LCDDisplay<Cols, Rows>::clock() 
	{
		if (callback_)
			(*callback_)(); // Client provides values in callback func.
	}

	template<uint8_t Cols, uint8_t Rows>
	template<class T>
	void LCDDisplay<Cols, Rows>::write_all(char* buf, iterator it, T&& arg)
	{
		if ((*it)->visible_)
		{
			write_label(*it); // Write field label ...
			write_value(buf, *it, static_cast<const T&>(arg)); // ... and value.
		}
	}

	template<uint8_t Cols, uint8_t Rows>
	template<class T, class ...Ts>
	void LCDDisplay<Cols, Rows>::write_all(char* buf, iterator it, T&& arg, Ts&& ...args)
	{
		if ((*it)->visible_)
		{
			write_label(*it);	// Write field label, ...
			write_value(buf, *it, static_cast<const T&>(arg)); // ... value ...
		}
		write_all(buf, ++it, std::forward<Ts>(args)...); // ... and the rest.
	}

	template<uint8_t Cols, uint8_t Rows>
	template<class T>
	void LCDDisplay<Cols, Rows>::write_value(char* buf, const Field* it, const T& arg)
	{
		(void)sprintf(buf, it->fmt_, arg); // Print formatted value to buf.
		lcd_->print(buf);
	}

	template<uint8_t Cols, uint8_t Rows>
	void LCDDisplay<Cols, Rows>::write_value(char* buf, const Field* it, const float& arg)
	{
		write_value(buf, it, static_cast<double>(arg));
	}

	template<uint8_t Cols, uint8_t Rows>
	void LCDDisplay<Cols, Rows>::write_value(char* buf, const Field* it, const double& arg)
	{
		// Arduin cstdlib sprintf() doesn't support printing of floating point types, 
		// so we have to use dtostrf().

		char str[11]; // Formatted print buf for float types.
		uint8_t w = std::atoi(std::strchr(it->fmt_, '%') + 1);	// Parse width from fmt spec.
		uint8_t p = std::atoi(std::strchr(it->fmt_, '.') + 1);	// Parse precision from fmt spec.

		(void)sprintf(buf, "%s", dtostrf(arg, w, p, str));		// Print formatted value to buf.
		lcd_->print(buf);
	}

	template<uint8_t Cols, uint8_t Rows>
	void LCDDisplay<Cols, Rows>::write_value(char* buf, const Field* it, const long double& arg)
	{
		write_value(buf, it, static_cast<double>(arg));
	}

	template<uint8_t Cols, uint8_t Rows>
	void LCDDisplay<Cols, Rows>::write_label(const Field* it)
	{
		lcd_->setCursor(it->col_, it->row_);	// Set cursor at field coords.
		lcd_->print(it->label_);				// Print field label.
		lcd_->setCursor(it->col_ + strlen(it->label_), it->row_);	// Set cursor after label for value.
	}

	template<uint8_t Cols, uint8_t Rows>
	void LCDDisplay<Cols, Rows>::set_cursor_event()
	{
		event_.set(Update::Cursor);
	}

	template<uint8_t Cols, uint8_t Rows>
	void LCDDisplay<Cols, Rows>::set_update_event()
	{
		event_.set(Update::Print);
		set_field_event();
	}

	template<uint8_t Cols, uint8_t Rows>
	void LCDDisplay<Cols, Rows>::set_field_event()
	{
		event_.set(Update::Field);
	}

	template<uint8_t Cols, uint8_t Rows>
	void LCDDisplay<Cols, Rows>::set_display_event()
	{
		if (!display_)
			event_ = Update::None; // Clear all other events when blanking the display.
		event_.set(Update::Display);
	}

	template<uint8_t Cols, uint8_t Rows>
	void LCDDisplay<Cols, Rows>::set_clear_event()
	{
		event_.set(Update::Clear);
	}

#pragma endregion
} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES

#endif
