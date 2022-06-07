/*
 *	This file defines several types for implementing the C++ "Command" design 
 *	pattern. 
 *
 *	***************************************************************************
 *
 *	File: icommand.h
 *	Date: May 24, 2022
 *	Version: 1.0
 *	Author: Michael Brodsky
 *	Email: mbrodskiis@gmail.com
 *	Copyright (c) 2012-2022 Michael Brodsky 
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
 *	**************************************************************************/

#if !defined __PG_ICOMMAND_H
# define __PG_ICOMMAND_H 20220524L

# include <tuple>			// Needed by objects in namespace experimental.
# include <lib/callback.h>	// `Callback' templates.

# if defined __PG_HAS_NAMESPACES

namespace pg
{
	// Command design pattern abstract interface class.
	struct icommand
	{
		virtual ~icommand() = default;

		virtual void execute() = 0;
	};

	// Constant indicating no command specified.
	const nullptr_t NoCommand = nullptr;

	// Null command type.
	struct NullCommand : public icommand
	{
		NullCommand() = default;
		~NullCommand() = default;

		void execute() override {}
	};

	// Concrete Command type for member functions with arguments.
	template <class Ret, class Obj = void, class Arg = void>
	class Command : public icommand
	{
	public:
		using return_type = Ret;
		using object_type = Obj;
		using delegate_type = typename pg::callback<Ret, Obj, Arg>::type;
		using arg_type = Arg;

	public:
		Command(object_type* object = nullptr, delegate_type delegate = nullptr, arg_type arg = arg_type()) :
			icommand(), arg_(arg), object_(object), delegate_(delegate) {}
		~Command() {};

	public:
		// Command execute method.
		void execute() override { (void)(object_->*delegate_)(arg_); }
		object_type*& object() { return object_; }
		const object_type*& object() const { return object_; }
		delegate_type& delegate() { return delegate_; }
		const delegate_type& delegate() const { return delegate_; }
		arg_type& arg() { return arg_; }
		const arg_type& arg() const { return arg_; }

	protected:
		object_type*	object_;	// Command receiver.
		delegate_type	delegate_;	// Receiver::method.
		Arg				arg_;		// Method argument.
	};

	// Concrete Command type for free-standing functions with arguments.
	template <class Ret, class Arg>
	class Command<Ret, void, Arg> : public icommand
	{
	public:
		// Callback type signature.
		using callback_type = typename pg::callback<Ret, void, Arg>::type;

	public:
		Command(callback_type callback, Arg arg) :
			icommand(), arg_(arg), callback_(callback) {}
		~Command() {}

	public:
		// Command execute method.
		void execute() override { (void)(*callback_)(arg_); }

	protected:
		callback_type	callback_;	// Command callback.
		Arg				arg_;		// Callback argument.
	};

	// Concrete Command type for member functions without arguments.
	template <class Ret, class Obj>
	class Command<Ret, Obj, void> : public icommand
	{
	public:
		// Callback type signature.
		using callback_type = typename pg::callback<Ret, Obj, void>::type;

	public:
		Command(Obj* object, callback_type callback) :
			icommand(), receiver_(object), callback_(callback) {}
		~Command() {}

	public:
		// Command execute method.
		void execute() override { (void)(receiver_->*callback_)(); }

	protected:
		Obj*			receiver_;	// Command receiver.
		callback_type	callback_;	// Receiver::method.
	};

	// Concrete Command type for free-standing functions without arguments.
	template <class Ret>
	class Command<Ret, void, void> : public icommand
	{
	public:
		// Callback type signature.
		using callback_type = typename pg::callback<Ret, void, void>::type;

	public:
		Command(callback_type callback) :
			icommand(), callback_(callback) {}
		~Command() {}

	public:
		// Command execute method.
		void execute() override { (void)(*callback_)(); }

	protected:
		callback_type callback_;	// Command callback.
	};

	namespace experimental
	{
		// These experimental command types support variadic arguments and 
		// should be able to call any function/method. Arguments are stored 
		// as tuples and forwarded to the callee using std::apply.

		template<class... Ts>
		class ICommand : public icommand
		{
		public:
			using args_type = std::tuple<Ts...>;
		public:
			explicit ICommand(const args_type& args) : args_(args) {}
			ICommand(Ts... args) : args_(args...) {}
		public:
			args_type& args() { return args_; }
		protected:
			args_type args_;
		};

		struct NullCommand : public icommand
		{
			using object_type = void;
			using delegate_type = typename callback<void>::type;
			using args_type = void;
			using return_type = void;

			void execute() override {}
		};

		template <class Ret, class Obj = void, class... Args>
		class Command : public experimental::ICommand<Args...>
		{
		public:
			using base_type = experimental::ICommand<Args...>;
			using object_type = Obj;
			using delegate_type = typename callback<Ret, Obj, Args...>::type;
			using args_type = typename base_type::args_type;
			using return_type = Ret;
		public:
			Command(object_type& obj, delegate_type del) : 
				base_type(args_type()), obj_(obj), del_(del) {}
			Command(object_type& obj, delegate_type del, Args...args) :
				base_type(args...), obj_(obj), del_(del) {}
			Command(object_type& obj, delegate_type del, const args_type& args) :
				base_type(args), obj_(obj), del_(del) {}
		public:
			void execute() override { std::experimental::apply(&obj_, del_, base_type::args_); }
		private:
			object_type&	obj_;
			delegate_type	del_;
		};

		template <class Ret, class...Args>
		class Command<Ret, void, Args...> : public experimental::ICommand<Args...>
		{
		public:
			using base_type = experimental::ICommand<Args...>;
			using object_type = void;
			using delegate_type = typename callback<Ret, void, Args...>::type;
			using args_type = typename base_type::args_type;
			using return_type = Ret;
		public:
			Command(delegate_type del) :
				base_type(args_type()), del_(del) {}
			Command(delegate_type del, Args...args) :
				base_type(args...), del_(del) {}
			Command(delegate_type del, const args_type& args) :
				base_type(args), del_(del) {}
		public:
			void execute() override { std::experimental::apply(del_, base_type::args_); }
		private:
			delegate_type	del_;
		};

		template <class Ret, class Obj>
		class Command<Ret, Obj, void> : public experimental::ICommand<> 
		{
		public:
			using base_type = experimental::ICommand<>;
			using object_type = Obj;
			using delegate_type = typename callback<Ret, Obj>::type;
			using args_type = typename base_type::args_type;
			using return_type = Ret;
		public:
			Command(object_type& obj, delegate_type del) :
				obj_(obj), del_(del) {}
		public:
			void execute() override { (obj_.*del_)(); }
		private:
			object_type&	obj_;
			delegate_type	del_;
		};

		template <class Ret>
		class Command<Ret, void, void> : public experimental::ICommand<>
		{
		public:
			using base_type = experimental::ICommand<>;
			using object_type = void;
			using delegate_type = typename callback<Ret>::type;
			using args_type = typename base_type::args_type;
			using return_type = Ret;
		public:
			Command(delegate_type del) :
				del_(del) {}
		public:
			void execute() override { (*del_)(); }
		private:
			delegate_type	del_;
		};
	} // namespace experimental

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES

#endif // !defined __PG_ICOMMAND_H
