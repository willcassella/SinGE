// ArgAny.h
#pragma once

#include "../env.h"

namespace sge
{
	struct ArgAny
	{
		union Storage
		{
			bool b;
			int8 i8;
			uint8 u8;
			int16 i16;
			uint16 u16;
			int32 i32;
			uint32 u32;
			int64 i64;
			uint64 u64;
			float f;
			double d;
			const void* obj;
		};

		///////////////////
		///   Methods   ///
	public:

		template <typename T>
		const T& get() const
		{
			return *static_cast<const T*>(_value.obj);
		}

		template <typename T>
		void set(const T& obj)
		{
			_value.obj = &obj;
		}

		template <typename T>
		void set(const T&& obj) = delete;

		template <typename T>
		void set(T* ptr)
		{
			_value.obj = ptr;
		}

		template <typename T>
		void set(const T* ptr)
		{
			_value.obj = ptr;
		}

		void set(bool value)
		{
			_value.b = value;
		}

		void set(int8 value)
		{
			_value.i8 = value;
		}

		void set(uint8 value)
		{
			_value.u8 = value;
		}

		void set(int16 value)
		{
			_value.i16 = value;
		}

		void set(uint16 value)
		{
			_value.u16 = value;
		}

		void set(int32 value)
		{
			_value.i32 = value;
		}

		void set(uint32 value)
		{
			_value.u32 = value;
		}

		void set(int64 value)
		{
			_value.i64 = value;
		}

		void set(uint64 value)
		{
			_value.u64 = value;
		}

		void set(float value)
		{
			_value.f = value;
		}

		void set(double value)
		{
			_value.d = value;
		}

		//////////////////
		///   Fields   ///
	private:

		Storage _value;
	};

	template <>
	inline const bool& ArgAny::get() const
	{
		return _value.b;
	}

	template <>
	inline const int8& ArgAny::get() const
	{
		return _value.i8;
	}

	template <>
	inline const uint8& ArgAny::get() const
	{
		return _value.u8;
	}

	template <>
	inline const int16& ArgAny::get() const
	{
		return _value.i16;
	}

	template <>
	inline const uint16& ArgAny::get() const
	{
		return _value.u16;
	}

	template <>
	inline const int32& ArgAny::get() const
	{
		return _value.i32;
	}

	template <>
	inline const uint32& ArgAny::get() const
	{
		return _value.u32;
	}

	template <>
	inline const int64& ArgAny::get() const
	{
		return _value.i64;
	}

	template <>
	inline const uint64& ArgAny::get() const
	{
		return _value.u64;
	}

	template <>
	inline const float& ArgAny::get() const
	{
		return _value.f;
	}

	template <>
	inline const double& ArgAny::get() const
	{
		return _value.d;
	}
}
