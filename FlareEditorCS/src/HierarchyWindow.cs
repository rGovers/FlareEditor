using System.Collections.Generic;
using FlareEngine;
using FlareEngine.Definitions;

namespace FlareEditor
{
    public static class HierarchyWindow
    {
        static ulong s_startID = uint.MaxValue;

        static void Select(ulong a_id, List<SelectionObject> a_selections)
        {
            List<SelectionObject> selection = new List<SelectionObject>();

            if (GUI.CtrlModifier)
            {
                selection = Workspace.Selection;
            }

            int index = 0;
            int sIndex = int.MaxValue;
            for (int i = 0; i < a_selections.Count; ++i)
            {
                SelectionObject obj = a_selections[i];

                ulong id = obj.ID;

                if (id == s_startID)
                {
                    sIndex = i;
                }

                if (id == a_id)
                {
                    if (!selection.Contains(obj))
                    {
                        selection.Add(obj);
                    }

                    index = i;

                    break;
                }
            }

            if (GUI.ShiftModifier)
            {
                if (index < sIndex)
                {
                    for (int i = index + 1; i < a_selections.Count; ++i)
                    {
                        SelectionObject obj = a_selections[i];

                        if (!selection.Contains(obj))
                        {
                            selection.Add(obj);
                        }

                        if (obj.ID == s_startID)
                        {
                            break;
                        }
                    }
                }
                else
                {
                    for (int i = index - 1; i >= sIndex; --i)
                    {
                        SelectionObject obj = a_selections[i];

                        if (!selection.Contains(obj))
                        {
                            selection.Add(obj);
                        }
                    }
                }
            }

            Workspace.Selection = selection;

            s_startID = a_id;
        }

        static void DisplayObject(GameObjectDef a_def, ref ulong a_selectionID, ref List<SelectionObject> a_selectionList, bool a_scene)
        {
            if (a_def == null)
            {
                return;
            }

            if (a_scene && !a_def.IsSceneDef)
            {
                return;
            }

            string name = a_def.DefName;

            List<GameObjectDef> sChildren = new List<GameObjectDef>();
            if (a_scene)
            {
                foreach (GameObjectDef def in a_def.Children)
                {
                    if (def.IsSceneDef)
                    {
                        sChildren.Add(def);
                    }
                }
            }
            else
            {
                foreach (GameObjectDef def in a_def.Children)
                {
                    sChildren.Add(def);
                }
            }
        
            uint childCount = (uint)sChildren.Count;

            ulong id = Workspace.GetID(a_def);

            a_selectionList.Add(new SelectionObject()
            {
                ID = id,
                SelectionMode = SelectionObjectMode.GameObjectDef,
                GameObject = a_def
            });

            string idStr = $"##[{id}]{name}";
            GUI.PushID(idStr);

            if (childCount > 0)
            {
                bool show = GUI.Node(idStr);

                GUI.SameLine();

                if (GUI.Selectable(name))
                {
                    a_selectionID = id;
                }

                if (show)
                {   
                    foreach (GameObjectDef def in sChildren)
                    {
                        DisplayObject(def, ref a_selectionID, ref a_selectionList, a_scene);
                    }

                    GUI.PopNode();
                }
            }
            else
            {
                GUI.NIndent();

                if (GUI.Selectable(name))
                {
                    a_selectionID = id;
                }

                GUI.Unindent();
            }

            GUI.PopID();
        }

        static void OnGUI()
        {
            Scene scene = Workspace.GetScene();

            if (scene == null)
            {
                return;
            }

            List<SelectionObject> selectionList = new List<SelectionObject>();
            ulong selectionID = ulong.MaxValue;

            foreach (SceneObject obj in scene.SceneObjects)
            {
                string name = obj.DefName;

                GameObjectDef def = DefLibrary.GetDef<GameObjectDef>(name);
                if (def == null)
                {
                    continue;
                }

                ulong id = Workspace.GetID(obj);
                string idStr = $"##[{id}]{obj.DefName}";

                bool show = false;
                if (def.IsSceneDef)
                {
                    show = GUI.Node(idStr);

                    GUI.SameLine();
                }

                GUI.PushID(idStr);

                selectionList.Add(new SelectionObject()
                {
                    ID = id,
                    SelectionMode = SelectionObjectMode.SceneObject,
                    SceneObject = obj
                });

                if (GUI.Selectable(name))
                {
                    selectionID = id;
                }

                if (show)
                {
                    DisplayObject(def, ref selectionID, ref selectionList, true);

                    GUI.PopNode();
                }

                GUI.PopID();
            }

            if (selectionID != ulong.MaxValue)
            {
                Select(selectionID, selectionList);
            }
        }
    }
}