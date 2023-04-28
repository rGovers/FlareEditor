using FlareEngine;
using System.Collections.Generic;
using System.IO;
using System.Xml;

namespace FlareEditor
{
    public static class SceneData
    {
        static Dictionary<string, Scene> m_scenes;

        internal static void Init()
        {
            m_scenes = new Dictionary<string, Scene>();
        }

        public static Scene GetScene(string a_path)
        {
            if (m_scenes.ContainsKey(a_path))
            {
                return m_scenes[a_path];
            }

            Logger.Error($"FlareEditorCS: Could not find scene {a_path}");

            return null;
        }

        static void LoadScenes(byte[][] a_data, string[] a_paths)
        {
            uint count = (uint)a_paths.LongLength;   

            for (uint i = 0; i < count; ++i)
            {
                string path = a_paths[i];

                MemoryStream stream = new MemoryStream(a_data[i]);

                XmlDocument doc = new XmlDocument();
                doc.Load(stream);

                if (m_scenes.ContainsKey(path))
                {
                    Scene s = m_scenes[path];
                    if (s != null && !s.IsDisposed)
                    {
                        s.Dispose();
                    }

                    m_scenes[path] = new Scene(doc);
                }
                else
                {
                    m_scenes.Add(path, new Scene(doc));
                }
            }
        }
    }
}