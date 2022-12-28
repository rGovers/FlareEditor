using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Xml;

namespace FlareEditor.BuildEngine
{
    public static class ProjectGenerator
    {
        static void GetScripts(ref List<string> a_files, string a_dir, string a_wDir)
        {
            if (!Directory.Exists(a_dir))
            {
                return;
            }

            string[] files = Directory.GetFiles(a_dir);
            foreach (string file in files)
            {
                if (Path.GetExtension(file) == ".cs")
                {
                    a_files.Add(Path.Combine(a_wDir, Path.GetFileName(file)));
                }
            }

            string[] dirs = Directory.GetDirectories(a_dir);
            foreach (string dir in dirs)
            {
                if (dir[0] != '.' && dir != "Editor")
                {
                    GetScripts(ref a_files, Path.Combine(a_dir, dir), Path.Combine(a_wDir, dir));
                }
            }
        }

        public static string[] GetScripts(string a_dir)
        {
            List<string> paths = new List<string>();

            GetScripts(ref paths, a_dir, "");

            if (paths.Count > 0)
            {
                return paths.ToArray();
            }

            return null;
        }
        public static string[] GetEditorScripts(string a_dir)
        {
            List<string> paths = new List<string>();

            GetScripts(ref paths, Path.Combine(a_dir, "Editor/"), "Editor");

            if (paths.Count > 0)
            {
                return paths.ToArray();
            }

            return null;
        }

        public static XmlDocument GenerateProject(string a_outPath, string a_assemblyName, string[] a_sources, string[] a_deps)
        {
            XmlDocument doc = new XmlDocument();

            doc.AppendChild(doc.CreateXmlDeclaration("1.0", "utf-8", null));

            XmlElement root = doc.CreateElement("Project");
            doc.AppendChild(root);
            // root.SetAttribute("ToolsVersion", "14.0");
            root.SetAttribute("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");

            XmlElement propertyGroup = doc.CreateElement("PropertyGroup");
            root.AppendChild(propertyGroup);

            XmlElement configuationProperty = doc.CreateElement("Configuration");
            propertyGroup.AppendChild(configuationProperty);
            configuationProperty.SetAttribute("Condition", "'$(Configuration)' == ''");
            configuationProperty.InnerText = "Debug";

            XmlElement outputProperty = doc.CreateElement("OutputPath");
            propertyGroup.AppendChild(outputProperty);
            outputProperty.InnerText = a_outPath;

            XmlElement assemblyNameProperty = doc.CreateElement("AssemblyName");
            propertyGroup.AppendChild(assemblyNameProperty);
            assemblyNameProperty.InnerText = a_assemblyName;

            XmlElement targetFrameworkProperty = doc.CreateElement("TargetFrameworkVersion");
            propertyGroup.AppendChild(targetFrameworkProperty);
            targetFrameworkProperty.InnerText = "v4.5";

            XmlElement outputTypeProperty = doc.CreateElement("OutputType");
            propertyGroup.AppendChild(outputTypeProperty);
            outputTypeProperty.InnerText = "Library";

            XmlElement debugPropertyGroup = doc.CreateElement("PropertyGroup");
            root.AppendChild(debugPropertyGroup);
            debugPropertyGroup.SetAttribute("Condition", "'$(Configuration)'=='Debug'");

            XmlElement debugOutpath = doc.CreateElement("OutputPath");
            debugPropertyGroup.AppendChild(debugOutpath);
            debugOutpath.InnerText = a_outPath;

            XmlElement debugSymbols = doc.CreateElement("DebugSymbols");
            debugPropertyGroup.AppendChild(debugSymbols);
            debugSymbols.InnerText = "true";

            XmlElement debugType = doc.CreateElement("DebugType");
            debugPropertyGroup.AppendChild(debugType);
            debugType.InnerText = "full";

            XmlElement debugOptimize = doc.CreateElement("Optimize");
            debugPropertyGroup.AppendChild(debugOptimize);
            debugOptimize.InnerText = "false";

            XmlElement releasePropertyGroup = doc.CreateElement("PropertyGroup");
            root.AppendChild(releasePropertyGroup);
            releasePropertyGroup.SetAttribute("Condition", "'$(Configuration)'=='Release'");

            XmlElement releaseOutpath = doc.CreateElement("OutputPath");
            releasePropertyGroup.AppendChild(releaseOutpath);
            releaseOutpath.InnerText = a_outPath;

            XmlElement releaseSymbols = doc.CreateElement("DebugSymbols");
            releasePropertyGroup.AppendChild(releaseSymbols);
            releaseSymbols.InnerText = "false";

            XmlElement releaseOptimize = doc.CreateElement("Optimize");
            releasePropertyGroup.AppendChild(releaseOptimize);
            releaseOptimize.InnerText = "true";

            if (a_sources != null)
            {
                XmlElement compileItemGroup = doc.CreateElement("ItemGroup");
                root.AppendChild(compileItemGroup);

                foreach (string source in a_sources)
                {
                    XmlElement compileSource = doc.CreateElement("Compile");
                    compileItemGroup.AppendChild(compileSource);
                    compileSource.SetAttribute("Include", Path.Combine("../Project/", source));
                }
            }

            if (a_deps != null)
            {
                XmlElement referenceItemGroup = doc.CreateElement("ItemGroup");
                root.AppendChild(referenceItemGroup);

                foreach (string dep in a_deps)
                {
                    XmlElement reference = doc.CreateElement("Reference");
                    referenceItemGroup.AppendChild(reference);

                    switch (dep)
                    {
                    case "FlareEngine":
                    {
                        string loc = Assembly.GetExecutingAssembly().Location;
                        string dir = Path.GetDirectoryName(loc);

                        reference.SetAttribute("Include", "FlareCS");

                        XmlElement hintPath = doc.CreateElement("HintPath");
                        reference.AppendChild(hintPath);
                        hintPath.InnerText = Path.Combine(dir, "FlareCS.dll");

                        XmlElement priv = doc.CreateElement("Private");
                        reference.AppendChild(priv);
                        priv.InnerText = "true";

                        break;
                    }
                    case "FlareEditor":
                    {
                        string loc = Assembly.GetExecutingAssembly().Location;
                        string dir = Path.GetDirectoryName(loc);

                        reference.SetAttribute("Include", "FlareEditorCS");

                        XmlElement hintPath = doc.CreateElement("HintPath");
                        reference.AppendChild(hintPath);
                        hintPath.InnerText = Path.Combine(dir, "FlareEditorCS.dll");

                        XmlElement priv = doc.CreateElement("Private");
                        reference.AppendChild(priv);
                        priv.InnerText = "true";

                        break;
                    }
                    default:
                    {
                        reference.SetAttribute("Include", dep);

                        break;
                    }
                    }
                }
            }

            XmlElement import = doc.CreateElement("Import");
            root.AppendChild(import);
            import.SetAttribute("Project", "$(MSBuildToolsPath)/Microsoft.CSharp.targets");

            return doc;
        }
    }
}