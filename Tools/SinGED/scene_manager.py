# scene_manager.py

from copy import deepcopy
from editor_session import EditorSession


# Recursively updates a dictionary D with E.
# This assumes that E has a structure that is a subset of D
def recursive_update(d, e):
    for k, v in e.items():
        if isinstance(v, dict):
            recursive_update(d[k], v)
        else:
            d[k] = v


class NodeId(object):
    NULL_ID = 0

    @staticmethod
    def from_str(node_id_str):
        result = NodeId()
        result._id = int(node_id_str)
        return result

    def __init__(self, id_v=NULL_ID):
        self._id = id_v

    def is_null(self):
        return self._id == NodeId.NULL_ID

    # Returns if this NodeId is a real (server-valid) id
    def is_real(self):
        return self._id >= NodeId.NULL_ID

    def set(self, id_v, force=False):
        # If the Id isn't a real id, and the caller hasn't forced a set
        if id_v < NodeId.NULL_ID and not force:
            return

        self._id = id_v

    def get(self):
        return self._id

    def __eq__(self, other):
        return self._id == other.get()

    def __ne__(self, other):
        return self._id != other.get()

    def __hash__(self):
        return hash(self._id)

    def __str__(self):
        return str(self._id)


class Node(object):
    UPDATE_ID_ALLOCATED = 'UPDATE_ID_ALLOCATED'
    UPDATE_ROOT_CHANGED = 'UPDATE_ROOT_CHANGED'
    UPDATE_NAME_CHANGED = 'UPDATE_NAME_CHANGED'
    UPDATE_LOCAL_TRANSFORM_CHANGED = 'UPDATE_LOCAL_TRANSFORM_CHANGED'
    UPDATE_NEW_COMPONENT = 'UPDATE_NEW_COMPONENT'
    UPDATE_DESTROYED_COMPONENT = 'UPDATE_DESTROYED_COMPONENT'

    def __init__(self):
        self.id = NodeId()
        self.root = NodeId()
        self.name = ""
        self.local_position = [0.0, 0.0, 0.0]
        self.local_scale = [1.0, 1.0, 1.0]
        self.local_rotation = [0.0, 0.0, 0.0, 1.0]
        self.components = set()
        self.user_data = None


class ComponentType(object):
    def __init__(self, name):
        self.name = name
        self.instances = dict()
        self.new_instance_callback = None
        self.update_instance_callback = None
        self.destroy_instance_callback = None


class SceneManager(object):
    def __init__(self):
        self._next_fake_node_id = -1
        self._new_node_callback = None
        self._update_node_callback = None
        self._destroy_node_callback = None
        self._nodes = dict()
        self._unsent_new_nodes = dict()
        self._sent_new_nodes = dict()
        self._destroyed_nodes = set()
        self._node_changed_roots = set()
        self._node_changed_names = set()
        self._node_changed_local_transforms = set()
        self._new_component_queries = dict()
        self._components = dict()
        self._destroyed_components = dict()
        self._get_component_queries = dict()
        self._set_component_queries = dict()
        self._sent_scene_query = False
        self._save_scene_path = ''
        self._generate_lightmaps_query = False

    def register_handlers(self, session):
        session.add_query_handler('get_scene', self._get_scene_query)
        session.add_response_handler('get_scene', self._get_scene_response)
        #session.add_query_handler('new_node', self._new_node_query)
        #session.add_response_handler('new_node', self._new_node_response)
        #session.add_query_handler('destroy_node', self._destroy_node_query)
        #session.add_response_handler('destroy_node', self._destroy_node_response)
        #session.add_query_handler('node_root_change', self._node_root_change_query)
        #session.add_response_handler('node_root_change', self._node_root_change_response)
        #session.add_query_handler('node_name_change', self._node_name_change_query)
        #session.add_response_handler('node_name_change', self._node_name_change_response)
        #session.add_query_handler('node_local_transform_change', self._node_local_transform_change_query)
        #session.add_response_handler('node_local_transform_change', self._node_local_transform_change_response)
        #session.add_query_handler('new_component', self._new_component_query)
        #session.add_response_handler('new_component', self._new_component_response)
        #session.add_query_handler('destroy_component', self._destroy_component_query)
        #session.add_response_handler('destroy_component', self._destroy_component_response)
        session.add_query_handler('get_component', self._get_component_query)
        session.add_response_handler('get_component', self._get_component_response)
        #session.add_query_handler('set_component', self._set_component_query)
        #session.add_response_handler('set_component', self._set_component_response)
        #session.add_query_handler('save_scene', self._save_scene_query)
        #session.add_query_handler('gen_lightmaps', self._gen_lightmaps_query)

    def _get_scene_query(self, seq_number, priority):
        # Unused arguments
        del seq_number, priority

        if not self._sent_scene_query:
            self._sent_scene_query = True
            return True # Actual value doesn't matter

    def _get_scene_response(self, seq_number, response):
        # Unused arguments
        del seq_number

        if response is None:
            return

        # Store all new nodes
        new_nodes = set()

        # For each node in the scene
        for node_id_str, value in response['nodes'].items():
            node_id = NodeId.from_str(node_id_str)

            # Insert a new entry into the nodes table
            node = Node()
            node.id = node_id
            self._nodes[node_id] = node

            # Initialize the node
            node.name = value['name']
            node.root = NodeId(value['root'])
            node.local_position = value['lpos']
            node.local_rotation = value['lrot']
            node.local_scale = value['lscale']

            # Add the entity to the list of newly created entities
            new_nodes.add(node)

        # For each component type
        for component_type_name, instances in response['components'].items():
            component_type = self._components.setdefault(component_type_name, ComponentType(component_type_name))

            if instances in None:
                continue

            # For each instance of this component type
            for node_id_num in instances:
                node_id = NodeId(node_id_num)
                assert(node_id in self._nodes)
                node = self._nodes[node_id]

                # Add the component value dictionary
                component_type.instances[node_id] = dict()

                # Add the component to the node
                node.components.add(component_type)

        # Run the 'new_node' callback on all new nodes
        if self._update_node_callback is not None:
            for node in new_nodes:
                self._update_node_callback(self, node)

    def _new_node_query(self, seq_number, priority):
        # Unused arguments
        del priority

        if len(self._unsent_new_nodes) == 0:
            return None

        message = dict()
        for fake_id, node in self._unsent_new_nodes:
            # Set all of the properties of the node, except for root
            node_dict = message[fake_id] = dict()
            node_dict['name'] = node.name
            node_dict['lpos'] = node.local_position.copy()
            node_dict['lscale'] = node.local_scale.copy()
            node_dict['lrot'] = node.local_rotation.copy()

        # Reset the table of unsent nodes
        self._sent_new_nodes[seq_number] = self._unsent_new_nodes
        self._unsent_new_nodes = dict()
        return message

    def _new_node_response(self, seq_number, response):
        # Check if these nodes correspond to nodes that we requested
        if seq_number not in self._sent_new_nodes:
            # Create new nodes
            new_nodes = list()
            for node_response in response.values():
                node = Node()
                node.id = NodeId(node_response['id'])
                node.root = NodeId(node_response['root'])
                node.name = node_response['name']
                node.local_position = node_response['lpos']
                node.local_rotation = node_response['lrot']
                node.local_scale = node_response['lscale']
                self._nodes[node.id] = node
                new_nodes.append(node)

            # Call 'new_node' on all created nodes
            if self._new_node_callback is not None:
                for new_node in new_nodes:
                    self._new_node_callback(self, new_node)
            return

        # Get the nodes that were supposed to go with this sequence number
        pending_nodes = self._sent_new_nodes[seq_number]
        assert(len(pending_nodes) == len(response))

        for fake_id_str, node_response in response.items():
            fake_id = NodeId.from_str(fake_id_str)
            node = pending_nodes[fake_id]

            # Update properties
            node.id = NodeId(node_response['id'])
            node.name = node_response['name']
            node.local_position = node_response['lpos']
            node.local_rotation = node_response['lrot']
            node.local_scale = node_response['lscale']

        # Call the update function on nodes
        if self._update_node_callback is not None:
            for node in pending_nodes.values():
                self._update_node_callback(self, node)

        # Remove the pending nodes
        del self._sent_new_nodes[seq_number]

    def _destroy_node_query(self, seq_number, priority):
        # Unused arguments
        del seq_number, priority

        if len(self._destroyed_nodes) == 0:
            return None

        message = list(self._destroyed_nodes)
        self._destroyed_nodes.clear()
        return message

    def _destroy_node_response(self, seq_number, response):
        # Unused arguments
        del seq_number

        for node_id_num in response:
            node_id = NodeId(node_id_num)
            if node_id in self._nodes:
                # Run the deletion callback, if it exists
                if self._destroy_node_callback is not None:
                    self._destroy_node_callback(self, self._nodes[node_id])

                # Delete the node
                del self._nodes[node_id]

    def _node_root_change_query(self, seq_number, priority):
        # Unused arguments
        del seq_number, priority

        if len(self._node_changed_roots) == 0:
            return None

        message = dict()
        for changed_root_node_id in self._node_changed_roots:
            assert(changed_root_node_id in self._nodes)
            node = self._nodes[changed_root_node_id]
            message[changed_root_node_id] = node.root

        self._node_changed_roots.clear()
        return message

    def _node_root_change_response(self, seq_number, response):
        # Unused arguments
        del seq_number

        updated_nodes = list()

        for node_id_str, root_id_num in response.items():
            node_id = NodeId.from_str(node_id_str)
            root_id = NodeId(root_id_num)
            assert(node_id in self._nodes and root_id in self._nodes)

            # Update the node
            node = self._nodes[node_id]
            if node.root != root_id:
                node.root = root_id
                updated_nodes.append(node)

        # Call the update callback, if any
        if self._update_node_callback is not None:
            for node in updated_nodes:
                self._update_node_callback(node)

    def _node_name_change_query(self, seq_number, priority):
        # Unused arguments
        del seq_number, priority

        if len(self._set_node_name_queries) == 0:
            return None

        message = self._set_node_name_queries
        self._set_node_name_queries = dict()
        return message

    def _node_name_change_response(self, seq_number, response):
        # Unused arguments
        del seq_number

        updated_nodes = list()

        for node_id_str, name in response.items():
            node_id = NodeId.from_str(node_id_str)
            node = self._nodes[node_id]
            if node.name != name:
                node.name = name
                updated_nodes.append(node)

        # Call the user callback
        if self._update_node_callback is not None:
            for node in updated_nodes:
                self._update_node_callback(self, node)

    def _node_local_transform_change_query(self, seq_number, priority):
        # Unused arguments
        del seq_number

        # Setting the transform is not a high priority update
        if priority != EditorSession.PRIORITY_ANY:
            return None

        if len(self._node_changed_local_transforms) == 0:
            return None

        message = dict()
        for node_id in self._node_changed_local_transforms:
            node = self._nodes[node_id]
            entry = message[node_id] = dict()
            entry['lpos'] = node.local_position.copy()
            entry['lrot'] = node.local_rotation.copy()
            entry['lscale'] = node.local_scale.copy()

        self._node_changed_local_transforms.clear()
        return message

    def _node_local_transform_change_response(self, seq_number, response):
        # If we were responsible for the query that caused this
        if seq_number != EditorSession.NULL_SEQUENCE_NUMBER:
            return

        # For each transformed node
        for node_id_str, trans in response.items():
            node_id = NodeId.from_str(node_id_str)
            node = self._nodes[node_id]
            node.local_position = trans['lpos']
            node.local_rotation = trans['lrot']
            node.local_scale = trans['lscale']

            # Call the update callback
            if self._update_node_callback is not None:
                self._update_node_callback(self, node)

    def _new_component_query(self, seq_number, priority):
        # Unused arguments
        del seq_number, priority

        # Make sure there are actually outstanding 'new_component' queries
        if len(self._new_component_queries) == 0:
            return None

        # Construct the message
        message = dict()
        for node_id, components in self._new_component_queries.items():
            message[node_id] = list(components)

        # Reset the 'new_component' query table
        self._new_component_queries.clear()
        return message

    def _new_component_response(self, seq_number, response):
        # Unused arguments
        del seq_number

        for component_type_name, instances in response.items():
            # Get the component type object
            component_type = self._components[component_type_name]

            # For each newly created instance
            for node_id_num in instances:
                node_id = NodeId(node_id_num)
                node = self._nodes[node_id]
                node.components.append(component_type)
                component_type.instances[node_id] = dict()

    def _destroy_component_query(self, seq_number, priority):
        # Unused arguments
        del seq_number, priority

        if len(self._destroyed_components) == 0:
            return None

        # Create the message
        message = dict()
        for component_type, instances in self._destroyed_components.items():
            message[component_type] = list(instances)

        self._destroyed_components.clear()
        return message

    def _destroy_component_response(self, seq_number, response):
        # Unused arguments
        del seq_number

        for component_type_name, instances in response.items():
            component_type = self._components[component_type_name]
            for node_id_num in instances:
                node_id = NodeId(node_id_num)

                # Remove the instance id from the component type's set
                component_type.instances.remove(node_id)

                # Remove the component type from the node's component set
                if node_id in self._nodes:
                    node = self._nodes[node_id]
                    node.components.remove(component_type)

                # Run the user callback
                if component_type.destroyed_instance_callback is not None:
                    component_type.destroyed_instance_callback(self, component_type, node_id)

    def _get_component_query(self, seq_number, priority):
        # Unused arguments
        del seq_number, priority

        # If there are no get component queries, just skip this
        if len(self._get_component_queries) == 0:
            return None

        # Create a message object
        message = dict()

        # For each entry in the table of 'get_component' queries
        for component_type, instances in self._get_component_queries.items():
            message[component_type] = [node_id.get() for node_id in instances]

        # Restore the get_component structure
        self._get_component_queries.clear()
        return message

    def _get_component_response(self, seq_number, response):
        # Unused arguments
        del seq_number

        if response is None:
            return

        # For each component type in the response
        for component_type_name, instances in response.items():

            # For each node in the response
            for node_id_str, component_value in instances.items():
                node_id = NodeId.from_str(node_id_str)

                # Get the component
                component_type = self._components[component_type_name]

                # Set the component value
                assert(node_id in component_type.instances)
                component_type.instances[node_id] = component_value

                # Update the values with the values waiting to be set
                recursive_update(
                    component_value,
                    self._set_component_queries.get(component_type, dict()).get(node_id, dict()))

                # If there's a callback for this component type
                if component_type.update_instance_callback is not None:
                    node = self._nodes[node_id]
                    component_type.update_instance_callback(self, component_type, node, component_value)

    def _set_component_query(self, seq_number, priority):
        # Unused arguments
        del seq_number, priority

        # Make sure there are actually outstanding 'set_component' queries
        if len(self._set_component_queries) == 0:
            return None

        # Create the message
        message = self._set_component_queries
        self._set_component_queries = dict()
        return message

    def _set_component_response(self, seq_number, response):
        # Pass it off to the get component response handler, for now
        self._get_component_response(seq_number, response)

    def _save_scene_query(self, seq_number, priority):
        # Unused arguments
        del seq_number, priority

        if len(self._save_scene_path) == 0:
            return None

        message = {
            'path': self._save_scene_path,
        }
        self._save_scene_path = ''
        return message

    def _gen_lightmaps_query(self, seq_number, priority):
        # Unused arguments
        del seq_number, priority

        if not self._generate_lightmaps_query:
            return None

        # Actual value doesn't matter
        self._generate_lightmaps_query = False
        return True

    def save_scene(self, path):
        self._save_scene_path = path

    def generate_lightmaps(self):
        self._generate_lightmaps_query = True

    def request_new_node(self, user_data):
        # Reserve fake node id
        fake_id = NodeId(self._next_fake_node_id)
        self._next_fake_node_id -= 1

        # Construct the node object
        node = Node()
        node.id = fake_id
        node.user_data = user_data

        # Insert it into the table
        self._nodes[fake_id] = node

        return fake_id

    def request_destroy_node(self, node_id):
        # Make sure the node id is valid
        if node_id not in self._nodes:
            print("Attempt to destroy invalid node, {}".format(node_id))
            return
        node = self._nodes[node_id]

        # Find all of the nodes's children
        children = list()
        for child_node in self._nodes.values():
            if child_node.root == node_id:
                children.append(child_node.id)

        # Destroy the children
        for child_node_id in children:
            self.request_destroy_node(child_node_id)

        # Destroy all of the components
        for component_type in list(node.components):
            self.request_destroy_component(node.id, component_type)

        # Remove the node from the node dictionary
        del self._nodes[node_id]

        # Add it to the destroyed nodes set
        self._destroyed_nodes.add(node_id)

        # Run the callback
        if self._destroy_node_callback is not None:
            self._destroy_node_callback(self, node)

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
