# EditorSession.py
import socket
import struct
import json

class EditorSession(object):
    def __init__(self, host="localhost", port=1995):
        # Connect to the editor server
        self.sock = socket.socket()
        self.sock.settimeout(0.008)
        self.sock.connect((host, port))
        self.query_handlers = {}
        self.response_handlers = {}

    def close(self):
        self.sock.close()

    def receive_message(self):
        try:

            # Get the length of the incoming string
            (in_len,) = struct.unpack("I", self.sock.recv(4))

            # Get the incoming string
            in_str = self.sock.recv(in_len)

            # Convert it to a dictionary
            return json.loads(in_str.decode("utf-8"))

        except socket.timeout as e:
            return None

    def send_message(self, message):
        # Convert the json to a byte string
        out_str = json.dumps(message).encode()

        # Construct a packet
        out_packet = struct.pack("I", len(out_str)) + out_str

        # Send it
        self.sock.send(out_packet)

    def add_query_handler(self, query, handler):
        self.query_handlers[query] = handler

    def add_response_handler(self, query, handler):
        self.response_handlers[query] = handler

    def create_query(self):
        # Create a message for each handler
        message = {}
        for query,handler in self.query_handlers.items():
            handler_message = handler()

            # If the handler has a message to send
            if handler_message is not None:
                message[query] = handler_message

        return message

    def handle_response(self, response):
        # For each query response in the response
        for query, query_response in response.items():
            if query in self.response_handlers:
                # Run the response handler
                self.response_handlers[query](query_response)

    def cycle(self):
        # Create and send the query
        query = self.create_query()
        if len(query) != 0:
            self.send_message(query)

        # Handle the response
        response = self.receive_message()
        if response is not None:
            self.handle_response(response)
