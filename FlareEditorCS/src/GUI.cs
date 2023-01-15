using FlareEngine.Maths;
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FlareEditor
{
    public class GUI
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static int GetInt(string a_label, IntPtr a_int);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static uint GetUInt(string a_label, IntPtr a_int);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static uint GetFloat(string a_label, IntPtr a_float);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static uint GetVec2(string a_label, IntPtr a_vec);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static uint GetVec3(string a_label, IntPtr a_vec);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static uint GetVec4(string a_label, IntPtr a_vec);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static string GetString(string a_label, string a_str);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static uint GetStringList(string a_label, string[] a_strings, IntPtr a_selected);

        public static bool EnumField(string a_label, ref object a_enum)
        {
            Type type = a_enum.GetType();
            string[] list = Enum.GetNames(type);

            string selectedStr = Enum.GetName(type, a_enum);
            int selectedValue = 0;
            for (int i = 0; i < list.Length; ++i)
            {
                if (list[i] == selectedStr)
                {
                    selectedValue = i;

                    break;
                }
            }

            GCHandle handle = GCHandle.Alloc(selectedValue, GCHandleType.Pinned);
            
            bool ret = false;
            if (GetStringList(a_label, list, handle.AddrOfPinnedObject()) != 0)
            {
                ret = true;
                a_enum = Enum.Parse(type, list[(int)handle.Target]);
            }

            handle.Free();

            return ret;
        }

        public static bool IntField(string a_label, ref int a_int)
        {
            GCHandle handle = GCHandle.Alloc(a_int, GCHandleType.Pinned);

            bool ret = false;
            if (GetInt(a_label, handle.AddrOfPinnedObject()) != 0)
            {
                ret = true;
                a_int = (int)handle.Target;
            }

            handle.Free();

            return ret;
        }
        public static bool UIntField(string a_label, ref uint a_int)
        {
            GCHandle handle = GCHandle.Alloc(a_int, GCHandleType.Pinned);

            bool ret = false;
            if (GetUInt(a_label, handle.AddrOfPinnedObject()) != 0)
            {
                ret = true;
                a_int = (uint)handle.Target;
            }

            handle.Free();

            return ret;
        }

        public static bool FloatField(string a_label, ref float a_float)
        {
            GCHandle handle = GCHandle.Alloc(a_float, GCHandleType.Pinned);

            bool ret = false;
            if (GetFloat(a_label, handle.AddrOfPinnedObject()) != 0)
            {
                ret = true;
                a_float = (float)handle.Target;
            }

            handle.Free();

            return ret;
        }

        public static bool Vec2Field(string a_label, ref Vector2 a_vec)
        {
            GCHandle handle = GCHandle.Alloc(a_vec, GCHandleType.Pinned);

            bool ret = false;
            if (GetVec2(a_label, handle.AddrOfPinnedObject()) != 0)
            {
                ret = true;
                a_vec = (Vector2)handle.Target;
            }

            handle.Free();

            return ret;
        }
        public static bool Vec3Field(string a_label, ref Vector3 a_vec)
        {
            GCHandle handle = GCHandle.Alloc(a_vec, GCHandleType.Pinned);

            bool ret = false;
            if (GetVec3(a_label, handle.AddrOfPinnedObject()) != 0)
            {
                ret = true;
                a_vec = (Vector3)handle.Target;
            }

            handle.Free();

            return ret;
        }
        public static bool Vec4Field(string a_label, ref Vector4 a_vec) 
        {
            GCHandle handle = GCHandle.Alloc(a_vec, GCHandleType.Pinned);

            bool ret = false;
            if (GetVec4(a_label, handle.AddrOfPinnedObject()) != 0)
            {
                ret = true;
                a_vec = (Vector4)handle.Target;
            }

            handle.Free();

            return ret;
        }
        public static bool StringField(string a_label, ref string a_str)
        {
            string ret = GetString(a_label, a_str);
            if (!string.IsNullOrEmpty(ret))
            {
                a_str = ret;
                
                return true;
            }

            return false;
        }
    }
}