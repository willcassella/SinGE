// ResourceManager.cpp

#include <Core/Memory/Functions.h>
#include "../include/Resource/ResourceManager.h"

namespace sge
{
	struct SGE_ALIGNED_BUFFER_HEADER ResourceManager::Resource
	{
		using Version = uint32;
		static constexpr Version VERSION_UNINITIALIZED = 0;
		static constexpr Version VERSION_INITIALIZED = 1;

		////////////////////////
		///   Constructors   ///
	private:

		Resource(const TypeInfo& type)
			: type(&type),
			version(VERSION_UNINITIALIZED),
			waiting_readers(0),
			version_last_modified(VERSION_UNINITIALIZED)
		{
		}
		~Resource()
		{
			if (version >= VERSION_INITIALIZED)
			{
				type->drop(object());
			}
		}

	public:

		static Resource* create_resource_object(const TypeInfo& type)
		{
			// Create a buffer for the resource
			void* buffer = sge::malloc(sizeof(Resource) + type.size());
			return new (buffer) Resource{ type };
		}

		///////////////////
		///   Methods   ///
	public:

		void* object()
		{
			return this + 1;
		}

		const void* object() const
		{
			return this + 1;
		}

		//////////////////
		///   Fields   ///
	public:

		const TypeInfo* const type;

		std::atomic<Version> version;

		std::atomic<uint32> waiting_readers;

		std::atomic<Version> version_last_modified;
	};

	std::string ResourceManager::fake_uri(const std::string& base)
	{
		// std::tmpnam isn't gauranteed to be thread-safe, so we use a mutex
		static std::mutex tmpnam_mutex;

		tmpnam_mutex.lock();
		std::string result = std::tmpnam(nullptr) + base;
		tmpnam_mutex.unlock();

		return result;
	}

	auto ResourceManager::new_resource(
		std::string uri,
		const TypeInfo& type,
		void* init) -> NewResourceResult
	{
		// Lock the resources map, and look up the uri
		_resources_lock.lock();

		auto iter = _resources.find(uri);
		if (iter != _resources.end())
		{
			return NewResourceResult::ERROR_RESOURCE_EXISTS;
		}

		// Initialize the resource
		auto* resource = Resource::create_resource_object(type);
		type.move_init(resource->object(), init);
		resource->version = Resource::VERSION_INITIALIZED;
		resource->version_last_modified = Resource::VERSION_INITIALIZED;

		// Add it to the resource map
		_resources.insert(std::make_pair(std::move(uri), resource));
	}

	auto ResourceManager::async_new_resource(
		const std::string& uri,
		const TypeInfo& type,
		CallbackQueue& cbQueue,
		std::function<WriteCallbackFn> writeFn) -> NewResourceResult
	{
		// Lock the resource map, and look up the resource
		_resources_lock.lock();
		auto iter = _resources.find(uri);

		// Make sure the resource doesn't already exist
		if (iter != _resources.end())
		{
			_resources_lock.unlock();
			return NewResourceResult::ERROR_RESOURCE_EXISTS;
		}

		// Initialize the resource object
		auto* resource = Resource::create_resource_object(type);
		//resource->callback_lock.lock();

		// Add it to the resource map
		_resources.insert(std::make_pair(uri, resource));
		_resources_lock.unlock();

		// Add this callback to the writers callback
		//resource->write_callbacks.push_back(std::move(writeFn));

		// Unlock the resource's callbacks
		//resource->callback_lock.unlock();

		return NewResourceResult::SUCCESS;
	}

	//auto ResourceManager::async_read_resource(
	//	const std::string& uri,
	//	const TypeInfo& type,
	//	AsyncToken token,
	//	std::function<ReadCallbackFn> readFn) -> AccessResourceResult
	//{
	//	// Lock the resource map, and look up the resource from the URI
	//	_resources_lock.lock();
	//	auto* resource = get_resource(uri);

	//	if (!resource)
	//	{
	//		_resources_lock.unlock();
	//		return AccessResourceResult::ERROR_NONEXISTANT_RESOURCE;
	//	}

	//	if (*resource->type != type)
	//	{
	//		_resources_lock.unlock();
	//		return AccessResourceResult::ERROR_TYPE_MISMATCH;
	//	}

	//	// Acquire the resource's callback lock, and release the resource map lock
	//	//resource->callback_lock.lock();
	//	_resources_lock.unlock();

	//	// Add ourselves to the reader callback queue and release the callback lock
	//	//resource->read_callbacks.push_back(std::move(readFn));
	//	//resource->callback_lock.unlock();

	//	return AccessResourceResult::SUCCESS;
	//}

	ResourceManager::Resource* ResourceManager::get_resource(const std::string& uri)
	{
		auto iter = _resources.find(uri);
		return iter == _resources.end() ? nullptr : iter->second;
	}
}
