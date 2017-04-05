// CStaticMesh.h
#pragma once

#include <Resource/Resources/StaticMesh.h>
#include "../../Component.h"

namespace sge
{
	/* Component applied to nodes that should render a static mesh. */
	class SGE_ENGINE_API CStaticMesh final
	{
	public:

		SGE_REFLECTED_TYPE;
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

        bool uses_lightmap() const;

        void set_uses_lightmap(bool value);

        const std::string& lightmap() const;

        void lightmap(std::string lightmap);

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
        std::string _lightmap;
		Vec2 _uv_scale = { 1.f, 1.f };
        IVec2<int32> _lightmap_size = { 512, 512 };
        bool _use_lightmap = false;
		SharedData* _shared_data;
	};
}
