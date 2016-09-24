// Any.h
#pragma once

namespace sge
{
	struct TypeInfo;

	template <typename T>
	const TypeInfo& get_type(const T&);

	struct AnyMut
	{
		////////////////////////
		///   Constructors   ///
	public:

		template <typename T>
		AnyMut(T& value)
		{
			_value = &value;
			_type = &sge::get_type(value);
		}

		template <typename T>
		AnyMut(T&& value) = delete;

		AnyMut(AnyMut& copy)
			: _value{ copy._value }, _type{ copy._type }
		{
		}
		AnyMut(const AnyMut& copy)
			: _value{ copy._value }, _type{ copy._type }
		{
		}
		AnyMut(AnyMut&& move)
			: _value{ move._value }, _type{ move._type }
		{
		}
		AnyMut(const AnyMut&& move)
			: _value{ move._value }, _type{ move._type }
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

		template <typename T>
		Any(const T& value)
		{
			_value = &value;
			_type = &sge::get_type(value);
		}

		template <typename T>
		Any(T&& value) = delete;

		Any(AnyMut any)
			: _value{ any.get_value() }, _type{ &any.get_type() }
		{
		}

		Any(Any& copy)
			: _value{ copy._value }, _type{ copy._type }
		{
		}
		Any(const Any& copy)
			: _value{ copy._value }, _type{ copy._type }
		{
		}
		Any(Any&& move)
			: _value{ move._value }, _type{ move._type }
		{
		}
		Any(const Any&& move)
			: _value{ move._value }, _type{ move._type }
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
