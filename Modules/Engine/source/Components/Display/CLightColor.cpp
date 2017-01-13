// CLightColor.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Display/CLightColor.h"
#include "../../../include/Engine/Scene.h"

SGE_REFLECT_TYPE(sge::CLightColor)
.property("color", &CLightColor::color, &CLightColor::color)
.property("red", &CLightColor::red, &CLightColor::red, PF_EDITOR_HIDDEN)
.property("blue", &CLightColor::blue, &CLightColor::blue, PF_EDITOR_HIDDEN)
.property("green", &CLightColor::green, &CLightColor::green, PF_EDITOR_HIDDEN)
.property("alpha", &CLightColor::alpha, &CLightColor::alpha, PF_EDITOR_HIDDEN);

namespace sge
{
	struct CLightColor::Data
	{
		///////////////////
		///   Methods   ///
	public:

		void to_archive(ArchiveWriter& writer) const
		{
			writer.object_member("color", color);
		}

		void from_archive(ArchiveReader& reader)
		{
			reader.object_member("color", color);
		}

		//////////////////
		///   Fields   ///
	public:

		Color_t color;
	};

	CLightColor::CLightColor(ProcessingFrame& pframe, EntityId entity, Data& data)
		: TComponentInterface<sge::CLightColor>(pframe, entity),
		_cached_color(data.color),
		_data(&data)
	{
	}

	void CLightColor::register_type(Scene& scene)
	{
		scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CLightColor, Data>>());
	}

	CLightColor::Color_t CLightColor::color() const
	{
		return _cached_color;
	}

	void CLightColor::color(Color_t value)
	{
		_cached_color = value;
		update_data();
	}

	CLightColor::Color_t::Red_t CLightColor::red() const
	{
		return _cached_color.red();
	}

	void CLightColor::red(Color_t::Red_t value)
	{
		_cached_color.red(value);
		update_data();
	}

	CLightColor::Color_t::Green_t CLightColor::green() const
	{
		return _cached_color.green();
	}

	void CLightColor::green(Color_t::Green_t value)
	{
		_cached_color.green(value);
		update_data();
	}

	CLightColor::Color_t::Blue_t CLightColor::blue() const
	{
		return _cached_color.blue();
	}

	void CLightColor::blue(Color_t::Blue_t value)
	{
		_cached_color.blue(value);
		update_data();
	}

	CLightColor::Color_t::Alpha_t CLightColor::alpha() const
	{
		return _cached_color.alpha();
	}

	void CLightColor::alpha(Color_t::Alpha_t value)
	{
		_cached_color.alpha(value);
		update_data();
	}

	void CLightColor::update_data()
	{
		_data->color = _cached_color;
		apply_component_modified_tag();
	}
}
