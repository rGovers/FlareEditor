using FlareEngine;
using System.Runtime.CompilerServices;

namespace FlareEditor
{
    public static class Workspace
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static string GetCurrentScene();
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void SetCurrentScene(string a_path);

        public static string CurrentScenePath
        {
            get
            {
                return GetCurrentScene();
            }
            set
            {
                SetCurrentScene(value);
            }
        }

        public static Scene GetScene()
        {
            string curScene = GetCurrentScene();
            if (string.IsNullOrEmpty(curScene))
            {
                return null;
            }

            Scene s = SceneData.GetScene(curScene);
            if (s == null)
            {
                SetCurrentScene(string.Empty);

                return null;
            }

            return s;
        }
    }
}