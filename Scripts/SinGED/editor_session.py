# EditorSession.py
import socket
import struct
import json

class EditorSession(object):
    CONNECTION_TIMEOUT = 2
    BUFFER_SIZE = 4096
    FLAG_NONE = ' '
    FLAG_GLOBAL = 'G'

    def __init__(self, host="localhost", port=1995):
        # Connect to the editor server
        self.sock = socket.socket()
        self.sock.settimeout(self.CONNECTION_TIMEOUT)
        self.sock.connect((host, port))
        self.query_handlers = {}
        self.response_handlers = {}
        self.socket_data = b''

    def close(self):
        self.sock.close()

    def get_socket_data(self):
        self.socket_data += self.sock.recv(self.BUFFER_SIZE)

    def split_socket_data(self, offset):
        while len(self.socket_data) < offset:
            self.get_socket_data()

        # Split the socket data along the offset
        result = self.socket_data[: offset]
        self.socket_data = self.socket_data[offset: ]
        return result

    def len_socket_data(self):
        return len(self.socket_data)

    def receive_message(self):

        try:
            # Set it to non-blocking while we check for a packet
            self.sock.setblocking(False)

            # Probe the socket for data
            self.get_socket_data()
            if self.len_socket_data() == 0:
                return None

        # If an error occured, quit
        except socket.error as e:
            return None

        # Set it to block while loading the packet
        self.sock.setblocking(True)

        # Get the packet flag
        flag = self.split_socket_data(1).decode('utf-8')

        # Get the length of the incoming string
        (in_len,) = struct.unpack('I', self.split_socket_data(4))

        # Create a result string
        in_str = self.split_socket_data(in_len).decode('utf-8')

        # Convert it to a dictionary
        return (flag, json.loads(in_str))

    def send_message(self, message):
        # Convert the json to a byte string
        out_str = json.dumps(message).encode()

        # Construct a packet
        out_packet = struct.pack('I', len(out_str)) + out_str

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

    def handle_response(self, flag, response):
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
            self.handle_response(*response)
