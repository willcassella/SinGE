// CStaticMesh.h
#pragma once

#include <Resource/Resources/StaticMesh.h>
#include "../../Component.h"

namespace sge
{
	/* Component applied to nodes that should render a static mesh. */
	struct SGE_ENGINE_API CStaticMesh
	{
		SGE_REFLECTED_TYPE;

		enum class LightmaskMode
		{
			/**
			 * \brief This mesh instance does not participate in lightmask rendering.
			 */
			NONE,

			/**
			 * \brief This mesh is treated as a lightmask occluder (shadow-caster).
			 */
			OCCLUDER,

			/**
			 * \brief This mesh is treated as a lightmask receiver.
			 */
			RECEIVER
		};

		struct SharedData;

		////////////////////////
		///   Constructors   ///
	public:

		explicit CStaticMesh(NodeId node, SharedData& shared_data);

		///////////////////
		///   Methods   ///
	public:

		static void register_type(Scene& scene);

        void to_archive(ArchiveWriter& writer) const;

        void from_archive(ArchiveReader& reader);

		const std::string& mesh() const;

		void mesh(std::string mesh);

		const std::string& material() const;

		void material(std::string material);

		LightmaskMode lightmask_mode() const;

		void lightmask_mode(LightmaskMode value);

		uint32 lightmask_group() const;

		void lightmask_group(uint32 value);

        bool uses_lightmap() const;

        void set_uses_lightmap(bool value);

        int32 lightmap_width() const;

        void lightmap_width(int32 width);

        int32 lightmap_height() const;

        void lightmap_height(int32 height);

		Vec2 uv_scale() const;

		void uv_scale(Vec2 value);

	private:

		void set_modified(const char* property_name);

        //////////////////
        ///   Fields   ///
    private:

		NodeId _node;
        std::string _mesh;
        std::string _material;
		LightmaskMode _lightmask_mode = LightmaskMode::NONE;
		uint32 _lightmask_group = 0;
        bool _use_lightmap = false;
		Vec2 _uv_scale = { 1.f, 1.f };
        IVec2<int32> _lightmap_size = { 512, 512 };
		SharedData* _shared_data;
	};
}

SGE_REFLECTED_ENUM(SGE_ENGINE_API, sge::CStaticMesh::LightmaskMode);
