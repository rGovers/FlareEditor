using System;

namespace FlareEditor.BuildEngine
{
    public static class BuildOutputParser
    {
        public static bool ParseOutput(string a_string)
        {   
            string[] lines = a_string.Split(new char[]{ '\n' }, StringSplitOptions.RemoveEmptyEntries);

            bool error = false;

            foreach (string l in lines)
            {
                // Repeats the output after no need for it
                if (l.Contains("Build FAILED"))
                {
                    return false;
                }
                // Again repeats no need to continue
                else if (l.Contains("Build succeeded"))
                {
                    break;
                }
                else if (l.Contains("error"))
                {
                    error = true;

                    Logger.Error(l.TrimStart());
                }
                else if (l.Contains("warning"))
                {
                    Logger.Warning(l.TrimStart());
                }
            }

            return !error;
        }
    }
}