using System.Runtime.CompilerServices;

namespace FlareEditor.BuildEngine
{
    internal static class Logger
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void PushMessage(string a_message);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void PushWarning(string a_message);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void PushError(string a_message);

        public static void Message(string a_msg)
        {
            PushMessage(a_msg);
        }
        public static void Warning(string a_msg)
        {
            PushWarning(a_msg);
        }
        public static void Error(string a_msg)
        {
            PushError(a_msg);
        }
    }
}