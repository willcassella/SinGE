// CVelocity.h
#pragma once

#include <Core/Math/Quat.h>
#include "../../Component.h"

namespace sge
{
	class SGE_ENGINE_API CVelocity final : public TComponentInterface<CVelocity>
	{
	public:

		SGE_REFLECTED_TYPE;

	    struct Data
	    {
            ///////////////////
            ///   Methods   ///
	    public:

            void to_archive(ArchiveWriter& writer) const;

            void from_archive(ArchiveReader& reader);

            //////////////////
            ///   Fields   ///
	    public:

            Vec3 linear_velocity;
            Vec3 angular_velocity;

            /////////////////////
            ///   Operators   ///
	    public:

            bool operator==(const Data& rhs) const;
            bool operator!=(const Data& rhs) const;
        };

        ////////////////////////
        ///   Constructors   ///
	public:

        CVelocity(ProcessingFrame& pframe, EntityId entity, Data& data);

        ///////////////////
        ///   Methods   ///
	public:

        static void register_type(Scene& scene);

        Vec3 linear_velocity() const;

        void linear_velocity(const Vec3& value);

        Vec3 angular_velocity() const;

        void angular_velocity(const Vec3& value);

        Data data() const;

        void data(const Data& value);

		//////////////////
		///   Fields   ///
	private:

        Data* _data;
	};
}
