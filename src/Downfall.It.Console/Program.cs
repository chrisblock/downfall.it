using System.Diagnostics;
using System.Text;

using DownfallItCore;

namespace Downfall.It.Console
{
	using Console = System.Console;

	class Program
	{
		static void Main(string[] args)
		{
			var merger = new Merger();

			var subtitles = @"
1
00:00:00,285 --> 00:00:03,289
Hello world
";

			var bytes = Encoding.UTF8.GetBytes(subtitles);

			var video = merger.AddSubtitles(bytes);

			var temp = Encoding.UTF8.GetString(video);

			Console.WriteLine(temp);

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
