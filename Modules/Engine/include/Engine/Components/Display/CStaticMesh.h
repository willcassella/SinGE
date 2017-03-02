// CStaticMesh.h
#pragma once

#include <Resource/Resources/StaticMesh.h>
#include "../../Component.h"
#include "../../Util/TagStorage.h"

namespace sge
{
	/* Component applied to entities that should render a static mesh. */
	class SGE_ENGINE_API CStaticMesh final : public TComponentInterface<CStaticMesh>
	{
    public:

		SGE_REFLECTED_TYPE;
		struct Data;

		///////////////////
		///   Methods   ///
	public:

		static void register_type(Scene& scene);

        void reset(Data& data);

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

	private:

        void generate_tags(std::map<const TypeInfo*, std::vector<TagBuffer>>& tags) override;

		////////////////
		///   Tags   ///
	public:

		struct SGE_ENGINE_API FMeshChanged
		{
			SGE_REFLECTED_TYPE;
		};

		struct SGE_ENGINE_API FMaterialChanged
		{
			SGE_REFLECTED_TYPE;
		};

        struct SGE_ENGINE_API FLightmapChanged
        {
            SGE_REFLECTED_TYPE;
        };

		//////////////////
		///   Fields   ///
	private:

		Data* _data = nullptr;
        TagStorage<FMeshChanged> _mesh_changed;
        TagStorage<FMaterialChanged> _material_changed;
        TagStorage<FLightmapChanged> _lightmap_changed;
	};
}
