using System.Runtime.CompilerServices;

namespace FlareEditor.BuildEngine
{
    internal static class Logger
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Message(string a_message);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Warning(string a_message);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Error(string a_message);
    }
}