# Scene.py

class Scene(object):
    def __init__(self):
        self.entities = {}
        self.new_entities = []
        self.new_components = {}

    def create_message(self, query):
        if query == 'get_scene':
            if len(self.entities) != 0:
                return None

        return ['entities', 'components']

    def handle_response(self, query, response):
        self.entities = response

    def get_components(self, entity):
        if entity in self.entities:
            return self.entities[entity]['components']

        
