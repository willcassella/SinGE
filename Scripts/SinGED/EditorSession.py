# EditorSession.py
import socket
import struct
import json

class EditorSession(object):
    def __init__(self, host="localhost", port=1995):
        # Connect to the editor server
        self.sock = socket.socket()
        self.sock.connect((host, port))
        self.handlers = {}

    def close(self):
        self.sock.close()

    def receive_message(self):
        # Get the length of the incoming string
        (in_len,) = struct.unpack("I", self.sock.recv(4))

        # Get the incoming string
        in_str = self.sock.recv(in_len)

        # Convert it to a dictionary
        return json.loads(in_str.decode("utf-8"))

    def send_message(self, message):
        # Convert the json to a byte string
        out_str = json.dumps(message).encode()

        # Construct a packet
        out_packet = struct.pack("I", len(out_str)) + out_str

        # Send it
        self.sock.send(out_packet)

    def send_handler_message(self):
        # Create a message for each handler
        message = {}
        for query,handler in self.handlers.items():
            handler_message = handler.create_message(query)

            # If the handler has a message to send
            if handler_message is not None:
                message[query] = handler_message

        # If there's nothing to be sent
        if len(message) == 0:
            return

        # Send out the message
        self.send_message(message)

    def run_handlers(self):
        message = self.receive_message()

        # For each query result in the received message
        for query,response in message.items():
            if query in self.handlers:
                # Run the query handler
                self.handlers[query].handle_response(query, response)

    def cycle(self):
        self.send_handler_message()
        self.run_handlers()

    def add_handler(self, query, handler):
        self.handlers[query] = handler
