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

    def __init__(self):
        self.id = Entity.NULL_ENTITY_ID
        self.name = ""
        self.parent = Entity.WORLD_ENTITY_ID
        self.components = dict()
        self.user_data = None

class SceneManager(object):
    def __init__(self, new_entity_callback = None):
        self._new_entity_callback = new_entity_callback
        self._next_entity_id = None
        self._entities = dict()
        self._set_entity_name_queries = dict()
        self._set_entity_parent_queries = dict()
        self._get_component_queries = dict()
        self._set_component_queries = dict()
        self._new_component_queries = dict()
        self._get_component_callbacks = dict()
        self._sent_scene_query = False
        self._num_new_entities = 0

    def register_handlers(self, session):
        session.add_query_handler('get_scene', self._get_scene_query)
        session.add_response_handler('get_scene', self._get_scene_response)
        session.add_query_handler('get_component', self._get_component_query)
        session.add_response_handler('get_component', self._get_component_response)
        session.add_query_handler('set_component', self._set_component_query)
        session.add_query_handler('new_component', self._new_component_query)
        session.add_query_handler('new_entity', self._new_entity_query)
        session.add_query_handler('set_entity_name', self._set_entity_name_query)

    def _get_scene_query(self):
        if not self._sent_scene_query:
            self._sent_scene_query = True
            return True # Actual value doesn't matter

    def _get_scene_response(self, response):
        if response is None:
            return

        # Get the next entity id (used for constructing new entities client-side)
        self._next_entity_id = response['next_entity_id']

        # Make sure there are entities in the scene, before attempting to load
        if response['entities'] is None:
            return

        # For each entity in the scene
        for entity_id_str, value in response['entities'].items():
            entity_id = int(entity_id_str)

            # Insert a new entry into the entities table
            self._entities.setdefault(entity_id, Entity())
            entity = self._entities[entity_id]

            # Create the entity
            entity.id = entity_id
            entity.name = value['name']
            entity.parent = value['parent']

            # Create the component type dictionary
            for component_type in value['components']:
                entity.components[component_type] = dict()

            # Run the user callback on the entity
            if self._new_entity_callback is not None:
                self._new_entity_callback(self, entity)

    def _get_component_query(self):
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

    def _get_component_response(self, response):
        if response is None:
            return

        # For each component type in the response
        for component_type, entities in response.items():

            # For each entity in the response
            for entity_id_str, value in entities.items():

                # Get the entity this component belongs to
                entity = self._entities[int(entity_id_str)]

                # Set the component value
                self._entities[entity.id].components[component_type] = value

                # Update the values with the values waiting to be set
                recursive_update(value, self._set_component_queries.get(component_type, dict()).get(entity.id, dict()))

                # If there's a callback for this component type
                if component_type in self._get_component_callbacks:
                    self._get_component_callbacks[component_type](self, entity, value)

    def _set_component_query(self):
        # Make sure there are actually outstanding 'set_component' queries
        if len(self._set_component_queries) == 0:
            return None

        # Create the message
        message = self._set_component_queries
        self._set_component_queries = dict()
        return message

    def _new_entity_query(self):
        if self._num_new_entities == 0:
            return None

        message = self._num_new_entities
        self._num_new_entities = 0
        return message

    def _set_entity_name_query(self):
        if len(self._set_entity_name_queries) == 0:
            return None

        message = self._set_entity_name_queries
        self._set_entity_name_queries = dict()
        return message

    def _new_component_query(self):
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

    def new_entity(self, user_data):
        # Reserve the entity id
        entity_id = self._next_entity_id
        self._next_entity_id += 1
        self._num_new_entities += 1

        # Construct the entity object
        entity = Entity()
        entity.id = entity_id
        entity.user_data = user_data

        # Insert it into the table
        self._entities[entity_id] = entity

        return entity_id

    def new_component(self, entity_id, component_type):
        # Add the component to the entity object
        self._entities[entity_id].components[component_type] = dict()

        # Add the component and entity id to the query table
        self._new_component_queries.setdefault(entity_id, set())
        self._new_component_queries[entity_id].add(component_type)

    def get_entity_userdata(self, entity_id):
        return self._entities[entity_id].user_data

    def get_entity_name(self, entity_id):
        return self._entities[entity_id].name

    def set_entity_name(self, entity_id, name):
        # Set the name on the entity object
        self._entities[entity_id].name = name

        # Add the name to the query table
        self._set_entity_name_queries[entity_id] = name

    def add_component_type_callback(self, component_type, callback):
        self._get_component_callbacks[component_type] = callback

    def add_get_component_query(self, entity_id, component_type):
        self._get_component_queries.setdefault(component_type, set())
        self._get_component_queries[component_type].add(entity_id)

    def get_property_value(self, entity_id, component_type, property_path, property_name):
        value = self._entities[entity_id].components[component_type]

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
        in_value = self._entities[entity_id].components[component_type]
        out_value = self._set_component_queries.setdefault(component_type, dict()).setdefault(entity_id, dict())

        for step in property_path:
            in_value = in_value.setdefault(step, dict())
            out_value = out_value.setdefault(step, dict())

        in_value[property_name] = value
        out_value[property_name] = value

    def get_components(self, entity_id):
        return self._entities[entity_id].components.keys()

    def get_component_value(self, entity_id, component_type):
        return self._entities[entity_id].components[component_type]

    def set_component_value(self, entity_id, component_type, value):
        # Copy the value to the input and output dictionaries
        self._entities[entity_id].components[component_type] = deepcopy(value)
        self._set_component_queries.setdefault(component_type, dict())[entity_id] = deepcopy(value)
