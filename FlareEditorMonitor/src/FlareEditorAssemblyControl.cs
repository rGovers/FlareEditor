using FlareEngine;
using FlareEngine.Mod;

namespace FlareEditor
{
    public class FlareEditorAssemblyControl : AssemblyControl
    {
        public override void Init()
        {
            Logger.Message("FlareEditor: Init");
        }
        public override void Update()
        {

        }
        public override void Close()
        {
            Logger.Message("FlareEditor: Close");
        }
    }
}