import bpy
import struct
import bmesh
import mathutils
import math

version = {
    "major" : 1,
    "minor" : 1
}

magic_number = 0xCA770770

record_types = {
    "done" : 0x00,
    "version" : 0x01,
    "vertexAttrib" : 0x10,
    "vertex" : 0x11,
    "newBinding" : 0x20,
    "triangle" : 0x21,
    "newBone" : 0x30,
    "parentBone" : 0x31,
    "nameBone" : 0x32,
    "newAction" : 0x40,
    "newCurve" : 0x41,
    "curveSample" : 0x42,
    "playbackData" : 0x43
}

def write_record(f, recordType, data):
    f.write(struct.pack("<B", recordType))
    f.write(data)

def write_model(context, filepath, use_some_setting):
    print("writing tmd model...")
    coord_transform = mathutils.Matrix(
                [[-1, 0, 0, 0],
                 [0, 0, 1, 0],
                 [0, 1, 0, 0],
                 [0, 0, 0, 1]])
    coord_transform_3x3 = coord_transform.to_3x3()
    f = open(filepath, 'wb')
    write_record(
        f, record_types['version'],
        struct.pack('<IBB', magic_number, version['major'], version['minor']))

    # Data to be exported.
    vertices = []
    normals = []
    texCos = []
    boneWeights = [[] for i in range(0, 8)]
    
    top_vertex_index = 1
    
    object = bpy.context.active_object
    if object.type not in ['MESH', 'CURVE', 'SURFACE', 'FONT', 'META']:
        return
    mesh = object.to_mesh(
        scene=bpy.context.scene,
        apply_modifiers=False,
        settings='RENDER')
    
    # Find if the mesh has an armature.
    armature = object.find_armature()
    if armature:
        armature = armature.copy()
        armature.data = armature.data.copy()
        
        # Make a copy of it and convert it into the correct space.
        armature_data = armature.data
        armature_data.transform(coord_transform * armature.matrix_world)
        bone_name_indices = {}
        if armature:
            for i, bone in enumerate(armature_data.bones):
                bone_name_indices[bone.name] = i
    
    # Triangulate.
    bm = bmesh.new()
    bm.from_mesh(mesh)
    bmesh.ops.triangulate(bm, faces=bm.faces)
    bm.to_mesh(mesh)
    bm.free()
    
    # Calculate the normals for each vertex.
    mesh.transform(coord_transform * object.matrix_world)
    mesh.calc_normals_split()
    
    loops = {}
    loop_to_vertex = {}
    
    # Iterate through each loop, find where we need to create new vertices or use old ones.
    for loop in mesh.loops:
        n = loop.normal.copy()
        v = loop.vertex_index
        if mesh.uv_layers.active != None:
            t = mesh.uv_layers.active.data[loop.index].uv.copy()
            t.freeze()
        else:
            t = None
        n.freeze()
        k = (v, n, t)
        # If the vertex has not been added yet, add it to the list.
        if k not in loops:
            loops[k] = top_vertex_index
            vertex_index = top_vertex_index

            top_vertex_index += 1
            weights = None
            # Find the weights for each bone.
            if armature:
                weights = [0.0 for i in range(0, len(armature_data.bones))]
                for group in mesh.vertices[v].groups:
                    group_name = object.vertex_groups[group.group].name
                    if group_name in bone_name_indices:
                        weights[bone_name_indices[group_name]] = group.weight
            
            vertices.append(mesh.vertices[v].co.copy())
            normals.append(n)
            texCos.append(t)
            
            if weights:
                for i in range(0, len(weights), 4):
                    if i < len(boneWeights) * 4:
                        components = weights[i:i+4]
                        components = components + [0.0] * (4 - len(components))
                        boneWeights[i // 4].append(mathutils.Vector(components))
                    else:
                        # Error, too many bones.
                        print("failed to export tmd: too many bones.")
        else:
            vertex_index = loops[k]
        
        loop_to_vertex[loop.index] = vertex_index
    
    # Iterate through each material, creating a binding for each one.
    for i, material in enumerate(mesh.materials):
        write_record(f, record_types['newBinding'],
            bytes(material.name + '\0', encoding="utf-8"))
        for poly in mesh.polygons:
            if poly.material_index == i:
                write_record(f, record_types['triangle'],
                    struct.pack('<III',
                        loop_to_vertex[poly.loop_indices[0]],
                        loop_to_vertex[poly.loop_indices[1]],
                        loop_to_vertex[poly.loop_indices[2]]))
    # Clean up mesh.
    bpy.data.meshes.remove(mesh)
    
    # Write the vertices.
    attribs = [b'vertex\0', b'normal\0', b'texCo\0']\
        + [bytes('boneWeights{}\0'.format(i + 1), encoding='utf-8')
            for i in range(0, len(boneWeights))]
    for i, array in enumerate([vertices, normals, texCos] + boneWeights):
        # Only write nonempty arrays.
        if array:
            write_record(f, record_types['vertexAttrib'], attribs[i])
            for vertex in array:
                if vertex:
                    x, y, z, w = vertex.to_4d().to_tuple()
                    write_record(f, record_types['vertex'],
                        struct.pack('<ffff', x, y, z, w))
                # If no coordinate was provided, use 0. For example, if missing texcos
                # for a certain object.
                else:
                    write_record(f, record_types['vertex'],
                        struct.pack('<ffff', 0, 0, 0, 0))
    
    # If the mesh has an armature, write the bones.
    if armature:
        bone_indices = {}
        for i, bone in enumerate(armature_data.bones):
            head = bone.head
            matrix = mathutils.Matrix.Translation(head)\
                    * bone.matrix.to_4x4()
            write_record(f, record_types['newBone'],
                struct.pack('<fffffffffffffffff',
                    bone.length,
                    matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
                    matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
                    matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3],
                    matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]))
            write_record(f, record_types['nameBone'],
                struct.pack("<I", i + 1)\
                + bytes(bone.name + '\0', encoding='utf-8'))
            bone_indices[bone] = i + 1
        for bone in armature_data.bones:
            if bone.parent:
                write_record(f, record_types['parentBone'],
                    struct.pack('<II',
                        bone_indices[bone],
                        bone_indices[bone.parent]))
        
        # Write armature actions.
        for action in bpy.data.actions:
            # Skip non armature actions.
            if action.id_root not in ['ARMATURE', 'OBJECT']:
                continue
            curves = [None for bone in armature_data.bones]
            frame_start = int(action.frame_range[0])
            frame_end = int(action.frame_range[1])
            
            for frame in range(frame_start, frame_end):
                for fcurve in action.fcurves:
                    if not fcurve.data_path.startswith("pose."):
                        continue
                    pose_bone = armature.path_resolve(
                        fcurve.data_path[:fcurve.data_path.rfind('.')])
                    bone = pose_bone.bone
                    bone_index = bone_indices[bone] - 1
                    if not curves[bone_index]:
                        curves[bone_index] = {
                            "samples" : [
                                mathutils.Matrix()\
                                for i in range(frame_start,\
                                                frame_end)],
                        }
                    armature.path_resolve(fcurve.data_path)[fcurve.array_index] =\
                        fcurve.evaluate(frame)
                    
                    head = bone.head
                    matrix = mathutils.Matrix.Translation(head)\
                              * bone.matrix.to_4x4()\
                              * pose_bone.matrix_basis
                    curves[bone_index]['samples'][frame - frame_start] = (
                            matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
                            matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
                            matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3],
                            matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3])
            
            # If there are no curves for this action, continue.
            if not any(curves):
                continue
            
            # Now, we write the action.
            write_record(f, record_types['newAction'],
                bytes(action.name + '\0', encoding='utf-8')
                + struct.pack('<I', frame_end - frame_start))
            for bone_index, curve in enumerate(curves):
                if curve:
                    write_record(f, record_types['newCurve'],
                        struct.pack('<B', bone_index + 1))
                    for sample in curve['samples']:
                        write_record(f, record_types['curveSample'],
                            struct.pack('<ffffffffffffffff', *sample))
    
    # Clean up armature.
    if armature:
        bpy.data.objects.remove(armature)
        bpy.data.armatures.remove(armature_data)

    write_record(f, record_types['done'], b'')
    f.close()

    return {'FINISHED'}


# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator


class ExportTmd(Operator, ExportHelper):
    """Export a tmd model."""
    bl_idname = "export_tmd.model"  # important since its how bpy.ops.import_test.some_data is constructed
    bl_label = "Export tmd"

    # ExportHelper mixin class uses this
    filename_ext = ".tmd"

    filter_glob = StringProperty(
            default="*.tmd",
            options={'HIDDEN'},
            maxlen=255,  # Max internal buffer length, longer would be clamped.
            )

    # List of operator properties, the attributes will be assigned
    # to the class instance from the operator settings before calling.
    use_setting = BoolProperty(
            name="Example Boolean",
            description="Example Tooltip",
            default=True,
            )

    type = EnumProperty(
            name="Example Enum",
            description="Choose between two items",
            items=(('OPT_A', "First Option", "Description one"),
                   ('OPT_B', "Second Option", "Description two")),
            default='OPT_A',
            )

    def execute(self, context):
        return write_model(context, self.filepath, self.use_setting)


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
    self.layout.operator(ExportSomeData.bl_idname, text="Export tmd")


def register():
    bpy.utils.register_class(ExportTmd)
    bpy.types.INFO_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(ExportTmd)
    bpy.types.INFO_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()

    # test call
    bpy.ops.export_tmd.model('INVOKE_DEFAULT')
