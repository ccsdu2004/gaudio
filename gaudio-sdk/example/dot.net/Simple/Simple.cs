using System;
using gaudio; 

namespace Simple
{
    class Simple
    {
        [STAThread]
        static void Main(string[] args)
        {
            if(args.Length == 0)
                return;

            gaudio.GAudio.gaudio_init("addons");
            int error = gaudio.GAudio.gaudio_error_get();
            if(error != 0)
                return;

            IntPtr source = gaudio.GAudio.gaudio_source_create_from_file(args[0], 0);
            if(source == IntPtr.Zero)
            {
                Console.WriteLine("create source failed.");
                gaudio.GAudio.gaudio_deinit();
                return;
            }
  
            gaudio.GAudio.gaudio_source_start(source,1);
            
            Console.WriteLine("Press any key to exit!");
            Console.ReadKey(true);
            gaudio.GAudio.gaudio_source_destroy(source);
            gaudio.GAudio.gaudio_deinit();
        }
    }
}
