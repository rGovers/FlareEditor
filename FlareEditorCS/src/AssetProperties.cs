using FlareEditor.Properties;
using FlareEngine;
using FlareEngine.Definitions;
using System;
using System.Collections.Generic;
using System.Reflection;

namespace FlareEditor
{
    public static class AssetProperties
    {
        static PropertiesWindow                     s_defaultWindow;

        static Dictionary<string, PropertiesWindow> s_windows;

        static string                               s_defName;

        internal static void Init()
        {
            s_defaultWindow = new PropertiesWindow();
            s_windows = new Dictionary<string, PropertiesWindow>();

            Assembly[] assemblies = AppDomain.CurrentDomain.GetAssemblies();

            foreach (Assembly asm in assemblies)
            {
                Type[] types = asm.GetTypes();
                foreach (Type type in types)
                {
                    PWindowAttribute att = type.GetCustomAttribute<PWindowAttribute>();
                    if (att != null)
                    {
                        if (type.IsSubclassOf(typeof(PropertiesWindow)))
                        {
                            PropertiesWindow window = Activator.CreateInstance(type) as PropertiesWindow;
                            if (window != null)
                            {
                                s_windows.Add(att.OverrideType.ToString(), window);
                            }
                            else
                            {
                                Logger.Error($"FlareEditorCS: Unabled to create PropertiesWindow of type {type}");
                            }
                        }
                        else
                        {
                            Logger.Error($"FlareEditorCS: {type} has Attribute PWindow and is not inherited from PropertiesWindow");
                        }
                    }
                }
            }
        }

        static void PushDef(string a_path)
        {
            List<Def> defs = DefLibrary.GetDefs();

            foreach (Def def in defs)
            {
                if (def.DefPath == a_path)
                {
                    s_defName = def.DefName;

                    return;
                }
            }

            s_defName = null;
        }

        static void OnGUI()
        {
            if (!string.IsNullOrWhiteSpace(s_defName))
            {
                Def def = DefLibrary.GetDef(s_defName);
                if (def != null)
                {
                    string type = def.GetType().ToString();
                    if (s_windows.ContainsKey(type))
                    {
                        s_windows[type].OnGUI(def);
                    }
                    else
                    {
                        s_defaultWindow.OnGUI(def);
                    }
                }
            }
        }
    }
}