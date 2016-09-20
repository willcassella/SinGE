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

		AnyMut(const AnyMut& copy) = default;
		AnyMut(AnyMut&& move) = default;

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

		Any(const Any& copy) = default;
		Any(Any&& move) = default;

		Any(AnyMut any)
			: _value{ any.get_value() }, _type{ &any.get_type() }
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
