# type_db.py

from functools import partial

class TypeDB(object):
    def __init__(self, type_constructor, component_type_constructor):
        self.type_constructor = type_constructor
        self.component_type_constructor = component_type_constructor
        self.types = {}
        self.waiting = {}
        self.pending = set()
        self.component_types = set()
        self.queried_component_types = False

    def register_handlers(self, session):
        session.add_query_handler('get_component_types', self.get_component_types_query)
        session.add_response_handler('get_component_types', self.get_component_types_response)
        session.add_query_handler('get_type_info', self.get_type_info_query)
        session.add_response_handler('get_type_info', self.get_type_info_response)

    def get_component_types_query(self, seq_number, priority):
        if not self.queried_component_types:
            self.queried_component_types = True
            return True # Actual value doesn't matter

    def get_component_types_response(self, seq_number, response):
        if response is None:
            return

        for type_name in response:
            self.component_types.add(type_name)
            self.construct_type(type_name)

    def get_type_info_query(self, seq_number, priority):
        if len(self.pending) == 0:
            return None

        message = list(self.pending)
        self.pending = set()
        return message

    def get_type_info_response(self, seq_number, response):
        if response is None:
            return None

        # Iterate through respones types
        for type_name,type_info in response.items():
            dependent_types = set()
            self.waiting[type_name] = (type_info, dependent_types)


            # If the type has any dependent properties
            if type_info["properties"] is not None:
                for propName,propInfo in type_info["properties"].items():
                    dependent_types.add(propInfo["type"])
            else:
                # Set the properties to an empty table if there are no properties
                # TODO: This should be removed once the serialization api supports writer coercion
                type_info["properties"] = dict()

        # Try to construct waiting types
        for type_name in list(self.waiting.keys()):
            self.construct_type(type_name)

    def insert_type(self, type_name, type_object):
        self.types[type_name] = type_object

    def construct_type(self, type_name):
        # If the type has already been constructed
        if type_name in self.types:
            return True

        # If the type isn't waiting to be constructed
        if type_name not in self.waiting:
            self.pending.add(type_name)
            return False

        # Try to construct all types this type is waiitng on
        dependent = self.waiting[type_name][1]

        # For each type this type is waiting on to be constructed
        for dependent_type_name in dependent.copy():
            # Try to construct it
            if self.construct_type(dependent_type_name):
                dependent.remove(dependent_type_name)

        # If there are no more dependent types:
        if len(dependent) == 0:
            # Construct the type and add it to the types table
            type_object = self.type_constructor(self, type_name, self.waiting[type_name][0])
            self.insert_type(type_name, type_object)
            del self.waiting[type_name]

            # If it's a component type, run the constructor for that
            if type_name in self.component_types:
                self.component_type_constructor(self, type_name, type_object)
            return True
        else:
            return False

    def get_type(self, type_name):
        if type_name in self.types:
            return self.types[type_name]
        else:
            self.pending.add(type_name)
            return None
