namespace FlareEditor
{
    internal class Program
    {
        static void Load()
        {
            Logger.Message("Editor Loaded");
        }

        static void Update(double a_delta)
        {
            
        }

        static void Unload()
        {
            // AppDomain.Unload(AppDomain.CurrentDomain);
        }
        
        // On Windows seems to need a main function and be executable to work
        // Needs it even if it is not used for some odd reason
        static void Main(string[] a_args)
        {

        }
    }
}