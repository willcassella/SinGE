# ComponentView.py

def get_value(table, path, name):
    # Navigate to the value in the table
    for step in path:
        if step not in table:
            return (False, None)
        table = table[step]

    # Return the value
    if name in table:
        return (True, table[name])
    else:
        return (False, None)

def set_value(table, path, name, value):
    # Navigate along the path to the value
    for step in path:
        if step not in table:
            table[step] = {}
        table = table[step]

    # Set the value
    table[name] = value

class ComponentView(object):
    def __init__(self, component_constructor):
        self.constructed_components = set()
        self.in_values = {}
        self.out_values = {}
        self.component_constructor = component_constructor

    def property_get(self, name, obj):
        # Locate the value in the intput table
        path = obj.name.split('.')
        (found, value) = get_value(self.in_values, path, name)
        return value

    def property_set(self, name, obj, value):
        # Insert the value into the output table and the input table
        path = obj.name.split('.')
        set_value(self.out_values, path, name, value)
        set_value(self.in_values, path, name, value)

    def create_message(self, query):
        out = self.out_values
        self.out_values = {}
        return out

    def handle_response(self, query, response):
        if query == 'get_component':
            self.in_values = response
        elif query == 'get_scene':
            for component_type in response['components'].keys():
                if component_type not in self.constructed_components:
                    self.component_constructor(component_type)
