# scene_manager.py

# Recursively updates a dictionary D with E.
# This assumes that E has a structure that is a subset of D
def recursive_update(D, E):
    for k, v in E.items():
        if isinstance(v, dict):
            recursive_update(D[k], v)
        else:
            D[k] = v

class SceneManager(object):
    def __init__(self, new_entity_callback):
        self.new_entity_callback = new_entity_callback
        self.entities = {}
        self.get_component_queries = {}
        self.set_component_queries = {}
        self.get_component_callbacks = {}
        self.user_entity_data = {}

    def register_handlers(self, session):
        session.add_query_handler('get_scene', self.get_scene_query)
        session.add_response_handler('get_scene', self.get_scene_response)
        session.add_query_handler('get_component', self.get_component_query)
        session.add_response_handler('get_component', self.get_component_response)
        session.add_query_handler('set_component', self.set_component_query)

    def get_scene_query(self):
        if len(self.entities) == 0:
            return True # Actual value doesn't matter

    def get_scene_response(self, response):
        if response is None:
            return

        for entity_id_str, value in response.items():
            component_dict = {}
            entity_dict = {
                'name': value['name'],
                'parent': value['parent'],
                'components': component_dict,
            }

            # Create the component type dictionary
            for component_type in value['components']:
                component_dict[component_type] = {}

            entity_id = int(entity_id_str)
            self.entities[entity_id] = entity_dict

            if entity_id not in self.user_entity_data:
                self.user_entity_data[entity_id] = self.new_entity_callback(self, entity_id, value['name'], value['components'])

    def get_component_query(self):
        if len(self.get_component_queries) == 0:
            return None

        message = {}
        for component_type, entities in self.get_component_queries.items():
            message[component_type] = list(entities)

        self.get_component_queries = {}
        return message

    def get_component_response(self, response):
        if response is None:
            return

        for component_type, entities in response.items():
            for entity_id_str, value in entities.items():
                entity_id = int(entity_id_str)
                self.entities[entity_id]['components'][component_type] = value

                # Update the get values with the set values
                recursive_update(value, self.set_component_queries.get(component_type, {}).get(entity_id, {}))

                # If there's a callback for this component type
                if component_type in self.get_component_callbacks:
                    self.get_component_callbacks[component_type](self, entity_id, value)

    def set_component_query(self):
        if len(self.set_component_queries) != 0:
            message = self.set_component_queries
            self.set_component_queries = {}
            return message

    def add_get_component_callback(self, component_type, callback):
        self.get_component_callbacks[component_type] = callback

    def add_get_component_query(self, entity_id, component_type):
        if component_type in self.get_component_queries:
            self.get_component_queries[component_type].add(entity_id)
        else:
            self.get_component_queries[component_type] = set([entity_id])

    def get_property_value(self, entity_id, component_type, property_path, property_name):
        value = self.entities[entity_id]['components'][component_type]

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
        in_value = self.entities[entity_id]['components'][component_type]
        out_value = self.set_component_queries.setdefault(component_type, {}).setdefault(entity_id, {})

        for step in property_path:
            in_value = in_value.setdefault(step, {})
            out_value = out_value.setdefault(step, {})

        in_value[property_name] = value
        out_value[property_name] = value
