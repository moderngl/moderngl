import os
import re
import struct

import pygame


def init():
    os.environ['SDL_WINDOWS_DPI_AWARENESS'] = 'permonitorv2'
    pygame.display.init()
    pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MAJOR_VERSION, 3)
    pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MINOR_VERSION, 3)
    pygame.display.gl_set_attribute(pygame.GL_CONTEXT_PROFILE_MASK, pygame.GL_CONTEXT_PROFILE_CORE)
    pygame.display.gl_set_attribute(pygame.GL_CONTEXT_FORWARD_COMPATIBLE_FLAG, 1)


def load_obj(path):
    text = open(path).read()
    vertices = re.findall(r'^v\s+(\S+)\s+(\S+)\s+(\S+)', text, re.M)
    uvs = re.findall(r'^vt\s+(\S+)\s+(\S+)', text, re.M)
    normals = re.findall(r'^vn\s+(\S+)\s+(\S+)\s+(\S+)', text, re.M)
    faces = re.findall(r'^f\s+(\d+)/(\d+)/(\d+)\s+(\d+)/(\d+)/(\d+)\s+(\d+)/(\d+)/(\d+)', text, re.M)
    data = []
    for face in faces:
        for v, vt, vn in zip(face[0::3], face[1::3], face[2::3]):
            data += vertices[int(v) - 1] + normals[int(vn) - 1] + uvs[int(vt) - 1]
    return struct.pack(f'{len(data)}f', *map(float, data))
