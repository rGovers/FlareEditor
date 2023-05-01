using FlareEngine.Maths;
using System.Runtime.CompilerServices;

namespace FlareEditor
{
    public static class Gizmos
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void DrawLine(Vector3 a_start, Vector3 a_end, float a_width, Vector4 a_color);

        public static void DrawLine(Vector3 a_start, Vector3 a_end, float a_width, Color a_color)
        {
            DrawLine(a_start, a_end, a_width, a_color.ToVector4());
        }
    }
}