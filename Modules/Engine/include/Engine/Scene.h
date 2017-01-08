// Scene.h
#pragma once

#include <map>
#include "SceneData.h"

namespace sge
{
	struct TypeDB;
	class SystemFrame;
	class SystemFrameMut;

	/**
	 * \brief Top-level scene interface.
	 */
	struct SGE_ENGINE_API Scene
	{
		SGE_REFLECTED_TYPE;

		/**
		 * \brief Token used to identify registered system functions, and later unregister them.
		 * This design may change in the future.
		 */
		using SystemFnToken = uint16;

		/**
		 * \brief Default system token value.
		 */
		static constexpr SystemFnToken NULL_SYSTEM_TOKEN = 0;

		/**
		 * \brief Function signature used for system funtions during the read phase. This design may change in the future.
		 * \param frame The frame to be used by the system function.
		 * \param current_time The current game time.
		 * \param dt The time delta since the last frame.
		 */
		using SystemFn = void(SystemFrame& frame, float current_time, float dt);

		/**
		 * \brief Function signature used for system functions during the read/write phase. This design may change in the future.
		 * \param frame The frame to be used by the system function.
		 * \param current_time The current game time.
		 * \param dt The time delta since the last frame.
		 */
		using SystemMutFn = void(SystemFrameMut& frame, float current_time, float dt);

		////////////////////////
		///   Constructors   ///
	public:

		Scene(TypeDB& typedb);
		Scene(const Scene& copy) = delete;
		Scene& operator=(const Scene& copy) = delete;
		Scene(Scene&& move) = default;
		Scene& operator=(Scene&& move) = default;
		~Scene();

		///////////////////
		///   Methods   ///
	public:

		/**
		 * \brief Resets entity/component data. Essentially creates a new scene without unregistering component types.
		 */
		void reset_scene();

		/**
		 * \brief Returns raw scene data. You should not use this unless you know what you're doing.
		 */
		const SceneData& get_raw_scene_data() const;

		/**
		 * \brief Serializes the state of this Scene to an Archive.
		 * \param writer The writer for the archive to serialize to.
		 */
		void to_archive(ArchiveWriter& writer) const;

		/**
		 * \brief Deserializes the state of this Scene from an Archive.
		 * \param reader The reader for the Archive to deserialize from.
		 */
		void from_archive(ArchiveReader& reader);

		/**
		 * \brief Returns the type database for this Scene.
		 */
		TypeDB& get_type_db();

		/**
		 * \brief Returns the type database for this Scene.
		 */
		const TypeDB& get_type_db() const;

		/**
		 * \brief Searches for a component type with the given name in the type database.
		 * \param typeName The name of the component type to search for.
		 * \return A pointer to the type information for the type, if found.
		 */
		const TypeInfo* get_component_type(const char* typeName) const;

		/**
		 * \brief Registers a new component type with the scene.
		 * \param type
		 * \param container
		 */
		void register_component_type(const TypeInfo& type, std::unique_ptr<ComponentContainer> container);

		/**
		 * \brief Registers a system function to be called during the read phase.
		 * This design may change in the future.
		 * \param system_fn The system function to call during the read phase.
		 * \return The token for the system function, which may be used to unregister it.
		 */
		SystemFnToken register_system_fn(std::function<SystemFn> system_fn);

		/**
		 * \brief Registers a system member function to be called during the read phase.
		 * This design may change in the future.
		 * \param outer The system object that the member function is to be called on.
		 * \param system_fn The system member function to call during the read phase.
		 * \return The token for the system function, which may be used to unregister it.
		 */
		template <typename T>
		SystemFnToken register_system_fn(T* outer, void(T::*system_fn)(SystemFrame& frame, float current_time, float dt))
		{
			return this->register_system_fn(std::bind(
				system_fn,
				outer,
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3));
		}

		/**
		 * \brief Registers a system function to be called during the read/write phase.
		 * This design may change in the future.
		 * \param system_fn The system function to call during the read/write phase.
		 * \return The token for the system function, which may be used to unregister it.
		 */
		SystemFnToken register_system_mut_fn(std::function<SystemMutFn> system_fn);

		/**
		 * \brief Registers a system member function to be called during the read/write phase.
		 * This design may change in the future.
		 * \param outer The system object that the member function is to be called on.
		 * \param system_fn The system member function to call during the read/write phase.
		 * \return The token for the system function, which may be used to unregister it.
		 */
		template <typename T>
		SystemFnToken register_system_mut_fn(T* outer, void(T::*system_fn)(SystemFrameMut& frame, float current_time, float dt))
		{
			return this->register_system_mut_fn(std::bind(
				system_fn,
				outer,
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3));
		}

		/**
		 * \brief Unregisters the system function identified by the given token.
		 * \param token The token associated with the system function to unregister.
		 */
		void unregister_system_fn(SystemFnToken token);

		/**
		 * \brief Runs a full read-read/write update of the scene, with the given time delta.
		 * \param dt The game time that is supposed to have passed since the last call to 'update'.
		 */
		void update(float dt);

		//////////////////
		///   Fields   ///
	private:

		float _current_time;
		TypeDB* _type_db;

		/* Scene data */
		SceneData _scene_data;

		/* System data */
		SystemFnToken _next_system_fn_token;
		std::map<SystemFnToken, std::function<SystemFn>> _system_fns;
		std::map<SystemFnToken, std::function<SystemMutFn>> _system_mut_fns;
	};
}
