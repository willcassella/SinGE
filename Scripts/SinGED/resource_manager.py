# resource_manager.py

class ResourceManager(object):
    def __init__(self, resource_constructor):
        self.resource_constructor =  resource_constructor
        self.resources = {}
        self.waiting_resources = {}

    def register_handlers(self, session):
        session.add_query_handler('get_resource', self.get_resource_query)
        session.add_response_handler('get_resource', self.get_resource_response)

    def get_resource_query(self):
        message = []

        # For each resource to be loaded
        for path, info in self.waiting_resources.items():
            # If a query has already been sent for this resource, continue
            if info[1]:
                continue

            # Add the resource to the message
            message.append({
                "type": info[0],
                "path": path,
            })

            # Mark this resource as having been sent in a query
            info[1] = True

        # See if we even need to send a message
        if len(message) == 0:
            return None

        # Send the message
        return message

    def get_resource_response(self, response):
        if response is None:
            return

        for resource in response:
            type = resource['type']
            path = resource['path']
            value = resource['value']

            # Construct the resource
            res = self.resource_constructor(self, path, type, value)

            # Insert the resource into the table
            self.resources[path] = res

            # Get it from the waiting table
            if path not in self.waiting_resources:
                continue

            # Get the callbacks for this resource
            callbacks = self.waiting_resources[path][2]
            del self.waiting_resources[path]

            # Run all the callbacks
            for callback in callbacks:
                callback(path, res)

    def insert_resource(self, path, obj):
        self.resources[path] = obj

    def get_resource(self, path, type, callback):
        self.waiting_resources.setdefault(path, [type, False, []])
        self.waiting_resources[path][2].append(callback)
