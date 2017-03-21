// Scene.cpp

#include <iostream>
#include <Core/Reflection/TypeDB.h>
#include <Core/Reflection/ReflectionBuilder.h>
#include <Core/Util/StringUtils.h>
#include "../include/Engine/Scene.h"
#include "../include/Engine/SystemFrame.h"
#include "../include/Engine/UpdatePipeline.h"
#include "../include/Engine/SystemInfo.h"
#include "../include/Engine/Component.h"

SGE_REFLECT_TYPE(sge::Scene)
.implements<IToArchive>()
.implements<IFromArchive>();

namespace sge
{
	////////////////////////
	///   Constructors   ///

	Scene::Scene(TypeDB& typedb)
		: _type_db(&typedb)
	{
		_current_time = 0;
	}

	Scene::~Scene()
	{
	}

	///////////////////
	///   Methods   ///

	void Scene::create_nodes(std::size_t num_nodes, Node** out_nodes)
	{
		for (std::size_t i = 0; i < num_nodes; ++i)
		{
			// Reserve an Id for the new node
			const auto id = _scene_data.next_node_id;
			_scene_data.next_node_id.index += 1;

			// Allocate space for it
			auto* buff = _scene_data.node_buffer.alloc(sizeof(Node));
			auto* node = new (buff) Node();

			// Initialize it
			node->_id = id;
			node->_scene = this;
			node->_mod_state = Node::NEW;

			// Insert it
			_scene_data.nodes[id] = node;
			out_nodes[i] = node;
		}

		// Add all new nodes to the 'new nodes' buffer
		_scene_data.new_nodes.insert(_scene_data.new_nodes.end(), out_nodes, out_nodes + num_nodes);
		_scene_data.modified_nodes.insert(_scene_data.modified_nodes.end(), out_nodes, out_nodes + num_nodes);
	}

	void Scene::destroy_nodes(std::size_t num_nodes, Node* const* nodes)
	{
		// Mark all nodes as pending destroy
		for (std::size_t i = 0; i < num_nodes; ++i)
		{
			nodes[i]->set_mod_state(nodes[i]->_mod_state | Node::DELETED);
		}

		// Add all nodes to the list of pending destroy nodes
		_scene_data.destroyed_nodes.insert(_scene_data.destroyed_nodes.end(), nodes, nodes + num_nodes);
	}

	void Scene::get_nodes(const NodeId* nodes, std::size_t num_nodes, Node** out_nodes)
	{
		for (std::size_t i = 0; i < num_nodes; ++i)
		{
			const auto node = nodes[i];
			const auto iter = _scene_data.nodes.find(node);
			out_nodes[i] = iter != _scene_data.nodes.end() ? iter->second : nullptr;
		}
	}

	void Scene::get_nodes(const NodeId* nodes, std::size_t num_nodes, const Node** out_nodes) const
	{
		for (std::size_t i = 0; i < num_nodes; ++i)
		{
			const auto node = nodes[i];
			const auto iter = _scene_data.nodes.find(node);
			out_nodes[i] = iter != _scene_data.nodes.end() ? iter->second : nullptr;
		}
	}

	std::size_t Scene::num_root_nodes() const
	{
		return _scene_data.root_nodes.size();
	}

	std::size_t Scene::get_root_nodes(std::size_t start_index, std::size_t num_nodes, std::size_t* out_num_nodes, NodeId* out_nodes) const
	{
		if (start_index >= _scene_data.root_nodes.size())
		{
			*out_num_nodes = 0;
			return 0;
		}

		const auto num_copy = std::min(_scene_data.root_nodes.size() - start_index, num_nodes);
		std::memcpy(out_nodes, _scene_data.root_nodes.data() + start_index, num_copy * sizeof(NodeId));
		*out_num_nodes = num_copy;
		return num_copy;
	}

	ComponentContainer* Scene::get_component_container(const TypeInfo& type)
	{
		const auto iter = _scene_data.components.find(&type);
		return iter != _scene_data.components.end() ? iter->second.get() : nullptr;
	}

	EventChannel* Scene::get_event_channel(const TypeInfo& component_type, const char* channel_name)
	{
		auto* comp = get_component_container(component_type);
		if (!comp)
		{
			return nullptr;
		}

		return comp->get_event_channel(channel_name);
	}

	EventChannel* Scene::get_node_transform_changed_channel()
	{
		return &_scene_data.node_transform_changed_channel;
	}

	EventChannel* Scene::get_node_root_changed_channel()
	{
		return &_scene_data.node_root_changed_channel;
	}

	void Scene::reset_scene()
	{
		_current_time = 0;
		_scene_data.next_node_id.index = 1;
		_scene_data.nodes.clear();
		_scene_data.node_buffer.clear();
		_scene_data.root_nodes.clear();
		_scene_data.node_root_changes.clear();
		_scene_data.node_transform_changes.clear();
		_scene_data.new_nodes.clear();
		_scene_data.destroyed_nodes.clear();

		for (auto& component_type : _scene_data.components)
		{
			component_type.second->reset();
		}
	}

	SceneData& Scene::get_raw_scene_data()
	{
		return _scene_data;
	}

	const SceneData& Scene::get_raw_scene_data() const
	{
		return _scene_data;
	}

	void Scene::to_archive(ArchiveWriter& writer) const
	{
		// Serialize next entity id
		writer.object_member("next_node_id", _scene_data.next_node_id);

		// Serialize nodes
		writer.push_object_member("nodes");
		for (const auto node : _scene_data.nodes)
		{
			char id_str[20];
			node.first.to_string(id_str, 20);
			writer.push_object_member(id_str);

			// Write the entity name and root id
			writer.object_member("name", node.second->get_name());
			writer.object_member("root", node.second->get_root());

			// Write transform
			writer.object_member("lpos", node.second->get_local_position());
			writer.object_member("lscale", node.second->get_local_scale());
			writer.object_member("lrot", node.second->get_local_rotation());

			writer.pop(); // id
		}
		writer.pop(); // "nodes"

		// Serialize all components
		writer.push_object_member("components");
		for (const auto& componentType : _scene_data.components)
		{
			// Add the component type name as a field
			writer.push_object_member(componentType.first->name().c_str());
			componentType.second->to_archive(writer);
			writer.pop();
		}
		writer.pop(); // "components"
	}

	void Scene::from_archive(ArchiveReader& reader)
	{
		reset_scene();

		// Deserialize nodes
		reader.object_member("next_node_id", _scene_data.next_node_id);
		reader.pull_object_member("nodes");

		// Get the number of nodes
		std::size_t num_nodes = 0;
		reader.object_size(num_nodes);

		// Load all nodes
		reader.enumerate_object_members([this, &reader, &data = _scene_data](const char* id_str)
		{
			// Get the Id
			NodeId id;
			id.from_string(id_str);

			// Validate the Id
			if (id.is_null() || id.index >= _scene_data.next_node_id.index)
			{
				std::cout << "Error: Invalid node Id" << std::endl;
				return;
			}

			// Allocate the node
			void* buff = data.node_buffer.alloc(sizeof(Node));
			auto* node = new (buff) Node();

			// Initialize it
			node->_id = id;
			node->_scene = this;
			node->_mod_state = Node::NEW | Node::TRANSFORM_PENDING;
			node->_transform_mod_index = (int32)this->_scene_data.node_transform_changes.size();

			// Deserialize node data
			reader.object_member("root", node->_root);
			reader.object_member("name", node->_name);

			// Deserialize transform data
			NodeTransformMod trans;
			trans.node = node;
			reader.object_member("lpos", trans.local_pos);
			reader.object_member("lscale", trans.local_scale);
			reader.object_member("lrot", trans.local_rot);
			this->_scene_data.node_transform_changes.push_back(trans);

			// Insert it into the scene
			data.nodes[id] = node;
			this->_scene_data.new_nodes.push_back(node);
		});
		reader.pop(); // "nodes"

		// Fix-up parent-child relationships
		for (auto node : _scene_data.nodes)
		{
			if (node.second->_root.is_null())
			{
				_scene_data.root_nodes.push_back(node.first);
				continue;
			}

			// Search for the parent
			const auto iter = _scene_data.nodes.find(node.second->_root);
			if (iter == _scene_data.nodes.end())
			{
				node.second->_root = NodeId::null_id();
				continue;
			}

			// Add the node as a child of the parent
			iter->second->_child_nodes.push_back(node.first);
		}

		// Initialize hierarchy depth
		initialize_hierarchy_depths();

		// Deserialize all components
		reader.pull_object_member("components");
		reader.enumerate_object_members([&](const char* name)
		{
			// Try to get the component type
			auto type = get_component_type(name);
			if (!type)
			{
				return;
			}

			// Deserialize the storage object
			auto storageIter = _scene_data.components.find(type);
			storageIter->second->from_archive(reader);
		});

		reader.pop(); // "components"

		// Validate components
		for (auto& component_type : _scene_data.components)
		{
			NodeId component_instances[8];
			std::size_t num_instances;
			std::size_t start_index = 0;
			auto* component = component_type.second.get();

			while (component->get_instance_nodes(start_index, 8, &num_instances, component_instances))
			{
				start_index += 8;
				NodeId destroy_instances[8];
				std::size_t num_destroy = 0;

				// Make sure all the nodes exist
				for (std::size_t i = 0; i < num_instances; ++i)
				{
					const auto iter = _scene_data.nodes.find(component_instances[i]);
					if (iter == _scene_data.nodes.end())
					{
						// Mark the instance to be destroyed
						destroy_instances[num_destroy] = component_instances[i];
						num_destroy += 1;
					}
				}

				// Destroy all invalid instances
				component->remove_instances(destroy_instances, num_destroy);
			}
		}

		// Apply changes to scene (generate events and matrices)
		on_end_system_frame();
	}

	TypeDB& Scene::get_type_db()
	{
		return *_type_db;
	}

	const TypeDB& Scene::get_type_db() const
	{
		return *_type_db;
	}

	const TypeInfo* Scene::get_component_type(const char* typeName) const
	{
		auto type = _type_db->find_type(typeName);
		if (type && _scene_data.components.find(type) != _scene_data.components.end())
		{
			return type;
		}

		return nullptr;
	}

	void Scene::register_component_type(const TypeInfo& type, std::unique_ptr<ComponentContainer> container)
	{
		// Insert the type
		_scene_data.components.insert(std::make_pair(&type, std::move(container)));
		_type_db->new_type(type);
	}

	void Scene::update(UpdatePipeline& pipeline, float dt)
	{
		const auto& pipeline_steps = pipeline.get_pipeline();

		// Execute the pipeline as a job queue
		execute_job_queue(pipeline_steps.data(), pipeline_steps.size(), pipeline, dt);

		// End the frame for all components
		for (auto& component_type : _scene_data.components)
		{
			component_type.second->on_end_update_frame();
		}

		// Reset node modification states
		for (auto mod_nodes : _scene_data.modified_nodes)
		{
			mod_nodes->_mod_state = Node::NONE;
		}
		_scene_data.modified_nodes.clear();

		// Clear event channels
		_scene_data.new_nodes.clear();
		_scene_data.destroyed_nodes.clear();
		_scene_data.node_transform_changes.clear();
		_scene_data.node_root_changes.clear();
		_scene_data.new_node_channel.clear();
		_scene_data.destroyed_node_channel.clear();
		_scene_data.node_transform_changed_channel.clear();
		_scene_data.node_root_changed_channel.clear();

		// Update time
		_current_time += dt;
	}

	void Scene::initialize_hierarchy_depths()
	{
		const NodeId* root_nodes = _scene_data.root_nodes.data();
		const auto num_root_nodes = _scene_data.root_nodes.size();

		// Create buffer for nodes
		std::vector<Node*> current_nodes;
		std::vector<Node*> next_nodes;
		current_nodes.assign(num_root_nodes, nullptr);
		get_nodes(root_nodes, num_root_nodes, current_nodes.data());

		uint32 current_depth = 0;
		while (!current_nodes.empty())
		{
			for (auto node : current_nodes)
			{
				// Update hierarchy depth
				node->_hierarchy_depth = current_depth;

				// Add children
				const auto offset = next_nodes.size();
				next_nodes.insert(next_nodes.end(), node->_child_nodes.size(), nullptr);
				get_nodes(node->_child_nodes.data(), node->_child_nodes.size(), next_nodes.data() + offset);
			}

			// Move to next nodes
			std::swap(current_nodes, next_nodes);
			next_nodes.clear();

			// Increment depth
			current_depth += 1;
		}
	}

	void Scene::execute_job_queue(SystemInfo* const* jobs, std::size_t num_jobs, UpdatePipeline& pipeline, float time_delta)
	{
		for (std::size_t i = 0; i < num_jobs; ++i)
		{
			// Create a system frame for the job
			SystemFrame frame;
			frame._current_time = _current_time;
			frame._time_delta = time_delta;
			frame._scene = this;
			frame._update_pipeline = &pipeline;

			// Run the job
			jobs[i]->system_fn(*this, frame);

			on_end_system_frame();

			// Run the job's created jobs
			execute_job_queue(frame._job_queue.data(), frame._job_queue.size(), pipeline, time_delta);
		}
	}

	void Scene::on_end_system_frame()
	{
		// Create array of nodes that need to have their matrices updated
		std::vector<Node*> outdated_matrices;

		// Transform nodes
		for (auto node_trans : _scene_data.node_transform_changes)
		{
			// Apply transform
			node_trans.node->_local_position = node_trans.local_pos;
			node_trans.node->_local_scale = node_trans.local_scale;
			node_trans.node->_local_rotation = node_trans.local_rot;
			node_trans.node->_cached_world_matrix =
				Mat4::translation(node_trans.local_pos) *
				Mat4::rotate(node_trans.local_rot) *
				Mat4::scale(node_trans.local_scale);

			// Update state
			node_trans.node->_transform_mod_index = -1;
			node_trans.node->_mod_state = (node_trans.node->_mod_state & ~Node::TRANSFORM_PENDING) | Node::TRANSFORM_APPLIED;
		}

		// Update matrices

		// Create a single temporary buffer for all event types
		const auto max_event_size = std::max({
			sizeof(ENewNode),
			sizeof(EDestroyedNode),
			sizeof(ENodeTransformChanged),
			sizeof(ENodeRootChangd) });
		const auto num_new_nodes = _scene_data.new_nodes.size();
		const auto num_destroyed_nodes = _scene_data.destroyed_nodes.size();
		const auto num_transformed_nodes = _scene_data.node_transform_changes.size();
		const auto num_root_changes = _scene_data.node_root_changes.size();
		const auto max_event_count = std::max({
			num_new_nodes,
			num_destroyed_nodes,
			num_transformed_nodes,
			num_root_changes });
		void* const event_buff = std::malloc(max_event_count * max_event_size);

		// Create new node events
		const auto* const new_nodes = _scene_data.new_nodes.data();
		for (std::size_t i = 0; i < num_new_nodes; ++i)
		{
			((ENewNode*)event_buff)[i].node = new_nodes[i];
		}
		_scene_data.new_node_channel.append(event_buff, sizeof(ENewNode), static_cast<int32>(num_new_nodes));

		// Create destroyed node events
		const auto* const destroyed_nodes = _scene_data.destroyed_nodes.data();
		for (std::size_t i = 0; i < num_destroyed_nodes; ++i)
		{
			((EDestroyedNode*)event_buff)[i].node = destroyed_nodes[i];
		}
		_scene_data.destroyed_node_channel.append(event_buff, sizeof(EDestroyedNode), static_cast<int32>(num_destroyed_nodes));

		// Create transformed node events
		const auto* const transformed_nodes = _scene_data.node_transform_changes.data();
		for (std::size_t i = 0; i < num_transformed_nodes; ++i)
		{
			((ENodeTransformChanged*)event_buff)[i].node = transformed_nodes[i].node;
		}
		_scene_data.node_transform_changed_channel.append(event_buff, sizeof(ENodeTransformChanged), static_cast<int32>(num_transformed_nodes));

		// Create root changed events
		const auto* const root_changed_nodes = _scene_data.node_root_changes.data();
		for (std::size_t i = 0; i < num_root_changes; ++i)
		{
			((ENodeRootChangd*)event_buff)[i].node = root_changed_nodes[i].node;
			((ENodeRootChangd*)event_buff)[i].root = root_changed_nodes[i].root;
		}
		_scene_data.node_root_changed_channel.append(event_buff, sizeof(ENodeRootChangd), static_cast<int32>(num_root_changes));
	}

	void Scene::update_matrices(Node* const* nodes, std::size_t num_nodes)
	{
		// Allocate a buffer for root Ids and root pointers
		auto* const root_ids = (NodeId*)std::malloc(sizeof(NodeId) * num_nodes + sizeof(Node*) * num_nodes);
		auto* const root_nodes = (Node**)(root_ids + num_nodes);

		// Fill the array of parent Ids
		for (std::size_t i = 0; i < num_nodes; ++i)
		{
			root_ids[i] = nodes[i]->_root;
		}

		// Get parents
		get_nodes(root_ids, num_nodes, root_nodes);

		// Create buffer for transform events
		std::vector<ENodeTransformChanged> transform_events;
		transform_events.reserve(num_nodes);

		// Create a buffer for child nodes
		std::vector<Node*> child_nodes;

		// Update matrices
		for (std::size_t i = 0; i < num_nodes; ++i)
		{
			// Calculate the new matrix
			const Mat4 root_matrix = root_nodes[i] ? root_nodes[i]->_cached_world_matrix : Mat4{};
			const Mat4 node_matrix =
				root_matrix *
				Mat4::translation(nodes[i]->_local_position) *
				Mat4::rotate(nodes[i]->_local_rotation) *
				Mat4::scale(nodes[i]->_local_scale);

			// Update node
			nodes[i]->_cached_world_matrix = node_matrix;
			nodes[i]->_mod_state = (nodes[i]->_mod_state & ~Node::TRANSFORM_PENDING) | Node::TRANSFORM_APPLIED;

			// Create event
			ENodeTransformChanged event;
			event.node = nodes[i];
			transform_events.push_back(event);

			// Update children
			child_nodes.assign(nodes[i]->_child_nodes.size(), nullptr);
			get_nodes(nodes[i]->_child_nodes.data(), child_nodes.size(), child_nodes.data());
			update_child_matrices(node_matrix, child_nodes, 0, transform_events);
			child_nodes.clear();
		}

		// Create events
		_scene_data.node_transform_changed_channel.append(transform_events.data(), (int32)transform_events.size());

		// Clean up
		std::free(root_ids);
	}

	void Scene::update_child_matrices(
		const Mat4& parent_matrix,
		std::vector<Node*>& nodes,
		std::size_t offset,
		std::vector<ENodeTransformChanged>& out_events)
	{
		const auto num_nodes = nodes.size();
		out_events.reserve(out_events.size() + num_nodes - offset);

		for (std::size_t i = offset; i < num_nodes; ++i)
		{
			auto* const node = nodes[i];

			// If this node has a pending transform, skip it
			if (node->_mod_state & Node::TRANSFORM_PENDING)
			{
				continue;
			}

			// Update this node
			node->_cached_world_matrix =
				parent_matrix *
				Mat4::translation(node->_local_position) *
				Mat4::rotate(node->_local_rotation) *
				Mat4::scale(node->_local_scale);
			node->set_mod_state(node->_mod_state | Node::TRANSFORM_APPLIED);

			// Create event
			ENodeTransformChanged event;
			event.node = node;
			out_events.push_back(event);

			// Add child nodes
			const auto num_child_nodes = node->_child_nodes.size();
			auto* const child_nodes = node->_child_nodes.data();
			nodes.insert(nodes.end(), num_child_nodes, nullptr);
			get_nodes(child_nodes, num_child_nodes, nodes.data() + num_nodes);

			// Update children
			update_child_matrices(node->_cached_world_matrix, nodes, num_nodes, out_events);

			// Remove child nodes
			nodes.erase(nodes.begin() + num_nodes, nodes.end());
		}
	}

}
