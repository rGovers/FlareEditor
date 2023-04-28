using FlareEngine;
using FlareEngine.Definitions;

namespace FlareEditor
{
    public static class HierarchyWindow
    {
        static void DisplayObject(GameObjectDef a_def, ref uint a_index)
        {
            ++a_index;

            string name = a_def.DefName;

            uint childCount = (uint)a_def.Children.Count;

            if (childCount > 0)
            {
                string id = $"##[{a_index}]{name}";

                bool show = GUI.Node(id);

                GUI.SameLine();

                GUI.Selectable(name);

                if (show)
                {   
                    GUI.PushID(id);

                    foreach (GameObjectDef def in a_def.Children)
                    {
                        DisplayObject(def, ref a_index);
                    }

                    GUI.PopID();

                    GUI.PopNode();
                }
            }
            else
            {
                GUI.NIndent();

                GUI.Selectable(name);

                GUI.Unindent();
            }
        }

        static void OnGUI()
        {
            Scene scene = Workspace.GetScene();

            if (scene == null)
            {
                return;
            }

            uint index = 0;
            foreach (SceneObject obj in scene.SceneObjects)
            {
                GameObjectDef def = DefLibrary.GetDef<GameObjectDef>(obj.DefName);

                DisplayObject(def, ref index);
            }
        }
    }
}