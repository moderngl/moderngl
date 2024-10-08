# WebGL Water
# https://madebyevan.com/webgl-water/
# Copyright 2011 Evan Wallace
# Released under the MIT license

import os
import glm
from light_gl import *
from water import Water
import moderngl


def local(*path):
    return os.path.join(os.path.dirname(__file__), *path)

def local_textures(path):
    return local("../data/textures", path)

class Renderer():
    _helper_functions = """
    const float IOR_AIR = 1.0;
    const float IOR_WATER = 1.333;
    const vec3 abovewaterColor = vec3(0.25, 1.0, 1.25);
    const vec3 underwaterColor = vec3(0.4, 0.9, 1.0);
    const float poolHeight = 1.0;
    uniform vec3 light;
    uniform vec3 sphereCenter;
    uniform float sphereRadius;
    uniform sampler2D tiles;
    uniform sampler2D causticTex;
    uniform sampler2D water;
    
    vec2 intersectCube(vec3 origin, vec3 ray, vec3 cubeMin, vec3 cubeMax) {
        vec3 tMin = (cubeMin - origin) / ray;
        vec3 tMax = (cubeMax - origin) / ray;
        vec3 t1 = min(tMin, tMax);
        vec3 t2 = max(tMin, tMax);
        float tNear = max(max(t1.x, t1.y), t1.z);
        float tFar = min(min(t2.x, t2.y), t2.z);
        return vec2(tNear, tFar);
    }
    
    float intersectSphere(vec3 origin, vec3 ray, vec3 sphereCenter, float sphereRadius) {
        vec3 toSphere = origin - sphereCenter;
        float a = dot(ray, ray);
        float b = 2.0 * dot(toSphere, ray);
        float c = dot(toSphere, toSphere) - sphereRadius * sphereRadius;
        float discriminant = b*b - 4.0*a*c;
        if (discriminant > 0.0) {
        float t = (-b - sqrt(discriminant)) / (2.0 * a);
        if (t > 0.0) return t;
        }
        return 1.0e6;
    }
    
    vec3 getSphereColor(vec3 point) {
        vec3 color = vec3(0.5);
        
        /* ambient occlusion with walls */
        color *= 1.0 - 0.9 / pow((1.0 + sphereRadius - abs(point.x)) / sphereRadius, 3.0);
        color *= 1.0 - 0.9 / pow((1.0 + sphereRadius - abs(point.z)) / sphereRadius, 3.0);
        color *= 1.0 - 0.9 / pow((point.y + 1.0 + sphereRadius) / sphereRadius, 3.0);
        
        /* caustics */
        vec3 sphereNormal = (point - sphereCenter) / sphereRadius;
        vec3 refractedLight = refract(-light, vec3(0.0, 1.0, 0.0), IOR_AIR / IOR_WATER);
        float diffuse = max(0.0, dot(-refractedLight, sphereNormal)) * 0.5;
        vec4 info = texture2D(water, point.xz * 0.5 + 0.5);
        if (point.y < info.r) {
        vec4 caustic = texture2D(causticTex, 0.75 * (point.xz - point.y * refractedLight.xz / refractedLight.y) * 0.5 + 0.5);
        diffuse *= caustic.r * 4.0;
        }
        color += diffuse;
        
        return color;
    }
    
    vec3 getWallColor(vec3 point) {
        float scale = 0.5;
        
        vec3 wallColor;
        vec3 normal;
        if (abs(point.x) > 0.999) {
            wallColor = texture2D(tiles, point.yz * 0.5 + vec2(1.0, 0.5)).rgb;
            normal = vec3(-point.x, 0.0, 0.0);
        } else if (abs(point.z) > 0.999) {
            wallColor = texture2D(tiles, point.yx * 0.5 + vec2(1.0, 0.5)).rgb;
            normal = vec3(0.0, 0.0, -point.z);
        } else {
            wallColor = texture2D(tiles, point.xz * 0.5 + 0.5).rgb;
            normal = vec3(0.0, 1.0, 0.0);
        }
        
        scale /= length(point); /* pool ambient occlusion */
        scale *= 1.0 - 0.9 / pow(length(point - sphereCenter) / sphereRadius, 4.0); /* sphere ambient occlusion */
        
        /* caustics */
        vec3 refractedLight = -refract(-light, vec3(0.0, 1.0, 0.0), IOR_AIR / IOR_WATER);
        float diffuse = max(0.0, dot(refractedLight, normal));
        vec4 info = texture2D(water, point.xz * 0.5 + 0.5);
        if (point.y < info.r) {
            vec4 caustic = texture2D(causticTex, 0.75 * (point.xz - point.y * refractedLight.xz / refractedLight.y) * 0.5 + 0.5);
            scale += diffuse * caustic.r * 2.0 * caustic.g;
        } else {
            /* shadow for the rim of the pool */
            vec2 t = intersectCube(point, refractedLight, vec3(-1.0, -poolHeight, -1.0), vec3(1.0, 2.0, 1.0));
            diffuse *= 1.0 / (1.0 + exp(-200.0 / (1.0 + 10.0 * (t.y - t.x)) * (point.y + refractedLight.y * t.y - 2.0 / 12.0)));
            
            scale += diffuse * 0.5;
        }
        
        return wallColor * scale;
    }
    """

    def _splice(array: list, start: int, num: int):
        new_array = []
        exclude_range = range(start, start + num)
        for idx, elem in enumerate(array):
            if idx not in exclude_range:
                new_array.append(elem)
        return new_array

    def _water_shader(ctx: moderngl.Context, under_water: bool):
        vert_shader = """
        uniform sampler2D water;
        varying vec3 position;
        void main() {
            vec4 info = texture2D(water, gl_Vertex.xy * 0.5 + 0.5);
            position = gl_Vertex.xzy;
            position.y += info.r;
            gl_Position = gl_ModelViewProjectionMatrix * vec4(position, 1.0);
        }
        """
        if under_water:
            water_frag_shader = """
            /* underwater */ 
            normal = -normal;
            vec3 reflectedRay = reflect(incomingRay, normal);
            vec3 refractedRay = refract(incomingRay, normal, IOR_WATER / IOR_AIR);
            float fresnel = mix(0.5, 1.0, pow(1.0 - dot(normal, -incomingRay), 3.0));
            
            vec3 reflectedColor = getSurfaceRayColor(position, reflectedRay, underwaterColor);
            vec3 refractedColor = getSurfaceRayColor(position, refractedRay, vec3(1.0)) * vec3(0.8, 1.0, 1.1);
            
            gl_FragColor = vec4(mix(reflectedColor, refractedColor, (1.0 - fresnel) * length(refractedRay)), 1.0);
            """
        else:
            water_frag_shader = """
                /* above water */
                vec3 reflectedRay = reflect(incomingRay, normal);
                vec3 refractedRay = refract(incomingRay, normal, IOR_AIR / IOR_WATER);
                float fresnel = mix(0.25, 1.0, pow(1.0 - dot(normal, -incomingRay), 3.0));
                
                vec3 reflectedColor = getSurfaceRayColor(position, reflectedRay, abovewaterColor);
                vec3 refractedColor = getSurfaceRayColor(position, refractedRay, abovewaterColor);
                
                gl_FragColor = vec4(mix(refractedColor, reflectedColor, fresnel), 1.0);
            """

        frag_shader = """
        uniform vec3 eye;
        varying vec3 position;
        uniform samplerCube sky;
        
        vec3 getSurfaceRayColor(vec3 origin, vec3 ray, vec3 waterColor) {
            vec3 color;
                float q = intersectSphere(origin, ray, sphereCenter, sphereRadius);
            if (q < 1.0e6) {
                color = getSphereColor(origin + ray * q);
            } else if (ray.y < 0.0) {
                vec2 t = intersectCube(origin, ray, vec3(-1.0, -poolHeight, -1.0), vec3(1.0, 2.0, 1.0));
                color = getWallColor(origin + ray * t.y);
            } else {
                vec2 t = intersectCube(origin, ray, vec3(-1.0, -poolHeight, -1.0), vec3(1.0, 2.0, 1.0));
                vec3 hit = origin + ray * t.y;
                if (hit.y < 2.0 / 12.0) {
                    color = getWallColor(hit);
                } else {
                    color = textureCube(sky, ray).rgb;
                    color += vec3(pow(max(0.0, dot(light, ray)), 5000.0)) * vec3(10.0, 8.0, 6.0);
                }
            }
            if (ray.y < 0.0) color *= waterColor;
            return color;
        }
        
        void main() {
            vec2 coord = position.xz * 0.5 + 0.5;
            vec4 info = texture2D(water, coord);
            
            /* make water look more "peaked" */
            for (int i = 0; i < 5; i++) {
            coord += info.ba * 0.005;
            info = texture2D(water, coord);
            }
            
            vec3 normal = vec3(info.b, sqrt(1.0 - dot(info.ba, info.ba)), info.a);
            vec3 incomingRay = normalize(position - eye);
        """ + water_frag_shader + """}"""
        return Shader(vert_shader, Renderer._helper_functions + frag_shader, ctx)

    def __init__(self, ctx: moderngl.Context) -> None:
        self.ctx = ctx
        self.tile_texture = ImageTexture(
            local_textures("tiles.jpg"), "RGB", ctx, (True, True, True), mipmaps=True)
        self.light_dir = glm.normalize(glm.vec3(2.0, 2.0, -1.0))
        # water
        self.water_mesh = MeshBuilder.panel(detail=200).build(ctx)
        self.water_shaders: list[Shader] = []
        for under_water in (False, True):
            self.water_shaders.append(Renderer._water_shader(ctx, under_water))
        # sphere
        self.sphere_mesh = MeshBuilder.sphere(detail=10).build(ctx)
        self.sphere_shader = Shader(Renderer._helper_functions + """
        varying vec3 position;
        void main() {
            position = sphereCenter + gl_Vertex.xyz * sphereRadius;
            gl_Position = gl_ModelViewProjectionMatrix * vec4(position, 1.0);
        }
        """, Renderer._helper_functions + """
        varying vec3 position;
        void main() {
            gl_FragColor = vec4(getSphereColor(position), 1.0);
            vec4 info = texture2D(water, position.xz * 0.5 + 0.5);
            if (position.y < info.r) {
                gl_FragColor.rgb *= underwaterColor * 1.2;
            }
        }""", ctx)
        self.sphere_center = glm.vec3(0.0)
        self.sphere_radius = 0
        # cude
        cude_mesh_builder = MeshBuilder.cube()
        # remove cude top
        cude_mesh_builder.triangles = np.array(Renderer._splice(
            cude_mesh_builder.triangles, 4, 2), dtype=np.int32)
        self.cube_mesh = cude_mesh_builder.build(ctx)
        self.cude_shader = Shader(Renderer._helper_functions + """
        varying vec3 position;
        void main() {
            position = gl_Vertex.xyz;
            position.y = ((1.0 - position.y) * (7.0 / 12.0) - 1.0) * poolHeight;
            gl_Position = gl_ModelViewProjectionMatrix * vec4(position, 1.0);
        }""", Renderer._helper_functions + """
        varying vec3 position;
        void main() {
            gl_FragColor = vec4(getWallColor(position), 1.0);
            vec4 info = texture2D(water, position.xz * 0.5 + 0.5);
            if (position.y < info.r) {
                gl_FragColor.rgb *= underwaterColor * 1.2;
            }
        }""", ctx)

        self.caustic_texture = RawTexture((1024, 1024), ctx=ctx)
        self.caustics_shader = Shader(Renderer._helper_functions + """
        varying vec3 oldPos;
        varying vec3 newPos;
        varying vec3 ray;
        
        /* project the ray onto the plane */
        vec3 project(vec3 origin, vec3 ray, vec3 refractedLight) {
            vec2 tcube = intersectCube(origin, ray, vec3(-1.0, -poolHeight, -1.0), vec3(1.0, 2.0, 1.0));
            origin += ray * tcube.y;
            float tplane = (-origin.y - 1.0) / refractedLight.y;
            return origin + refractedLight * tplane;
        }
        
        void main() {
            vec4 info = texture2D(water, gl_Vertex.xy * 0.5 + 0.5);
            info.ba *= 0.5;
            vec3 normal = vec3(info.b, sqrt(1.0 - dot(info.ba, info.ba)), info.a);
            
            /* project the vertices along the refracted vertex ray */
            vec3 refractedLight = refract(-light, vec3(0.0, 1.0, 0.0), IOR_AIR / IOR_WATER);
            ray = refract(-light, normal, IOR_AIR / IOR_WATER);
            oldPos = project(gl_Vertex.xzy, refractedLight, refractedLight);
            newPos = project(gl_Vertex.xzy + vec3(0.0, info.r, 0.0), ray, refractedLight);

            gl_Position = vec4(0.75 * (newPos.xz + refractedLight.xz / refractedLight.y), 0.0, 1.0);
        }""", Renderer._helper_functions + """
        varying vec3 oldPos;
        varying vec3 newPos;
        varying vec3 ray;
        void main() {
            /* if the triangle gets smaller, it gets brighter, and vice versa */
            float oldArea = length(dFdx(oldPos)) * length(dFdy(oldPos));
            float newArea = length(dFdx(newPos)) * length(dFdy(newPos));
            gl_FragColor = vec4(oldArea / newArea * 0.2, 1.0, 0.0, 0.0);

            vec3 refractedLight = refract(-light, vec3(0.0, 1.0, 0.0), IOR_AIR / IOR_WATER);            
            /* compute a blob shadow and make sure we only draw a shadow if the player is blocking the light */
            vec3 dir = (sphereCenter - newPos) / sphereRadius;
            vec3 area = cross(dir, refractedLight);
            float shadow = dot(area, area);
            float dist = dot(dir, -refractedLight);
            shadow = 1.0 + (shadow - 1.0) / (0.05 + dist * 0.025);
            shadow = clamp(1.0 / (1.0 + exp(-shadow)), 0.0, 1.0);
            shadow = mix(1.0, shadow, clamp(dist * 2.0, 0.0, 1.0));
            gl_FragColor.g = shadow;
            
            /* shadow for the rim of the pool */
            vec2 t = intersectCube(newPos, -refractedLight, vec3(-1.0, -poolHeight, -1.0), vec3(1.0, 2.0, 1.0));
            gl_FragColor.r *= 1.0 / (1.0 + exp(-200.0 / (1.0 + 10.0 * (t.y - t.x)) * (newPos.y - refractedLight.y * t.y - 2.0 / 12.0)));
        }""", ctx)
        self.sky = Cubemap(local_textures("xpos.jpg"), local_textures("xneg.jpg"), local_textures("ypos.jpg"),
                           local_textures("ypos.jpg"), local_textures("zpos.jpg"), local_textures("zneg.jpg"), ctx)

    def update_caustics(self, matrices: Matrices, water: Water):
        self.caustic_texture.draw_to(self.ctx)
        self.ctx.clear()
        water.texture_a.use(0)
        self.caustics_shader.draw_mesh(self.water_mesh, matrices, unifroms={
            "light": self.light_dir,
            "water": 0,
            "sphereCenter": self.sphere_center,
            "sphereRadius": self.sphere_radius,
        })

    def render_water(self, matrices: Matrices, water: Water):
        water.texture_a.use(0)
        self.tile_texture.use(1)
        self.sky.use(2)
        self.caustic_texture.use(3)

        self.ctx.enable(moderngl.CULL_FACE)
        cull_face = ("front", "back")
        for idx in range(2):
            self.ctx.cull_face = cull_face[idx]
            self.water_shaders[idx].draw_mesh(self.water_mesh, matrices, unifroms={
                "light": self.light_dir,
                "water": 0,
                "tiles": 1,
                "sky": 2,
                "causticTex": 3,
                "eye": matrices.eye,
                "sphereCenter": self.sphere_center,
                "sphereRadius": self.sphere_radius,
            })
        self.ctx.disable(moderngl.CULL_FACE)

    def render_sphere(self, matrices: Matrices, water: Water):
        water.texture_a.use(0)
        self.caustic_texture.use(1)
        self.sphere_shader.draw_mesh(self.sphere_mesh, matrices, unifroms={
            "light": self.light_dir,
            "water": 0,
            "causticTex": 1,
            "sphereCenter": self.sphere_center,
            "sphereRadius": self.sphere_radius
        })

    def render_cude(self, matrices: Matrices, water: Water):
        self.ctx.enable(moderngl.CULL_FACE)
        water.texture_a.use(0)
        self.tile_texture.use(1)
        self.caustic_texture.use(2)
        self.cude_shader.draw_mesh(self.cube_mesh, matrices, unifroms={
            "light": self.light_dir,
            "water": 0,
            "tiles": 1,
            "causticTex": 2,
            "sphereCenter": self.sphere_center,
            "sphereRadius": self.sphere_radius
        })
        self.ctx.disable(moderngl.CULL_FACE)
