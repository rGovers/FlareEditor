using FlareEngine;
using FlareEngine.Definitions;
using System;
using System.Collections.Generic;
using System.Reflection;

namespace FlareEditor
{
    public class AssetProperties
    {
        static PropertiesWindow                     m_defaultWindow;

        static Dictionary<string, PropertiesWindow> m_windows;

        static string                               m_defName;

        internal static void Init()
        {
            m_defaultWindow = new PropertiesWindow();
            m_windows = new Dictionary<string, PropertiesWindow>();

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
                                m_windows.Add(att.OverrideType.ToString(), window);
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
                    m_defName = def.DefName;

                    return;
                }
            }

            m_defName = null;
        }

        static void OnGUI()
        {
            if (!string.IsNullOrWhiteSpace(m_defName))
            {
                Def def = DefLibrary.GetDef(m_defName);
                if (def != null)
                {
                    string type = def.GetType().ToString();
                    if (m_windows.ContainsKey(type))
                    {
                        m_windows[type].OnGUI(def);
                    }
                    else
                    {
                        m_defaultWindow.OnGUI(def);
                    }
                }
            }
        }
    }
}