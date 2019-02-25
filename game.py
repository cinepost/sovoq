#! /usr/bin/env python


from ctypes import *
import sys

import pygame
from pygame.locals import *

try:
    # For OpenGL-ctypes
    from OpenGL import platform
    gl = platform.OpenGL
except: # ImportError:
    try:
      # For PyOpenGL
      gl = cdll.LoadLibrary('libGL.so')
    except OSError:
        # Load for Mac
        from ctypes.util import find_library
        path = find_library('OpenGL')    # finds the absolute path to the framework
        gl = cdll.LoadLibrary(path)
 
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

GL_FRAGMENT_SHADER_ARB = 0x8B30
GL_VERTEX_SHADER_ARB = 0x8B31
GL_OBJECT_COMPILE_STATUS_ARB= 0x8B81
GL_OBJECT_LINK_STATUS_ARB = 0x8B82
GL_INFO_LOG_LENGTH_ARB = 0x8B84


glCreateShaderObjectARB = gl.glCreateShaderObjectARB
glShaderSourceARB = gl.glShaderSourceARB
glShaderSourceARB.argtypes = [c_int, c_int, POINTER(c_char_p), POINTER(c_int)]
glCompileShaderARB = gl.glCompileShaderARB
glGetObjectParameterivARB = gl.glGetObjectParameterivARB
glGetObjectParameterivARB.argtypes = [c_int, c_int, POINTER(c_int)]
glCreateProgramObjectARB = gl.glCreateProgramObjectARB
glGetInfoLogARB = gl.glGetShaderInfoLog
glGetInfoLogARB.argtypes = [c_int, c_int, POINTER(c_int), c_char_p]
glAttachObjectARB = gl.glAttachObjectARB
glLinkProgramARB = gl.glLinkProgramARB
glDeleteObjectARB = gl.glDeleteObjectARB
glGetError = gl.glGetError
glUseProgramObjectARB = gl.glUseProgramObjectARB



        
def print_log(shader):
    length = c_int()
    glGetObjectParameterivARB(shader, GL_INFO_LOG_LENGTH, byref(length))
 
    if length.value > 0:
        log = create_string_buffer(length.value)
        glGetInfoLogARB(shader, length, byref(length), log)
        print >> sys.stderr, log.value

        
        
def compile_shader(source, shader_type):
    shader = glCreateShaderObjectARB(shader_type)
    source = c_char_p(source)
    length = c_int(-1)
    glShaderSourceARB(shader, 1, byref(source), byref(length))
    glCompileShaderARB(shader)
    status = c_int()
    glGetObjectParameterivARB(shader, GL_OBJECT_COMPILE_STATUS_ARB,
        byref(status))
    if (not status.value):
        print_log(shader)
        raise SystemExit
    return shader
    

def compile_program(vertex_source, fragment_source):
    vertex_shader = None
    fragment_shader = None
    program = glCreateProgramObjectARB()
 
    if vertex_source:
        vertex_shader = compile_shader(vertex_source, GL_VERTEX_SHADER_ARB)
        glAttachObjectARB(program, vertex_shader)
    if fragment_source:
        fragment_shader = compile_shader(fragment_source,
            GL_FRAGMENT_SHADER_ARB)
        glAttachObjectARB(program, fragment_shader)
 
    glLinkProgramARB(program)
 
    if vertex_shader:
        glDeleteObjectARB(vertex_shader)
    if fragment_shader:
        glDeleteObjectARB(fragment_shader)
 
    return program
    
    
if __name__ == '__main__':
    glutInit(sys.argv)
    game_screen_size = 480, 272
    crt_screen_size = 1920, 1080
    
    pygame.init()
    game_screen = pygame.display.set_mode( game_screen_size, OPENGL )
    crt_screen = pygame.display.set_mode( crt_screen_size, OPENGL | DOUBLEBUF )


 
    program = compile_program('''
    varying vec3 normal;
    void main() {
        normal = gl_NormalMatrix * gl_Normal;
        gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    }
    ''', '''
    varying vec3 normal;
    void main() {
        float intensity;
        vec4 color;
        vec3 n = normalize(normal);
        vec3 l = normalize(gl_LightSource[0].position).xyz;
 
        // quantize to 5 steps (0, .25, .5, .75 and 1)
        intensity = (floor(dot(l, n) * 4.0) + 1.0)/4.0;
        color = vec4(intensity*1.0, intensity*0.5, intensity*0.5,
            intensity*1.0);
 
        gl_FragColor = color;
    }
    ''')
 
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(90.0, width/float(height), 1.0, 100.0)
    glMatrixMode(GL_MODELVIEW)
    glEnable(GL_DEPTH_TEST)
    
    quit = False
    angle = 0
    while not quit:
        for e in pygame.event.get():
            if e.type in (QUIT, KEYDOWN):
                quit = True
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glLoadIdentity()
        glTranslate(0.0, 0.0, -2.5)
        glRotate(angle, 0.0, 1.0, 0.0)
        glUseProgramObjectARB(program)
        glutSolidTeapot(1.0)
        angle += 0.1 
        pygame.display.flip()
