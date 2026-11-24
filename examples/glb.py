import io
import json
import struct

import numpy as np
from PIL import Image


def load_glb(path, normals=True, texcoords=True):
    data = open(path, 'rb').read()
    json_size = struct.unpack('<I', data[12:16])[0]
    gltf = json.loads(data[20:20 + json_size])
    binary = data[28 + json_size:]

    def view(index):
        buffer_view = gltf['bufferViews'][index]
        offset = buffer_view.get('byteOffset', 0)
        return binary[offset:offset + buffer_view['byteLength']]

    def accessor(index, dtype, size):
        acc = gltf['accessors'][index]
        offset = gltf['bufferViews'][acc['bufferView']].get('byteOffset', 0) + acc.get('byteOffset', 0)
        return np.frombuffer(binary, dtype, acc['count'] * size, offset).reshape(-1, size)

    def mesh(index):
        parts = []
        for primitive in gltf['meshes'][index]['primitives']:
            attributes = primitive['attributes']
            index = accessor(primitive['indices'], 'u2' if gltf['accessors'][primitive['indices']]['componentType'] == 5123 else 'u4', 1)[:, 0]
            vertices = accessor(attributes['POSITION'], 'f4', 3)[index]
            normal = accessor(attributes['NORMAL'], 'f4', 3)[index] if 'NORMAL' in attributes else np.zeros((len(index), 3), 'f4')
            uv = accessor(attributes['TEXCOORD_0'], 'f4', 2)[index] * (1.0, -1.0) + (0.0, 1.0) if 'TEXCOORD_0' in attributes else np.zeros((len(index), 2), 'f4')
            parts.append((vertices, normal, uv))
        return [np.concatenate(columns) for columns in zip(*parts)]

    def pack(vertices, normal, uv):
        columns = [vertices @ zup]
        if normals:
            columns.append(normal @ zup)
        if texcoords:
            columns.append(uv)
        return np.concatenate(columns, axis=1, dtype='f4').tobytes()

    def transform(node):
        x, y, z, w = node.get('rotation', (0.0, 0.0, 0.0, 1.0))
        rotation = np.array([
            [1.0 - 2.0 * (y * y + z * z), 2.0 * (x * y - z * w), 2.0 * (x * z + y * w)],
            [2.0 * (x * y + z * w), 1.0 - 2.0 * (x * x + z * z), 2.0 * (y * z - x * w)],
            [2.0 * (x * z - y * w), 2.0 * (y * z + x * w), 1.0 - 2.0 * (x * x + y * y)],
        ])
        return rotation * node.get('scale', (1.0, 1.0, 1.0)), np.array(node.get('translation', (0.0, 0.0, 0.0)))

    zup = np.array([[1.0, 0.0, 0.0], [0.0, 0.0, 1.0], [0.0, -1.0, 0.0]])
    models = {item.get('name', str(i)): pack(*mesh(i)) for i, item in enumerate(gltf.get('meshes', []))}

    nodes = {}
    for i, node in enumerate(gltf.get('nodes', [])):
        if 'mesh' not in node:
            continue
        vertices, normal, uv = mesh(node['mesh'])
        matrix, translation = transform(node)
        normal = normal @ np.linalg.inv(matrix)
        normal = normal / np.linalg.norm(normal, axis=1, keepdims=True).clip(1e-6)
        nodes[node.get('name', str(i))] = pack(vertices @ matrix.T + translation, normal, uv)

    textures = {}
    for i, image in enumerate(gltf.get('images', [])):
        if 'bufferView' not in image:
            continue
        textures[image.get('name', str(i))] = Image.open(io.BytesIO(view(image['bufferView'])))

    return {'models': models, 'nodes': nodes, 'scene': b''.join(nodes.values()), 'textures': textures}
