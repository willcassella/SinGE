// Any.h
#pragma once

namespace sge
{
	struct TypeInfo;

	template <typename T>
	const TypeInfo& get_type(const T&);

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

		////////////////
		///   Data   ///
	private:

		const void* _value;
		const TypeInfo* _type;
	};

	struct AnyMut
	{
		////////////////////////
		///   Constructors   ///
	public:

		template <typename T>
		AnyMut(T& value)
		{
			_value = &value;
			_type = &get_type(value);
		}

		template <typename T>
		AnyMut(T&& value) = delete;

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

		////////////////
		///   Data   ///
	private:

		void* _value;
		const TypeInfo* _type;
	};
}
