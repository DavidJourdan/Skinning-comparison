import bpy
from mathutils import Vector

import struct
import sys
from pathlib import Path

out_path = input("Output file: ")

out_path = Path(out_path).with_suffix(".nocor")
print("Writing to " + str(out_path))

with open(out_path, "wb") as f:
    # File ID
    id = b"NOCO"
    f.write(id)

    # Endianness
    end = 0
    f.write(struct.pack("I", end))

    # Mesh
    mesh = bpy.data.meshes[0]

    for obj in bpy.data.objects:
        if obj.type == "MESH":
            mesh_obj = obj

    mesh_mat = mesh_obj.matrix_local
    mesh.transform(mesh_mat)

    def write_vec(a):
        l = [a[1],a[2],a[0]]
        for x in l:
            f.write(struct.pack("f", x))

    # Vertex array
    vertices = mesh.vertices

    vertex_count = len(vertices)

    f.write(vertex_count.to_bytes(4, "little"))

    for vertex in vertices:
        write_vec(vertex.co)
        write_vec(vertex.normal)

    # Triangle array
    polygons = mesh.polygons

    triangle_count = len(polygons)

    f.write(triangle_count.to_bytes(4, "little"))

    for poly in polygons:
        vertices = poly.vertices
        for i in vertices:
            f.write(i.to_bytes(4, "little"))

    # Bones
    arm = bpy.data.armatures[0]

    for obj in bpy.data.objects:
        if obj.type == "ARMATURE":
            arm_obj = obj

    arm_mat = arm_obj.matrix_world
    #arm.transform(arm_mat)
    bones = arm.bones

    bone_count = len(bones)

    f.write(struct.pack("I", bone_count))

    for bone in bones:
        head = bone.head_local
        tail = bone.tail_local
        children = [bones.find(x.name) for x in bone.children]

        write_vec(head)
        write_vec(tail)

        child_count = len(children)
        f.write(struct.pack("I", child_count))
        for i in children:
            f.write(struct.pack("I", i))

    # Bone weights
    vertices = mesh.vertices

    for vertex in vertices:
        grps = vertex.groups
        weight_count = len(grps)

        f.write(struct.pack("I", weight_count))

        for vge in grps:
            index = vge.group
            w = vge.weight
            f.write(struct.pack("I", index))
            f.write(struct.pack("f", w))
