using System;
using System.Diagnostics;
using System.IO;
using System.Xml;

namespace FlareEditor.BuildEngine
{
    public class Program
    {
        // On Windows seems to need a main function and be executable to work
        // Needs it even if it is not used for some odd reason
        static void Main(string[] a_args)
        {

        }

        public static bool Build(string a_path, string a_name)
        {
            string cacheDir = Path.Combine(a_path, ".cache");
            string projectDir = Path.Combine(a_path, "Project");

            if (!Directory.Exists(cacheDir))
            {
                Directory.CreateDirectory(cacheDir);
            }
            if (!Directory.Exists(projectDir))
            {
                Directory.CreateDirectory(projectDir);
            }

            string editorProjectName = a_name + "Editor";

            string projectPath = Path.Combine(cacheDir, a_name + ".csproj");

            XmlDocument projectDoc = null;
            XmlDocument editorProjectDoc = null;

            string[] projectSources = ProjectGenerator.GetScripts(projectDir);
            if (projectSources != null)
            {
                projectDoc = ProjectGenerator.GenerateProject("Core/Assemblies", a_name, projectSources, new string[] { "System", "System.Xml", "FlareEngine" });
                projectDoc.Save(projectPath);
            }

            string[] editorProjectSources = ProjectGenerator.GetEditorScripts(projectDir);
            if (editorProjectSources != null)
            {
                editorProjectDoc = ProjectGenerator.GenerateProject("Editor", editorProjectName, editorProjectSources, new string[] { "System", "System.Xml", "FlareEngine", "FlareEditor" });
                editorProjectDoc.Save(Path.Combine(cacheDir, editorProjectName + ".csproj"));
            }

            if (projectDoc != null)
            {
                try
                {
                    // Msbuild was kicking my ass..... again....
                    // I love having obscure issues finding a bug that has been reported and not fixed for 5 years thank you Microsoft
                    // Basically there is a fun interaction between the Mono runtime and the build engine that causes a crash
                    // Despite being deprecated it just works
                    // I am aware of the security risks of doing it as a process I just dont care anymore
                    // Atleast it is not that big of a concern when you are able to execute arbitrary code
                    using (Process proc = new Process())
                    {
                        proc.StartInfo.UseShellExecute = false;
                        proc.StartInfo.FileName = "./lib/mono/4.5/xbuild.exe";
                        proc.StartInfo.CreateNoWindow = true;
                        proc.StartInfo.Arguments = projectPath;
                        proc.StartInfo.RedirectStandardOutput = true;

                        proc.Start();

                        proc.WaitForExit();

                        string output = proc.StandardOutput.ReadToEnd();

                        if (!BuildOutputParser.ParseOutput(output))
                        {
                            return false;
                        }
                    }
                }
                catch (Exception)
                {
                    Logger.Error("Error Starting Process");

                    return false;
                }
            }

            return true;   
        }
    }   
}