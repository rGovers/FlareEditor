using FlareEngine.Definitions;
using System;
using System.Reflection;

namespace FlareEditor
{
    internal static class Program
    {
        static void Load()
        {
            Type defLibraryType = typeof(DefLibrary);
            MethodInfo initMethod = defLibraryType.GetMethod("Init", BindingFlags.Static | BindingFlags.NonPublic);
            initMethod.Invoke(null, new object[] { });

            AssetProperties.Init();
        }

        static void Update(double a_delta)
        {
            
        }

        static void Unload()
        {
            DefLibrary.Clear();

            // AppDomain.Unload(AppDomain.CurrentDomain);
        }
        
        // On Windows seems to need a main function and be executable to work
        // Needs it even if it is not used for some odd reason
        static void Main(string[] a_args)
        {

        }
    }
}