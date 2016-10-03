// Any.h
#pragma once

#include "../env.h"

namespace sge
{
	struct TypeInfo;

	template <typename T>
	const auto& get_type(const T&);

	struct AnyMut
	{
		////////////////////////
		///   Constructors   ///
	public:

		AnyMut(void* value, const TypeInfo& type)
			: _value(value), _type(&type)
		{
		}

		template <typename T>
		AnyMut(T& value)
			: _value(&value), _type(&sge::get_type(value))
		{
		}

		template <typename T>
		AnyMut(T&& value) = delete;

		AnyMut(AnyMut& copy)
			: _value(copy._value), _type(copy._type)
		{
		}
		AnyMut(const AnyMut& copy)
			: _value(copy._value), _type(copy._type)
		{
		}
		AnyMut(AnyMut&& move)
			: _value(move._value), _type(move._type)
		{
		}
		AnyMut(const AnyMut&& move)
			: _value(move._value), _type(move._type)
		{
		}

		///////////////////
		///   Methods   ///
	public:

		void* get_value() const
		{
			return _value;
		}

		const TypeInfo& get_type() const
		{
			return *_type;
		}

		//////////////////
		///   Fields   ///
	private:

		void* _value;
		const TypeInfo* _type;
	};

	struct Any
	{
		////////////////////////
		///   Constructors   ///
	public:

		Any(const void* value, const TypeInfo& type)
			: _value(value), _type(&type)
		{
		}

		template <typename T>
		Any(const T& value)
			: _value(&value), _type(&sge::get_type(value))
		{
		}

		template <typename T>
		Any(T&& value) = delete;

		Any(AnyMut any)
			: _value(any.get_value()), _type(&any.get_type())
		{
		}

		Any(Any& copy)
			: _value(copy._value), _type(copy._type)
		{
		}
		Any(const Any& copy)
			: _value(copy._value), _type(copy._type)
		{
		}
		Any(Any&& move)
			: _value(move._value), _type(move._type)
		{
		}
		Any(const Any&& move)
			: _value(move._value), _type(move._type)
		{
		}

		///////////////////
		///   Methods   ///
	public:

		const void* get_value() const
		{
			return _value;
		}

		const TypeInfo& get_type() const
		{
			return *_type;
		}

		//////////////////
		///   Fields   ///
	private:

		const void* _value;
		const TypeInfo* _type;
	};

	struct ArgAny
	{
		union Storage
		{
			bool b;
			char i8;
			byte u8;
			int16 i16;
			uint16 u16;
			int32 i32;
			uint32 u32;
			int64 i64;
			uint64 u64;
			float f;
			double d;
			long double ld;
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

		void set(char value)
		{
			_value.i8 = value;
		}

		void set(byte value)
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

		void set(long double value)
		{
			_value.ld = value;
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
	inline const char& ArgAny::get() const
	{
		return _value.i8;
	}

	template <>
	inline const byte& ArgAny::get() const
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

	template <>
	inline const long double& ArgAny::get() const
	{
		return _value.ld;
	}

	namespace specialized
	{
		template <typename T>
		struct GetType;

		template<>
		struct GetType< Any >
		{
		};

		template <>
		struct GetType< AnyMut >
		{
		};
	}
}
