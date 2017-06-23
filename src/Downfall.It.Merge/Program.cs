using System;
using System.Diagnostics;

namespace Downfall.It.Merge
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Hello World!");

			PressAnyKeyToContinue();
		}

		[Conditional("DEBUG")]
		private static void PressAnyKeyToContinue()
		{
			if (Console.IsInputRedirected == false)
			{
				Console.WriteLine("Press any key to continue.");
				Console.ReadKey();
			}
		}
    }
}
