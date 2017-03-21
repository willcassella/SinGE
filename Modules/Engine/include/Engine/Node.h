// Node.h
#pragma once

#include <vector>
#include <Core/Math/Mat4.h>
#include "EventChannel.h"

namespace sge
{
    struct Scene;
	struct NodeTransformMod;
	struct NodeRootMod;

	struct NodeId
	{
		using Version_t = uint32;
		using Index_t = uint32;

		////////////////////////
		///   Constructors   ///
	public:

		NodeId()
			: index(0)
		{
		}

		explicit NodeId(uint64 value)
		{
			from_u64(value);
		}

		static NodeId null_id()
		{
			return NodeId{ 0 };
		}

		///////////////////
		///   Methods   ///
	public:

		uint64 to_u64() const
		{
			const uint64 result = 0;
			return result + index;
		}

		void from_u64(uint64 value)
		{
			index = value & 0x00000000FFFFFFFF;
		}

		std::size_t to_string(char* out_str, std::size_t size) const
		{
			return (std::size_t)std::snprintf(out_str, size, "%llu", to_u64());
		}

		void from_string(const char* str)
		{
			const uint64 value = std::strtoull(str, nullptr, 10);
			from_u64(value);
		}

		void to_archive(ArchiveWriter& writer) const
		{
			writer.number(to_u64());
		}

		void from_archive(ArchiveReader& reader)
		{
			uint64 value = 0;
			reader.number(value);
			from_u64(value);
		}

		bool is_null()
		{
			return index == 0;
		}

		/////////////////////
		///   Operators   ///
	public:

		friend bool operator==(const NodeId& lhs, const NodeId& rhs)
		{
			return lhs.index == rhs.index;
		}
		friend bool operator!=(const NodeId& lhs, const NodeId& rhs)
		{
			return !(lhs == rhs);
		}
		friend bool operator<(const NodeId& lhs, const NodeId& rhs)
		{
			return lhs.index < rhs.index;
		}

		//////////////////
		///   Fields   ///
	public:

		Index_t index;
	};

    struct SGE_ENGINE_API Node
    {
        SGE_REFLECTED_TYPE;
		friend Scene;

		using ModState_t = uint32;
        enum ModState : ModState_t
        {
	        /**
             * \brief This node has not been modified.
             */
            NONE = 0,

	        /**
             * \brief This node's root has been modified, but the change has not yet been applied.
             */
            ROOT_PENDING = 1 << 0,

	        /**
             * \brief This node's root has been modified during the frame, and the update has been applied to the scene.
             */
            ROOT_APPLIED = 1 << 1,

	        /**
             * \brief This node's local transform has been modified, and the update is pending.
             */
            TRANSFORM_PENDING = 1 << 2,

	        /**
             * \brief This node's local transform has been modified during the frame, and the update has been applied.
             */
            TRANSFORM_APPLIED = 1 << 3,

	        /**
             * \brief This node has been created this frame.
             */
            NEW = 1 << 4,

	        /**
             * \brief This node has been marked for destruction this frame.
             */
            DELETED = 1 << 5,
        };

        ////////////////////////
        ///   Constructors   ///
    private:

        Node();

        ///////////////////
        ///   Methods   ///
    public:

		/**
		 * \brief Returns the Id for this node.
		 */
        NodeId get_id() const;

	    /**
         * \brief Returns the name of this node.
         */
        const std::string& get_name() const;

	    /**
         * \brief Returns the current modification state for this node.
         */
        ModState_t get_mod_state() const;

	    /**
		 * \brief Returns the Id of the root for this node.
		 */
		NodeId get_root() const;

	    /**
         * \brief Sets the pending root of this node.
         * \param root The pending root to set. Use nullptr to set no root.
         */
        void set_root(Node* root);

	    /**
         * \brief Returns the pending root of this node.
         * NOTE: If there is no pending root, this returns the current root.
         */
        NodeId get_pending_root() const;

	    /**
		 * \brief Returns the current number of children of this node.
		 */
		std::size_t get_num_children() const;

	    /**
         * \brief Fills the given array with the Ids of all children of this node betwee [start_index, start_index + num_children)
         * \param start_index The index of the first child to retreive.
         * \param num_children The number of children to retreive.
         * \param out_num_children Variable to assign with the number of children retrieved.
         * \param out_children The array to fill with the ids of the children.
         * \return The number of children retreived.
         */
        std::size_t get_children(
			std::size_t start_index,
			std::size_t num_children,
			std::size_t* out_num_children,
			NodeId* out_children) const;

	    /**
         * \brief Sets this node as the pending root of the given node.
         * \param child The child to set.
         */
        void add_child(Node& child);

	    /**
         * \brief If the given node is a child of this node, sets its pending root as NULL.
         * \param child The child to remove.
         */
        void remove_child(Node& child);

	    /**
         * \brief Returns the position of this node relative to the root.
         */
        Vec3 get_local_position() const;

	    /**
         * \brief Sets the pending local position of this node.
         * \param pos The local position to set.
         */
        void set_local_position(Vec3 pos);

	    /**
         * \brief Returns the pending local position of this node.
         * NOTE: If there is no current pending local position, this returns the current local position.
         */
        Vec3 get_pending_local_position() const;

	    /**
         * \brief Returns the scale of this node relative to the root.
         */
        Vec3 get_local_scale() const;

	    /**
         * \brief Sets the pending local scale of this node.
         * \param scale The local scale to set.
         */
        void set_local_scale(Vec3 scale);

	    /**
         * \brief Returns the pending local scale of this node.
         * NOTE: If there is no current pending local scale, this returns the current local scale.
         */
        Vec3 get_pending_local_scale() const;

	    /**
         * \brief Returns the local rotation of this node relative to the root.
         */
        Quat get_local_rotation() const;

	    /**
         * \brief Sets the pending local rotation of this node.
         * \param rot The local rotation to set.
         */
        void set_local_rotation(Quat rot);

	    /**
         * \brief Returns the pending local rotation of this node.
         * NOTE: If there is no pending local rotation, this returns the current local rotation.
         */
        Quat get_pending_local_rotation() const;

	    /**
         * \brief Returns the local-to-world matrix for this node.
         */
        Mat4 get_world_matrix() const;

    private:

		NodeTransformMod& get_transform_mod();

		NodeRootMod& get_root_mod();

		void set_mod_state(ModState_t state);

        //////////////////
        ///   Fields   ///
    private:

        Scene* _scene;
        NodeId _id;
        NodeId _root;
        std::vector<NodeId> _child_nodes;
		uint32 _hierarchy_depth;
    	ModState_t _mod_state;
        int32 _transform_mod_index;
        int32 _root_mod_index;
        Vec3 _local_position;
        Vec3 _local_scale;
        Quat _local_rotation;
        Mat4 _cached_world_matrix;
        std::string _name;
    };

	struct ENewNode
	{
		Node* node;
	};

	struct EDestroyedNode
	{
		Node* node;
	};

	struct ENodeTransformChanged
	{
		Node* node;
	};

	struct ENodeRootChangd
	{
		Node* node;
		Node* root;
	};
}
