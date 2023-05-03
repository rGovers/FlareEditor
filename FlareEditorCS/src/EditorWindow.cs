using FlareEditor.Editor;
using FlareEngine;
using FlareEngine.Definitions;
using FlareEngine.Maths;
using System;
using System.Collections.Generic;
using System.Reflection;

namespace FlareEditor
{
    public static class EditorWindow
    {
        static Dictionary<Type, EditorDisplay> s_componentLookup;

        internal static void Init()
        {
            s_componentLookup = new Dictionary<Type, EditorDisplay>();

            Assembly[] assemblies = AppDomain.CurrentDomain.GetAssemblies();
            foreach (Assembly asm in assemblies)
            {
                Type[] types = asm.GetTypes();
                foreach (Type t in types)
                {
                    EDisplayAttribute att = t.GetCustomAttribute<EDisplayAttribute>();
                    if (att != null)
                    {
                        s_componentLookup.Add(att.OverrideType, Activator.CreateInstance(t) as EditorDisplay);
                    }
                }
            }
        }

        static void RenderComponents(GameObjectDef a_def, bool a_selected, Matrix4 a_transform)
        {
            foreach (ComponentDef c in a_def.Components)
            {
                Type t = c.ComponentType;
                if (s_componentLookup.ContainsKey(t))
                {
                    EditorDisplay disp = s_componentLookup[t];
                    if (disp != null)
                    {
                        disp.Render(a_selected, c, a_transform);
                    }
                    else
                    {
                        Logger.Error($"FlareEditorCS: Null editor display {t}");
                    }
                }
            }
        }

        static void RenderGameObjects(GameObjectDef a_gameObjectDef, Matrix4 a_parentTransform)
        {
            Matrix4 mat = Matrix4.FromTransform(a_gameObjectDef.Translation, a_gameObjectDef.Rotation, a_gameObjectDef.Scale) * a_parentTransform;

            RenderComponents(a_gameObjectDef, true, mat);

            foreach (GameObjectDef c in a_gameObjectDef.Children)
            {
                RenderGameObjects(c, mat);
            }
        }

        static void OnGUI()
        {
            Scene scene = Workspace.GetScene();

            if (scene == null)
            {
                return;
            }

            foreach (SceneObject obj in scene.SceneObjects)
            {
                GameObjectDef def = DefLibrary.GetDef(obj.DefName) as GameObjectDef;

                Matrix4 mat = Matrix4.FromTransform(obj.Translation, obj.Rotation, obj.Scale);

                if (def != null)
                {
                    RenderGameObjects(def, mat);
                }
            }
        }
    }
}