# EditorSession.py
import socket
import struct
import json

class EditorSession(object):
    CONNECTION_TIMEOUT = 2
    BUFFER_SIZE = 4096

    SEQUENCE_NUMBER_SIZE = 4
    CONTENT_LENGTH_SIZE = 4

    # Indicates that any-priority packets should be run
    PRIORITY_ANY = 'PRIORITY_ANY'

    # Indicates that only high-priority queries should be run
    PRIORITY_HIGH = 'PRIORITY_HIGH'

    def __init__(self):
        # Connect to the editor server
        self._sock = socket.socket()
        self._sock.settimeout(self.CONNECTION_TIMEOUT)
        self._socket_data = b''
        self._query_handlers = {}
        self._response_handlers = {}
        self._next_seq_number = 1

    def _get_socket_data(self):
        self._socket_data += self._sock.recv(self.BUFFER_SIZE)

    def _split_socket_data(self, offset):
        while len(self._socket_data) < offset:
            self._get_socket_data()

        # Split the socket data along the offset
        result = self._socket_data[: offset]
        self._socket_data = self._socket_data[offset: ]
        return result

    def _len_socket_data(self):
        return len(self._socket_data)

    def connect(self, host="localhost", port=1995):
        try:
            self._sock.connect((host, port))
        except Exception as e:
            return False
        return True

    def close(self):
        self._sock.close()

    def receive_message(self):

        try:
            # Set it to non-blocking while we check for a packet
            self._sock.setblocking(False)

            # Probe the socket for data
            self._get_socket_data()
            if self._len_socket_data() == 0:
                return None

        # If an error occured, quit
        except socket.error as e:
            return None

        # Set it to block while loading the packet
        self._sock.setblocking(True)

        # Get the packet sequence number and content length
        (seq_number,) = struct.unpack('I', self._split_socket_data(self.SEQUENCE_NUMBER_SIZE))

        # Get the length of the incoming string
        (in_len,) = struct.unpack('I', self._split_socket_data(self.CONTENT_LENGTH_SIZE))

        # Create a result string
        in_str = self._split_socket_data(in_len).decode('utf-8')

        # Return the packet info
        return (seq_number, json.loads(in_str))

    def send_message(self, seq_number, message):
        # Convert the json to a byte string
        out_content = json.dumps(message).encode()

        # Create the packet header
        out_packet = struct.pack('II', seq_number, len(out_content))

        # Add the content
        out_packet += out_content

        # Send it
        self._sock.send(out_packet)

    def add_query_handler(self, query, handler):
        self._query_handlers[query] = handler

    def add_response_handler(self, query, handler):
        self._response_handlers[query] = handler

    def create_query(self, seq_number, priority):
        # Create a message for each handler
        message = {}
        for query,handler in self._query_handlers.items():
            handler_message = handler(seq_number, priority)

            # If the handler has a message to send
            if handler_message is not None:
                message[query] = handler_message

        return message

    def handle_response(self, seq_number, response):
        # For each query response in the response
        for query, query_response in response.items():
            if query in self._response_handlers:
                # Run the response handler
                self._response_handlers[query](seq_number, query_response)

    def cycle(self, priority):
        # Create and send the query
        seq_number = self._next_seq_number
        query = self.create_query(seq_number, priority)
        if len(query) != 0:
            self.send_message(seq_number, query)
            self._next_seq_number += 1

        # Handle the response
        response = self.receive_message()
        if response is not None:
            self.handle_response(*response)
