using FlareEngine;
using FlareEngine.Mod;

namespace @ProjectName@
{
    public class @ScriptName@ : AssemblyControl
    {
        public override void Init()
        {
            // Assembly Initialization
            Logger.Message("Init");
        }

        public override void Update()
        {
            // Assembly Update
        }

        public override void Close()
        {
            // Assembly Shutdown
            Logger.Message("Shutdown");
        }
    }
}
