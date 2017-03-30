# EditorSession.py
import socket
import struct
import json
import threading
import queue


class EditorSession(object):
    CONNECTION_TIMEOUT = 2
    BUFFER_SIZE = 2 ** 14

    SEQUENCE_NUMBER_SIZE = 4
    CONTENT_LENGTH_SIZE = 4

    # Sequence number indicating that this message is not associated with a known sequence number
    NULL_SEQUENCE_NUMBER = 0

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
        self._thread = None
        self._continue = True
        self._in_messages = queue.Queue()
        self._out_messages = queue.Queue()

    def _thread_loop(self):
        while self._continue:
            # Cycle in messages
            in_message = self.receive_message()
            if in_message is not None:
                self._in_messages.put(in_message)

            # Cycle out messages
            if not self._out_messages.empty():
                out_message = self._out_messages.get()
                self.send_message(*out_message)

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
        # Try connecting to the server
        try:
            self._sock.connect((host, port))
        except Exception as _:
            return False

        # Spin up a thread for receiving messages
        self._continue = True
        self._thread = threading.Thread(target=self._thread_loop)
        self._thread.start()
        return True

    def close(self):
        self._continue = False
        self._thread.join()
        self._sock.close()

    def receive_message(self):

        try:
            # Set it to non-blocking while we check for a packet
            self._sock.setblocking(False)

            # Probe the socket for data
            self._get_socket_data()
            if self._len_socket_data() == 0:
                return None

        # If an error occurred, quit
        except socket.error as _:
            return None

        # Set it to block while loading the packet
        self._sock.setblocking(True)

        # Get the packet sequence number and content length
        (seq_number,) = struct.unpack('I', self._split_socket_data(self.SEQUENCE_NUMBER_SIZE))

        # Get the length of the incoming string
        (in_len,) = struct.unpack('I', self._split_socket_data(self.CONTENT_LENGTH_SIZE))

        # Create a result string
        in_str = self._split_socket_data(in_len).decode('utf-8')
        in_content = json.loads(in_str)

        # Return the packet info
        return seq_number, in_content

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
        for query, handler in self._query_handlers.items():
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
            else:
                print("WARNING: Unregistered handler for response: {}".format(query))

    def cycle(self, priority):
        # Create and send the query
        seq_number = self._next_seq_number
        query = self.create_query(seq_number, priority)
        if len(query) != 0:
            self._out_messages.put((seq_number, query))
            self._next_seq_number += 1

        # Handle the response
        if not self._in_messages.empty():
            response = self._in_messages.get()
            self.handle_response(*response)
