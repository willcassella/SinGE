# scene_manager.py

from copy import deepcopy
from . import editor_session


# Recursively updates a dictionary a with b.
# This assumes that b has a structure that is a subset of a
# Returns whether the dictionary a was modified.
def recursive_update(a, b):
    modified = False
    for k, v in b.items():
        if isinstance(v, dict):
            modified = recursive_update(a[k], v) or modified
        else:
            modified = a[k] != v or modified
            a[k] = v
    return modified


class Node(object):
    NULL_ID = 0

    def __init__(self):
        self.id = None
        self.fake_id = None
        self.root = None
        self.name = ""
        self.local_position = [0.0, 0.0, 0.0]
        self.local_scale = [1.0, 1.0, 1.0]
        self.local_rotation = [0.0, 0.0, 0.0, 1.0]
        self.user_data = None
        self.destroyed = False

    def get_root_id(self):
        return self.root.id if self.root is not None else Node.NULL_ID

    def is_real(self):
        return self.id > Node.NULL_ID

    def is_fake(self):
        return self.id < Node.NULL_ID


class ComponentType(object):
    def __init__(self, type_name):
        self.type_name = type_name
        self.instances = dict()
        self.new_instances = set()
        self.changed_instances = set()
        self.destroyed_instances = set()
        self.new_instance_callback = None
        self.update_instance_callback = None
        self.destroy_instance_callback = None

    def get_instance(self, node):
        return self.instances.get(node, None)

    def set_new_instance_callback(self, callback):
        self.new_instance_callback = callback

    def set_update_instance_callback(self, callback):
        self.update_instance_callback = callback

    def set_destroy_instance_callback(self, callback):
        self.destroy_instance_callback = callback

    def request_new_instance(self, node):
        assert(not node.destroyed and node not in self.instances)

        instance = ComponentInstance(node, self)
        self.instances[node] = instance
        self.new_instances.add(instance)

        if self.new_instance_callback is not None:
            self.new_instance_callback(instance)

        return instance

    def request_destroy_instance(self, node):
        if node not in self.instances:
            return

        # Get the instance
        instance = self.instances[node]
        if instance.destroyed:
            return

        # Destroy the instance
        instance.destroyed = True
        self.destroyed_instances.add(instance)

        # If the user callback exists, run it
        if self.destroy_instance_callback is not None:
            self.destroy_instance_callback(instance)


class ComponentInstance(object):
    def __init__(self, node, type_v):
        self.type = type_v
        self.node = node
        self.destroyed = False
        self.is_loaded = False
        self.value = None
        self.changed_props = dict()
        self.loaded_callbacks = list()

    def _set_property(self, prop_name, value):
        old_value = self.value[prop_name]
        if isinstance(value, dict):
            assert(isinstance(old_value, dict))
            changed = recursive_update(old_value, value)
        else:
            changed = old_value != value
            self.value[prop_name] = value
        return changed

    def get_value_immediate(self, default=None):
        if not self.is_loaded:
            return default
        return self.value

    def get_value_async(self, callback):
        if not self.is_loaded:
            self.loaded_callbacks.append(lambda instance: callback(instance.value))
            return
        callback(self.value)

    def set_value(self, value):
        for prop_name, prop_val in value.items():
            self.set_property(prop_name, prop_val)

    def server_set_value(self, seq_num, value):
        modified = False
        for prop_name, prop_val in value.items():
            # If this property was not expected to be changed, or it's the final change
            if seq_num == 0 or prop_name not in self.changed_props or self.changed_props[prop_name] == seq_num:
                modified = self._set_property(prop_name, prop_val) or modified
            # Remove it from the change table
            self.changed_props.pop(prop_name, None)
        return modified

    def get_property_immediate(self, prop_name, default=None):
        if not self.is_loaded:
            return default
        return self.value[prop_name]

    def get_sub_property_immediate(self, prop_path, default=None):
        if not self.is_loaded:
            return default
        value = self.value
        for prop_name in prop_path:
            value = value[prop_name]
        return value

    def get_property_async(self, prop_name, callback):
        if not self.is_loaded:
            self.loaded_callbacks.append(lambda instance: callback(instance.value[prop_name]))
            return
        callback(self.value[prop_name])

    def set_property(self, prop_name, value):
        if not self.is_loaded:
            value = deepcopy(value)
            self.loaded_callbacks.append(lambda instance: instance.set_property(prop_name, value))
            return

        changed = self._set_property(prop_name, value)
        if changed:
            self.changed_props[prop_name] = None
            self.type.changed_instances.add(self)

            # Run the modified callback
            if self.type.update_instance_callback is not None:
                self.type.update_instance_callback(self)

    def set_sub_property_immediate(self, prop_path, value):
        if not self.is_loaded:
            return False
            return

        outer_prop_name = prop_path[0]
        inner_prop_name = prop_path[-1]

        old_value = self.value
        for prop_name in prop_path[:-1]:
            old_value = old_value[prop_name]

        modified = recursive_update(old_value, {inner_prop_name: value})

        if modified:
            self.changed_props[outer_prop_name] = None
            self.type.changed_instances.add(self)

            # Run the update callback
            if self.type.update_instance_callback is not None:
                self.type.update_instance_callback(self)
        return True


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
        self._node_changed_roots = dict()
        self._node_changed_names = dict()
        self._node_changed_local_transforms = dict()
        self._new_components = dict()
        self._components = dict()
        self._sent_scene_query = False
        self._save_scene_path = ''
        self._generate_lightmaps_query = None
        self._lightmaps_generated_callback = None

    def register_handlers(self, session):
        session.add_query_handler('get_scene', self._get_scene_query)
        session.add_response_handler('get_scene', self._get_scene_response)
        session.add_query_handler('new_node', self._new_node_query)
        session.add_response_handler('new_node', self._new_node_response)
        session.add_query_handler('destroy_node', self._destroy_node_query)
        session.add_response_handler('destroy_node', self._destroy_node_response)
        session.add_query_handler('node_root_update', self._node_root_update_query)
        session.add_response_handler('node_root_update', self._node_root_update_response)
        session.add_query_handler('node_name_update', self._node_name_update_query)
        session.add_response_handler('node_name_update', self._node_name_update_response)
        session.add_query_handler('node_local_transform_update', self._node_local_transform_update_query)
        session.add_response_handler('node_local_transform_update', self._node_local_transform_update_response)
        session.add_query_handler('new_component', self._new_component_query)
        session.add_response_handler('new_component', self._new_component_response)
        session.add_query_handler('destroy_component', self._destroy_component_query)
        session.add_response_handler('destroy_component', self._destroy_component_response)
        session.add_query_handler('component_property_update', self._component_property_update_query)
        session.add_response_handler('component_property_update', self._component_property_update_response)
        session.add_query_handler('save_scene', self._save_scene_query)
        session.add_query_handler('gen_lightmaps', self._gen_lightmaps_query)
        session.add_response_handler('gen_lightmaps', self._gen_lightmaps_response)

    def _get_scene_query(self, seq_number, priority):
        # Unused arguments
        del seq_number, priority

        if not self._sent_scene_query:
            self._sent_scene_query = True
            return True  # Actual value doesn't matter

    def _get_scene_response(self, seq_number, response):
        # Unused arguments
        del seq_number

        if response is None:
            return

        # Store all new nodes
        new_nodes = set()
        root_ids = dict()
        new_components = dict()

        # For each node in the scene
        for node_id_str, value in response['nodes'].items():
            node_id = int(node_id_str)

            # Insert a new entry into the nodes table
            node = Node()
            node.id = node_id
            self._nodes[node_id] = node
            root_ids[node] = value['root']

            # Initialize the node
            node.name = value['name']
            node.local_position = value['lpos']
            node.local_rotation = value['lrot']
            node.local_scale = value['lscale']

            # Add the node to the list of newly created nodes
            new_nodes.add(node)

        # Add nodes to roots
        for node, root_id in root_ids.items():
            node.root = self.get_node(root_id)

        # For each component type
        for component_type_name, instances in response['components'].items():
            component_type = self._components.setdefault(component_type_name, ComponentType(component_type_name))

            # Stupid serialization system corner case
            if instances is None:
                continue

            new_instances = list()
            # For each instance of this component type
            for node_id_str, value in instances.items():
                node_id = int(node_id_str)
                node = self._nodes[node_id]

                # Add the component instance object
                instance = ComponentInstance(node, component_type)
                component_type.instances[node] = instance
                instance.value = value
                instance.is_loaded = True
                instance.loaded_callbacks = None
                new_instances.append(instance)

            if component_type.new_instance_callback is not None:
                new_components[component_type] = new_instances

        # Run the 'new_node' callback on all new nodes
        if self._new_node_callback is not None:
            for node in new_nodes:
                self._new_node_callback(self, node)

        # Run the 'update_node' callback on all new nodes
        if self._update_node_callback is not None:
            for node in new_nodes:
                self._update_node_callback(self, node)

        # Run the 'new_instance' callback on all components
        for component_type, instances in new_components.items():
            for instance in instances:
                component_type.new_instance_callback(instance)

    def _new_node_query(self, seq_number, priority):
        # Unused arguments
        del priority

        if len(self._unsent_new_nodes) == 0:
            return None

        message = dict()
        for fake_id, node in self._unsent_new_nodes.items():
            if node.destroyed:
                continue

            # Only send fake id and name, other properties will be updated later
            node_dict = message[fake_id] = dict()
            node_dict['name'] = node.name

        # Reset the table of unsent nodes
        self._sent_new_nodes[seq_number] = self._unsent_new_nodes
        self._unsent_new_nodes = dict()
        return message

    def _new_node_response(self, seq_number, response):
        # Check if these nodes correspond to nodes that we requested
        if seq_number not in self._sent_new_nodes:
            # Create new nodes
            new_nodes = list()
            root_ids = dict()

            for node_response in response.values():
                node = Node()
                node.id = node_response['id']
                node.name = node_response['name']
                node.local_position = node_response.get('lpos', [0.0, 0.0, 0.0])
                node.local_rotation = node_response.get('lrot', [0.0, 0.0, 0.0, 1.0])
                node.local_scale = node_response.get('lscale', [1.0, 1.0, 1.0])
                root_ids[node] = node_response.get('root', Node.NULL_ID)
                self._nodes[node.id] = node
                new_nodes.append(node)
                print("Received unrequested new node, id={}".format(node.id))

            # Set node roots
            for node, root_id in root_ids.items():
                node.root = self.get_node(root_id)

            # Call 'new_node' on all created nodes
            if self._new_node_callback is not None:
                for new_node in new_nodes:
                    self._new_node_callback(self, new_node)

            # Call 'update_node' on all created nodes
            if self._update_node_callback is not None:
                for new_node in new_nodes:
                    self._update_node_callback(self, new_node)
            return

        # Get the nodes that were supposed to go with this sequence number
        pending_nodes = self._sent_new_nodes[seq_number]
        del self._sent_new_nodes[seq_number]
        assert(len(pending_nodes) == len(response))

        updated_nodes = list()
        for fake_id_str, node_response in response.items():
            fake_id = int(fake_id_str)
            node = pending_nodes[fake_id]

            # Apply Id
            node.id = node_response['id']
            self._nodes[node.id] = node

            # If the node has been destroyed, don't add it to be updated
            if node.destroyed:
                continue

            updated_nodes.append(node)
            print("Allocated node id {} for fake node {}".format(node.id, node.fake_id))

        # Call the update function on updated nodes
        if self._update_node_callback is not None:
            for node in updated_nodes:
                self._update_node_callback(self, node)

    def _destroy_node_query(self, seq_number, priority):
        # Unused arguments
        del seq_number, priority

        if len(self._destroyed_nodes) == 0:
            return None

        message = list()
        remaining = set()
        for node in self._destroyed_nodes:

            # If the node isn't real yet (so they created it and then immediately destroyed it), don't destroy it yet
            if node.is_fake():
                remaining.add(node)
                continue

            message.append(node.id)

        self._destroyed_nodes = remaining
        return message

    def _destroy_node_response(self, seq_number, response):
        # Unused arguments
        del seq_number

        destroyed_nodes = list()

        # Figure out which ones haven't actually been destroyed yet
        for node_id in response:
            if node_id in self._nodes:

                # Destroy the node
                node = self._nodes[node_id]
                destroyed_nodes.append(node)

        # Destroy them
        for node in destroyed_nodes:
            self.request_destroy_node(node)

    def _node_root_update_query(self, seq_number, priority):
        # Unused arguments
        del priority

        if len(self._node_changed_roots) == 0:
            return None

        message = dict()
        for node, existing_seq_num in list(self._node_changed_roots.items()):
            # If the node was destroyed, remove it from the list and continue
            if node.destroyed:
                del self._node_changed_roots[node]
                continue

            # If this node is fake, don't add it to the query yet
            if node.is_fake():
                continue

            # If this message has already been sent out skip it
            if existing_seq_num is not None:
                continue

            # If this node's root is null, add it to the query
            if node.root is None:
                message[node.id] = Node.NULL_ID
                self._node_changed_roots[node] = seq_number
                continue

            # If this node's root is fake, don't add it to the query yet
            if node.root.is_fake():
                continue

            # Otherwise, add it to the message
            message[node.id] = node.root.id
            self._node_changed_roots[node] = seq_number

        if len(message) == 0:
            return None
        return message

    def _node_root_update_response(self, seq_number, response):
        updated_nodes = list()

        # For each node and root in the response
        for node_id_str, root_id in response.items():
            node_id = int(node_id_str)
            node = self._nodes[node_id]

            # If this node's root was not expected to be changed, or the change is final
            if seq_number == 0 or node not in self._node_changed_roots or self._node_changed_roots[node] == seq_number:
                # If the new root is different than the old
                if node.get_root_id() != root_id:
                    node.root = self.get_node(root_id)
                    updated_nodes.append(node)

                # Remove it from the changed root table
                self._node_changed_roots.pop(node, None)

        # Call the update callback, if any
        if self._update_node_callback is not None:
            for node in updated_nodes:
                self._update_node_callback(node)

    def _node_name_update_query(self, seq_number, priority):
        # Unused parameters
        del priority

        if len(self._node_changed_names) == 0:
            return None

        message = dict()
        for node, existing_seq_num in list(self._node_changed_names.items()):
            # If the node was destroyed, remove it from the table and continue
            if node.destroyed:
                del self._node_changed_names[node]
                continue

            # If the node is fake, don't add it yet
            if node.is_fake():
                continue

            # If the node's query hasn't been responded to yet, ignore it
            if existing_seq_num is not None:
                continue

            # Add it to the query
            message[node.id] = node.name
            self._node_changed_names[node] = seq_number

        if len(message) == 0:
            return None
        return message

    def _node_name_update_response(self, seq_number, response):
        updated_nodes = list()

        # For each node and name in the response
        for node_id_str, name in response.items():
            node_id = int(node_id_str)
            node = self._nodes[node_id]

            # If the node's name was not expected to be changed, or the change is final
            if seq_number == 0 or node not in self._node_changed_names or self._node_changed_names[node] == seq_number:
                # If the new name is different from the old one
                if node.name != name:
                    node.name = name
                    updated_nodes.append(node)

                # Remove it from the changed table
                self._node_changed_names.pop(node, None)

        # Call the user callback on all updated nodes
        if self._update_node_callback is not None:
            for node in updated_nodes:
                self._update_node_callback(self, node)

    def _node_local_transform_update_query(self, seq_number, priority):
        # Setting the transform is not a high priority update
        if priority != editor_session.EditorSession.PRIORITY_ANY:
            return None

        if len(self._node_changed_local_transforms) == 0:
            return None

        message = dict()
        for node, existing_seq_num in list(self._node_changed_local_transforms.items()):
            # If the node was destroyed, remove it and continue
            if node.destroyed:
                del self._node_changed_local_transforms[node]
                continue

            # If the node is fake, don't add it yet
            if node.is_fake():
                continue

            # If the node is in the table for a previously sent query
            if existing_seq_num is not None:
                continue

            # Add it to the query
            entry = message[node.id] = dict()
            entry['lpos'] = node.local_position.copy()
            entry['lrot'] = node.local_rotation.copy()
            entry['lscale'] = node.local_scale.copy()
            self._node_changed_local_transforms[node] = seq_number

        if len(message) == 0:
            return None
        return message

    def _node_local_transform_update_response(self, seq_number, response):
        updated_nodes = list()
        # For each transformed node, and it's new transform
        for node_id_str, trans in response.items():
            node_id = int(node_id_str)
            node = self._nodes[node_id]

            # If the node's name was not expected to be changed, or the change is final
            if seq_number == 0 \
                    or node not in self._node_changed_local_transforms \
                    or self._node_changed_local_transforms[node] == seq_number:
                # If the new transform is different than the old one
                different = node.local_position != trans['lpos']
                different = different or node.local_scale != trans['lscale']
                different = different or node.local_rotation != trans['lrot']

                # Update the node
                if different:
                    node.local_position = trans['lpos']
                    node.local_scale = trans['lscale']
                    node.local_rotation = trans['lrot']
                    updated_nodes.append(node)

                # Remove it from the change table
                self._node_changed_local_transforms.pop(node, None)

        # Call the update callback
        if self._update_node_callback is not None:
            for node in updated_nodes:
                self._update_node_callback(self, node)

    def _new_component_query(self, seq_number, priority):
        # Unused arguments
        del seq_number

        if priority != editor_session.EditorSession.PRIORITY_ANY:
            return

        # Construct the message
        message = dict()

        # For each component type
        for component_type_name, component_type in self._components.items():
            remaining = set()
            new_instances = list()

            for instance in component_type.new_instances:
                # If the node was destroyed, don't add it
                if instance.node.destroyed:
                    continue

                # If the node is fake, don't add it YET
                if instance.node.is_fake():
                    remaining.add(instance)
                    continue

                # Add it to the message
                new_instances.append(instance.node.id)

            # Reset the new instance set
            component_type.new_instances = remaining

            # Add it to the message only if new components were actually created
            if len(new_instances) == 0:
                continue
            message[component_type_name] = new_instances

        if len(message) == 0:
            return None
        return message

    def _new_component_response(self, seq_number, response):
        # For each component type and set of instances in the response
        for component_type_name, instances in response.items():
            # Get the component type object
            component_type = self._components[component_type_name]
            new_instances = list()
            loaded_instances = list()
            updated_instances = list()

            # For each newly created instance
            for node_id_str, value in instances.items():
                node_id = int(node_id_str)
                node = self._nodes[node_id]

                # If an instance doesn't already exist, create it
                if node not in component_type.instances:
                    instance = ComponentInstance(node, component_type)
                    component_type.instances[node] = instance
                    instance.is_loaded = True
                    instance.value = value
                    new_instances.append(instance)
                    continue

                # Get the existing instance
                instance = component_type.instances[node]

                # If the instance hasn't been loaded
                if not instance.is_loaded:
                    instance.value = value
                    instance.is_loaded = True
                    loaded_instances.append(instance)
                    continue

                # Update the value
                modified = instance.server_set_value(seq_number, value)
                if modified:
                    updated_instances.append(instance)

            # Call the new instance callback, if one exists
            if component_type.new_instance_callback is not None:
                for instance in new_instances:
                    component_type.new_instance_callback(instance)

            # Run callbacks for loaded instances
            for instance in loaded_instances:
                for callback in instance.loaded_callbacks:
                    callback(instance)
                instance.loaded_callbacks = None

            # Run the instance update callback, if one exists
            if component_type.update_instance_callback is not None:
                for instance in updated_instances:
                    component_type.update_instance_callback(instance)

    def _destroy_component_query(self, seq_number, priority):
        # Unused arguments
        del seq_number, priority

        # Create the message
        message = dict()

        for component_type_name, component_type in self._components.items():
            destroyed_instances = list()
            remaining = set()

            for instance in component_type.destroyed_instances:
                # If the node was destroyed, don't add it; it will be destroyed anyway (or was already)
                if instance.node.destroyed:
                    continue

                # If the node is fake, don't add it YET
                if instance.node.is_fake():
                    remaining.add(instance)
                    continue

                # If the instance hasn't been loaded yet, don't add it YET (it might not have been created yet)
                if not instance.is_loaded:
                    remaining.add(instance)
                    continue

                # Add it to the destroyed list
                destroyed_instances.append(instance.node.id)

            # Reset the destroyed instance set
            component_type.destroyed_instances = remaining

            # Only add the list to the query if it actually has anything
            if len(destroyed_instances) == 0:
                continue
            message[component_type_name] = destroyed_instances

        if len(message) == 0:
            return None
        return message

    def _destroy_component_response(self, seq_number, response):
        # Unused arguments
        del seq_number

        # For each component type with destroyed instances
        for component_type_name, instance_ids in response.items():
            component_type = self._components[component_type_name]
            destroyed_instances = list()

            # For each destroyed instance
            for node_id in instance_ids:

                # If the node has been destroyed, skip it
                if node_id not in self._nodes:
                    continue

                # Get the node
                node = self._nodes[node_id]

                # If the instance has already been destroyed, skip it
                if node not in component_type.instances:
                    continue

                # Get the instance
                instance = component_type.instances[node]
                destroyed_instances.append(instance)

                # Remove the instance
                instance.destroyed = True
                del component_type.instances[node]

            # Run the user callback
            if component_type.destroy_instance_callback is not None:
                for instance in destroyed_instances:
                    component_type.destroy_instance_callback(instance)

    def _component_property_update_query(self, seq_number, priority):
        # Unused parameters
        del priority

        message = dict()

        # For each component type
        for component_type_name, component_type in self._components.items():
            updated_instances = dict()

            # For each instance of this component type that was changed
            remaining = set()
            for changed_instance in component_type.changed_instances:
                updated_props = dict()

                # If this instance is destroyed, skip it
                if changed_instance.destroyed or changed_instance.node.destroyed:
                    continue

                # If the instance is not real, or it hasn't been loaded yet, don't add it YET
                if changed_instance.node.is_fake() or not changed_instance.is_loaded:
                    remaining.add(changed_instance)
                    continue

                # For each property of this instance that was changed
                for changed_prop_name, existing_seq_num in changed_instance.changed_props.items():
                    # If this property change has not been sent yet, add it to the query
                    if existing_seq_num is None:
                        updated_props[changed_prop_name] = deepcopy(changed_instance.value[changed_prop_name])
                        changed_instance.changed_props[changed_prop_name] = seq_number

                # Only add this instance as changed if something was actually changed
                if len(updated_props) == 0:
                    continue
                updated_instances[changed_instance.node.id] = updated_props

            # Reset the set of changed instances
            component_type.changed_instances = remaining

            # Only add this component type if something was actually changed
            if len(updated_instances) == 0:
                continue
            message[component_type_name] = updated_instances

        # Only send the message if something was changed
        if len(message) == 0:
            return None
        return message

    def _component_property_update_response(self, seq_number, response):
        # For each component type in the response
        for component_type_name, instances in response.items():
            component_type = self._components[component_type_name]

            updated_instances = list()

            # For each instance in the response
            for node_id_str, value in instances.items():
                node_id = int(node_id_str)
                node = self._nodes[node_id]

                # Get the component instance
                instance = component_type.instances[node]

                # Set the value
                modified = instance.server_set_value(seq_number, value)
                if modified:
                    updated_instances.append(instance)

            # If there's a callback for this component type
            if component_type.update_instance_callback is not None:
                for instance in updated_instances:
                    component_type.update_instance_callback(instance)

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

        message = self._generate_lightmaps_query
        self._generate_lightmaps_query = None
        return message

    def _gen_lightmaps_response(self, seq_number, response):
        # Unused parameters
        del seq_number

        if self._lightmaps_generated_callback is not None:
            self._lightmaps_generated_callback(response)

    def get_node(self, node_id):
        if node_id == Node.NULL_ID:
            return None
        return self._nodes[node_id]

    def get_component_type(self, component_type_name):
        return self._components[component_type_name]

    def get_node_components(self, node):
        result = list()
        for component_type in self._components.values():
            instance = component_type.get_instance(node)
            if instance is not None:
                result.append(instance)

        return result

    def set_new_node_callback(self, callback):
        self._new_node_callback = callback

    def set_update_node_callback(self, callback):
        self._update_node_callback = callback

    def set_destroy_node_callback(self, callback):
        self._destroy_node_callback = callback

    def set_new_component_callback(self, component_type_name, callback):
        # Get or set the component type, since this may be called before any queries are run
        component = self._components.setdefault(component_type_name, ComponentType(component_type_name))
        component.set_new_instance_callback(callback)

    def set_update_component_callback(self, component_type_name, callback):
        # Get or set the component type, since this may be called before any queries are run
        component = self._components.setdefault(component_type_name, ComponentType(component_type_name))
        component.set_update_instance_callback(callback)

    def set_destroy_component_callback(self, component_type_name, callback):
        # Get or set the component type, since this may be called before any queries are run
        component = self._components.setdefault(component_type_name, ComponentType(component_type_name))
        component.set_destroy_instance_callback(callback)

    def set_lightmaps_generated_callback(self, callback):
        self._lightmaps_generated_callback = callback

    def save_scene(self, path):
        self._save_scene_path = path

    def generate_lightmaps(self, light_dir, light_intensity, ambient, num_indirect_sample_sets, num_accumulation_steps, num_post_steps, lightmap_path):
        self._generate_lightmaps_query = {
            'light_direction': light_dir,
            'light_intensity': light_intensity,
            'ambient': ambient,
            'num_indirect_sample_sets': num_indirect_sample_sets,
            'num_accumulation_steps': num_accumulation_steps,
            'post_process_steps': num_post_steps,
            'lightmap_path': lightmap_path
        }

    def request_new_node(self, user_data):
        # Reserve fake node id
        fake_id = self._next_fake_node_id
        self._next_fake_node_id -= 1

        # Construct the node object
        node = Node()
        node.id = fake_id
        node.fake_id = fake_id
        node.user_data = user_data

        # Insert it into the table
        self._nodes[fake_id] = node
        self._unsent_new_nodes[fake_id] = node

        return node

    def request_destroy_node(self, node):
        # If the node has already been destroyed, just continue
        if node.destroyed:
            return

        # Find all of the nodes's children
        children = list()
        for child_node in self._nodes.values():
            if child_node.root == node:
                children.append(child_node)

        # Destroy the children
        for child_node in children:
            self.request_destroy_node(child_node)

        # Destroy all of the components
        for component_type in self._components.values():
            component_type.request_destroy_instance(node)

        # Remove the node from the node dictionary
        del self._nodes[node.id]
        if node.fake_id is not None:
            del self._nodes[node.fake_id]

        # Add it to the destroyed nodes set
        self._destroyed_nodes.add(node)

        # Run the callback
        if self._destroy_node_callback is not None:
            self._destroy_node_callback(self, node)

    def mark_name_dirty(self, node):
        assert(node in self._nodes.values())
        self._node_changed_names[node] = None

    def mark_root_dirty(self, node):
        assert(node in self._nodes.values())
        self._node_changed_roots[node] = None

    def mark_local_transform_dirty(self, node):
        assert(node in self._nodes.values())
        self._node_changed_local_transforms[node] = None
