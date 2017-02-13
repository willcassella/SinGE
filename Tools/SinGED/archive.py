# archive.py

import struct
import json

class Archive(object):
    NODE_NULL = 0
    NODE_BOOLEAN = 1
    NODE_INT8 = 2
    NODE_UINT8 = 3
    NODE_INT16 = 4
    NODE_UINT16 = 5
    NODE_INT32 = 6
    NODE_UINT32 = 7
    NODE_INT64 = 8
    NODE_UINT64 = 9
    NODE_FLOAT = 10
    NODE_DOUBLE = 11
    NODE_STRING = 12
    NODE_ARRAY_BOOLEAN = 13
    NODE_ARRAY_INT8 = 14
    NODE_ARRAY_UINT8 = 15
    NODE_ARRAY_INT16 = 16
    NODE_ARRAY_UINT16 = 17
    NODE_ARRAY_INT321 = 18
    NODE_ARRAY_UINT32 = 19
    NODE_ARRAY_INT64 = 20
    NODE_ARRAY_UINT64 = 21
    NODE_ARRAY_FLOAT = 22
    NODE_ARRAY_DOUBLE = 23
    NODE_ARRAY_GENERIC = 24
    NODE_OBJECT = 25

    class Node(object):
        def __init__(self):
            self.type = Archive.NODE_NULL
            self.data = None

    def __init__(self):
        self.stack = list()
        self.root = Archive.Node()

    def pop(self):
        self.root = self.stack.pop()

    def null(self):
        # Make sure this node is null
        assert(self.root.type == Archive.NODE_NULL)

    def u8(self, value):
        # Make sure this node type is null
        assert(self.root.type == Archive.NODE_NULL)
        self.root.type = Archive.NODE_UINT8
        self.root.data = value

    def string(self, string):
        # Make sure this node is null
        assert(self.root.type == Archive.NODE_NULL)
        self.root.type = Archive.NODE_STRING
        self.root.data = string

    def typed_array_i8(self, array):
        # Make sure this node is null
        assert(self.root.type == Archive.NODE_NULL)
        self.root.type = Archive.NODE_ARRAY_INT8
        self.root.data = array

    def typed_array_i16(self, array):
        # Make sure this node is null
        assert(self.root.type == Archive.NODE_NULL)
        self.root.type = Archive.NODE_ARRAY_INT16
        self.root.data = array

    def typed_array_u32(self, array):
        # Make sure this node is null
        assert(self.root.type == Archive.NODE_NULL)
        self.root.type = Archive.NODE_ARRAY_UINT32
        self.root.data = array

    def typed_array_f32(self, array):
        # Make sure this node is null
        assert(self.root.type == Archive.NODE_NULL)
        self.root.type = Archive.NODE_ARRAY_FLOAT
        self.root.data = array

    def as_object(self):
        # Make sure this node is null
        assert(self.root.type == Archive.NODE_NULL)
        self.root.type = Archive.NODE_OBJECT
        self.root.data = list()

    def push_object_member(self, name):
        # Make sure this node is an object
        assert(self.root.type == Archive.NODE_OBJECT)

        # Add this node as a member of the root
        new_root = Archive.Node()
        self.root.data.append((name, new_root))
        self.stack.append(self.root)
        self.root = new_root

    def to_binary(self):
        buffer = bytearray()
        Archive.binary_node(buffer, self.root)
        return buffer

    @staticmethod
    def binary_node_type(buffer, type):
        buffer += struct.pack('B', type)

    @staticmethod
    def binary_number(buffer, node, fmt):
        Archive.binary_node_type(buffer, node.type)
        buffer += struct.pack(fmt, node.data)

    @staticmethod
    def binary_string(buffer, node):
        Archive.binary_node_type(buffer, Archive.NODE_STRING)
        buffer += struct.pack('I', len(node.data))
        buffer += struct.pack('{}s'.format(len(node.data)), node.data.encode())

    @staticmethod
    def binary_typed_array(buffer, node, fmt):
        Archive.binary_node_type(buffer, node.type)
        buffer += struct.pack('I', len(node.data))
        buffer += struct.pack('{}{}'.format(len(node.data), fmt), *node.data)

    @staticmethod
    def binary_object(buffer, node):
        # Put the node type and object size into this buffer
        Archive.binary_node_type(buffer, node.type)
        buffer += struct.pack('I', len(node.data))

        # Serialize the object into a new buffer
        object_buff = bytearray()
        for name, member in node.data:
            # Add name and null-terminating byte
            object_buff += struct.pack('{}s'.format(len(name)), name.encode())
            object_buff += struct.pack('B', 0)

            # Serialize node
            Archive.binary_node(object_buff, member)

        # Append the span to the original buffer (accounts for indicator byte, size, span, and contents)
        buffer += struct.pack('I', 1 + 4 + 4 + len(object_buff))

        # Append the object buffer
        buffer += object_buff

    @staticmethod
    def binary_node(buffer, node):
        if node.type == Archive.NODE_NULL:
            Archive.binary_node_type(Archive.NODE_NULL)

        elif node.type == Archive.NODE_UINT8:
            Archive.binary_number(buffer, node, 'B')

        elif node.type == Archive.NODE_STRING:
            Archive.binary_string(buffer, node)

        elif node.type == Archive.NODE_ARRAY_INT8:
            Archive.binary_typed_array(buffer, node, 'b')

        elif node.type == Archive.NODE_ARRAY_INT16:
            Archive.binary_typed_array(buffer, node, 'h')

        elif node.type == Archive.NODE_ARRAY_UINT32:
            Archive.binary_typed_array(buffer, node, 'I')

        elif node.type == Archive.NODE_ARRAY_FLOAT:
            Archive.binary_typed_array(buffer, node, 'f')

        elif node.type == Archive.NODE_OBJECT:
            Archive.binary_object(buffer, node)
