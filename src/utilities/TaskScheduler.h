/*
 *	This files declares a task scheduling (multitasking) class.
 *
 *	***************************************************************************
 *
 *	File: TaskScheduler.h
 *	Date: July 18, 2021
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
 *	Description:
 * 
 *		The TaskScheduler class executes a collection of Tasks (command 
 *		objects) at specified intervals. It's useful for prioritizing 
 *		CPU tasks, managing CPU time and allowing concurrent execution 
 *		(multitasking). 
 * 
 *		A Task object has an interval, command and state (Active/Idle).
 *		Each `tick()' of the TaskScheduler causes it to execute any active 
 *		tasks whose interval has expired, and this occurs repeatedly on each 
 *		call to the tick() method. Calling tick() from the main application
 *		loop() results in non-preemptive multitasking, while an interrupt 
 *		driven tick() will result in preemptive multitasking.
 *
 *		Task objects are instantiated and controlled by the application via 
 *		their member methods. The TaskScheduler does not modify any task 
 *		properties on its own.
 * 
 *	Notes:
 * 
 *		Preemptive multitasking requires re-entrant API functions. Check the 
 *		Arduino documentation for compatibility.
 * 
 *	**************************************************************************/

#if !defined __PG_TASKSCHEDULER_H
# define __PG_TASKSCHEDULER_H 20210718L 

# include "array"			// STL fixed-size array types.
# include "CommandTimer.h"	// CommandTimer type.

# if defined __PG_HAS_NAMESPACES 

namespace pg
{
	// Schedules tasks to run at specified intervals.
	class TaskScheduler
	{
	public:
		// Scheduled task type.
		class Task
		{
			friend class TaskScheduler;

		public:
			// Enumerates the valid task states.
			enum class State
			{
				Idle = 0,	// Indicates the task is not currently active.
				Active		// Indicates the task is currently active.
			};
			using timer_type = CommandTimer<std::chrono::milliseconds>;
			using duration = timer_type::duration;

		public:
			// Constructs an uninitialized task.
			Task() = default;
			// Constructs a task with a given interval, command and state.
			Task(duration, icommand*, State = State::Idle);
			// No copy constructor.
			Task(const Task&) = delete;
			// No copy assignment operator.
			Task& operator=(const Task&) = delete;

		public:
			// Sets the current task command.
			void command(icommand*);
			// Returns the current task command.
			const icommand* command() const;
			// Sets the current task interval.
			void interval(duration);
			// Returns the current task interval.
			const duration interval() const;
			// Sets the current task state.
			void state(State);
			// Returns the current task state.
			const State& state() const;
			
		private:
			timer_type	timer_; // Task timer and executor.
			State		state_;	// The current task state.
		};

	public:
		using container_type = typename std::ArrayWrapper<Task*>;

	public:
		// Constructs an uninitialized TaskScheduler.
		TaskScheduler() = default;
		// Constructs a TaskScheduler from an array of Tasks.
		template <size_t Size>
		explicit TaskScheduler(Task* (&)[Size]);
		// Constructs a TaskScheduler from pointer to and size of Tasks.
		TaskScheduler(Task* [], size_t);
		// Constructs a TaskScheduler from a range of Tasks.
		TaskScheduler(Task**, Task**);

	public:
		// Starts the TaskScheduler.
		void start();
		// Stops the TaskScheduler.
		void stop();
		// Sets the current tasks from a range (requires a restart).
		void tasks(Task**, Task**);
		// Checks for and executes any currently scheduled tasks.
		void tick();

	private:
		container_type tasks_;	// The current tasks collection.
	};

#pragma region TaskScheduler

	template <size_t Size>
	TaskScheduler::TaskScheduler(Task* (&tasks)[Size]) :
		tasks_(tasks)
	{

	}

	TaskScheduler::TaskScheduler(Task* tasks[], size_t n) :
		tasks_(tasks, n)
	{
		assert(n);
	}

	TaskScheduler::TaskScheduler(Task** first, Task** last) :
		tasks_(first, last)
	{
		assert(first && last);
	}

	void TaskScheduler::start()
	{
		for (auto i : tasks_)
			if (i->state_ == Task::State::Active)
				i->timer_.start();
	}

	void TaskScheduler::stop()
	{
		for (auto i : tasks_)
			i->timer_.stop();
	}

	void TaskScheduler::tick()
	{
		for (auto i : tasks_)
			if (i->state_ == Task::State::Active)
				i->timer_.tick();

	}

	void TaskScheduler::tasks(Task** first, Task** last)
	{
		// Calling this method requires a subsequent call to start().
		tasks_ = container_type(first, last);
	}

#pragma endregion
#pragma region Task

	TaskScheduler::Task::Task(duration interval, icommand* command, State state) :
		timer_(interval, command, true), state_(state) 
	{
		assert(command);
	}

	void TaskScheduler::Task::command(icommand* cmd)
	{
		assert(cmd);
		timer_.command(cmd);
	}

	const icommand* TaskScheduler::Task::command() const
	{
		return timer_.command();
	}

	void TaskScheduler::Task::interval(duration intvl)
	{
		timer_.interval(intvl);
	}

	const TaskScheduler::Task::duration TaskScheduler::Task::interval() const
	{
		return timer_.interval();
	}

	void TaskScheduler::Task::state(State val)
	{
		state_ = val;
	}

	const TaskScheduler::Task::State& TaskScheduler::Task::state(void) const
	{
		return state_;
	}

#pragma endregion
}

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_TASKSCHEDULER_H