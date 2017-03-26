// EditorServerSystem.h
#pragma once

#include <memory>
#include <Core/Reflection/Reflection.h>
#include <Engine/Scene.h>
#include "config.h"

namespace sge
{
	struct SGE_EDITORSERVERSYSTEM_API EditorServerSystem
	{
		struct Data;
		SGE_REFLECTED_TYPE;

		////////////////////////
		///   Constructors   ///
	public:

		EditorServerSystem(uint16 port);
		~EditorServerSystem();

		///////////////////
		///   Methods   ///
	public:

		void register_pipeline(UpdatePipeline& pipeline);

		int get_serve_time() const;

		void set_serve_time(int milliseconds);

	private:

        void serve_fn(Scene& scene, SystemFrame& frame);

		//////////////////
		///   Fields   ///
	private:

		int _serve_time_ms;
		std::unique_ptr<Data> _data;
	};
}
