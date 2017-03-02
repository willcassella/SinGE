# scene_manager.py

from copy import deepcopy

# Recursively updates a dictionary D with E.
# This assumes that E has a structure that is a subset of D
def recursive_update(D, E):
    for k, v in E.items():
        if isinstance(v, dict):
            recursive_update(D[k], v)
        else:
            D[k] = v

class Entity(object):
    NULL_ENTITY_ID = 0
    WORLD_ENTITY_ID = 1

    def __init__(self, id):
        self.id = id
        self.name = ""
        self.parent = Entity.WORLD_ENTITY_ID
        self.components = set()
        self.user_data = None

class ComponentType(object):
    def __init__(self):
        self.instances = dict()
        self.update_instance_callback = None
        self.destroy_instance_callback = None

class SceneManager(object):
    def __init__(self):
        self._new_entity_callback = None
        self._update_entity_callback = None
        self._destroy_entity_callback = None
        self._next_entity_id = None
        self._entities = dict()
        self._destroyed_entities = set()
        self._set_entity_name_queries = dict()
        self._set_entity_parent_queries = dict()
        self._components = dict()
        self._new_component_queries = dict()
        self._destroyed_components = dict()
        self._get_component_queries = dict()
        self._set_component_queries = dict()
        self._num_new_entities = 0
        self._sent_scene_query = False
        self._save_scene_path = ''
        self._generate_lightmaps_query = False

    def register_handlers(self, session):
        session.add_query_handler('get_scene', self._get_scene_query)
        session.add_response_handler('get_scene', self._get_scene_response)
        session.add_query_handler('new_entity', self._new_entity_query)
        session.add_query_handler('destroy_entity', self._destroy_entity_query)
        session.add_query_handler('set_entity_name', self._set_entity_name_query)
        session.add_query_handler('new_component', self._new_component_query)
        session.add_query_handler('destroy_component', self._destroy_component_query)
        session.add_query_handler('get_component', self._get_component_query)
        session.add_response_handler('get_component', self._get_component_response)
        session.add_query_handler('set_component', self._set_component_query)
        session.add_query_handler('save_scene', self._save_scene_query)
        session.add_query_handler('gen_lightmaps', self._gen_lightmaps_query)

    def _get_scene_query(self, seq_number, priority):
        if not self._sent_scene_query:
            self._sent_scene_query = True
            return True # Actual value doesn't matter

    def _get_scene_response(self, seq_number, response):
        if response is None:
            return

        # Get the next entity id (used for constructing new entities client-side)
        self._next_entity_id = response['next_entity_id']

        # Store all newly entityies created
        new_entities = set()

        # For each entity in the scene
        for entity_id_str, value in response['entities'].items():
            entity_id = int(entity_id_str)

            # Insert a new entry into the entities table
            entity = Entity(entity_id)
            self._entities[entity_id] = entity

            # Initialize the entity
            entity.name = value['name']
            entity.parent = value['parent']

            # Run the 'new entity' callback
            if self._new_entity_callback is not None:
                self._new_entity_callback(self, entity)

            # Add the entity to the list of newly created entities
            new_entities.add(entity)

        # For each component type
        for component_type, instances in response['components'].items():
            component = self._components.setdefault(component_type, ComponentType())

            # For each instance of this component type
            for entity_id in instances:
                # Add the component value dictionary
                component.instances[entity_id] = dict()

                # Create the entity, if the entity did not appear in the scene structure (allowed)
                if entity_id in self._entities:
                    entity = self._entities[entity_id]
                else:
                    # Create the entity
                    entity = Entity(entity_id)
                    self._entities[entity_id] = entity
                    new_entities.add(entity)

                    # Run user callback
                    if self._new_entity_callback is not None:
                        self._new_entity_callback(self, entity)

                # Add the component to the entity
                entity.components.add(component_type)

                # Add a query to retrieve the value
                self.add_get_component_query(entity_id, component_type)

        # Run the 'update_entity' callback on all created entities
        if self._update_entity_callback is not None:
            for entity in new_entities:
                self._update_entity_callback(self, entity)

    def _new_entity_query(self, seq_number, priority):
        if self._num_new_entities == 0:
            return None

        message = self._num_new_entities
        self._num_new_entities = 0
        return message

    def _destroy_entity_query(self, seq_number, priority):
        if len(self._destroyed_entities) == 0:
            return None

        message = list(self._destroyed_entities)
        self._destroyed_entities.clear()
        return message

    def _set_entity_name_query(self, seq_number, priority):
        if len(self._set_entity_name_queries) == 0:
            return None

        message = self._set_entity_name_queries
        self._set_entity_name_queries = dict()
        return message

    def _new_component_query(self, seq_number, priority):
        # Make sure thare are actually outstanding 'new_component' queries
        if len(self._new_component_queries) == 0:
            return None

        # Construct the message
        message = dict()
        for entity_id, components in self._new_component_queries.items():
            message[entity_id] = list(components)

        # Reset the 'new_component' query table
        self._new_component_queries.clear()
        return message

    def _destroy_component_query(self, seq_number, prioirty):
        if len(self._destroyed_components) == 0:
            return None

        # Create the message
        message = dict()
        for component_type, instances in self._destroyed_components.items():
            message[component_type] = list(instances)

        self._destroyed_components.clear()
        return message

    def _get_component_query(self, seq_number, prioirty):
        # If there are no get component queries, just skip this
        if len(self._get_component_queries) == 0:
            return None

        # Create a message object
        message = dict()

        # For each entry in the table of 'get_component' queries
        for component_type, entities in self._get_component_queries.items():
            message[component_type] = list(entities)

        # Restore the get_component structure
        self._get_component_queries.clear()
        return message

    def _get_component_response(self, seq_number, response):
        if response is None:
            return

        # For each component type in the response
        for component_type, instances in response.items():

            # For each entity in the response
            for entity_id_str, component_value in instances.items():

                entity_id = int(entity_id_str)

                # Get the component
                component = self._components[component_type]

                # Set the component value
                assert(entity_id in component.instances)
                component.instances[entity_id] = component_value

                # Update the values with the values waiting to be set
                recursive_update(component_value, self._set_component_queries.get(component_type, dict()).get(entity_id, dict()))

                # If there's a callback for this component type
                if component.update_instance_callback is not None:
                    entity = self._entities[entity_id]
                    component.update_instance_callback(self, entity, component_value)

    def _set_component_query(self, seq_number, priority):
        # Make sure there are actually outstanding 'set_component' queries
        if len(self._set_component_queries) == 0:
            return None

        # Create the message
        message = self._set_component_queries
        self._set_component_queries = dict()
        return message

    def _save_scene_query(self, seq_number, priority):
        if len(self._save_scene_path) == 0:
            return None

        message = {
            'path': self._save_scene_path,
        }
        self._save_scene_path = ''
        return message

    def _gen_lightmaps_query(self, seq_number, priority):
        if not self._generate_lightmaps_query:
            return None

        # Actual value doesn't matter
        self._generate_lightmaps_query = False
        return True

    def save_scene(self, path):
        self._save_scene_path = path

    def generate_lightmaps(self):
        self._generate_lightmaps_query = True

    def request_new_entity(self, user_data):
        # Reserve the entity id
        entity_id = self._next_entity_id
        self._next_entity_id += 1
        self._num_new_entities += 1

        # Construct the entity object
        entity = Entity(entity_id)
        entity.user_data = user_data

        # Insert it into the table
        self._entities[entity_id] = entity

        return entity_id

    def request_destroy_entity(self, entity_id):
        # Make sure the entity id is valid
        if entity_id not in self._entities:
            print("Attempt to destroy invalid entity, {}".format(entity_id))
            return
        entity = self._entities[entity_id]

        # Find all of the entity's children
        children = list()
        for child_entity in self._entities.values():
            if child_entity.parent == entity.id:
                children.append(child_entity.id)

        # Destroy the children
        for child_entity_id in children:
            self.request_destroy_entity(child_entity_id)

        # Destroy all of the components
        for component_type in list(entity.components):
            self.request_destroy_component(entity.id, component_type)

        # Remove the entry from the entity list
        del self._entities[entity_id]

        # Add it to the destroyed entities set
        self._destroyed_entities.add(entity_id)

        # Run the callback
        if self._destroy_entity_callback is not None:
            self._destroy_entity_callback(self, entity)

    def request_new_component(self, entity_id, component_type):
        # Add the component to the entity object
        self._components.setdefault(component_type, ComponentType()).instances[entity_id] = dict()
        self._entities[entity_id].components.add(component_type)

        # Add the component and entity id to the query table
        self._new_component_queries.setdefault(entity_id, set()).add(component_type)
        self.add_get_component_query(entity_id, component_type)

    def request_destroy_component(self, entity_id, component_type):
        component = self._components[component_type]
        entity = self._entities[entity_id]

        # Make sure the component actually exists on the entity
        if component_type not in entity.components:
            return

        # Extract the component value
        component_value = component.instances[entity_id]
        del component.instances[entity_id]
        entity.components.remove(component_type)

        # Add it to the destroyed components set
        self._destroyed_components.setdefault(component_type, set()).add(entity_id)

        # Run the destroy component callback
        if component.destroy_instance_callback is not None:
            component.destroy_instance_callback(self, self._entities[entity_id], component_value)

    def get_entity_userdata(self, entity_id):
        return self._entities[entity_id].user_data

    def get_entity_name(self, entity_id):
        return self._entities[entity_id].name

    def set_entity_name(self, entity_id, name):
        # Set the name on the entity object
        self._entities[entity_id].name = name

        # Add the name to the query table
        self._set_entity_name_queries[entity_id] = name

    def new_entity_callback(self, callback):
        self._new_entity_callback = callback

    def update_entity_callback(self, callback):
        self._update_entity_callback = callback

    def destroy_entity_callback(self, callback):
        self._destroy_entity_callback = callback

    def update_component_callback(self, component_type, callback):
        # Get or set the component type, since this may be called before any queries are run
        component = self._components.setdefault(component_type, ComponentType())
        component.update_instance_callback = callback

    def destroy_component_callback(self, component_type, callback):
        # Get or set the component type, since this may be called before any queries are run
        component = self._components.setdefault(component_type, ComponentType())
        component.destroy_instance_callback = callback

    def add_get_component_query(self, entity_id, component_type):
        self._get_component_queries.setdefault(component_type, set()).add(entity_id)

    def get_property_value(self, entity_id, component_type, property_path, property_name):
        value = self._components[component_type].instances[entity_id]

        # Follow the path to get the property value
        for step in property_path:
            if step in value:
                value = value[step]
            else:
                self.add_get_component_query(entity_id, component_type)
                return None

        if property_name in value:
            return value[property_name]
        else:
            self.add_get_component_query(entity_id, component_type)
            return None

    def set_property_value(self, entity_id, component_type, property_path, property_name, value):
        in_value = self._components[component_type].instances[entity_id]
        out_value = self._set_component_queries.setdefault(component_type, dict()).setdefault(entity_id, dict())

        for step in property_path:
            in_value = in_value.setdefault(step, dict())
            out_value = out_value.setdefault(step, dict())

        in_value[property_name] = value
        out_value[property_name] = value

    def get_components(self, entity_id):
        return self._entities[entity_id].components

    def get_component_value(self, entity_id, component_type):
        return self._components[component_type].instances[entity_id]

    def set_component_value(self, entity_id, component_type, value):
        # Copy the value to the input and output dictionaries
        self._components[component_type].instances[entity_id] = deepcopy(value)
        self._set_component_queries.setdefault(component_type, dict())[entity_id] = deepcopy(value)
