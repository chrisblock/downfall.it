#pragma once

using namespace System;
using namespace System::Text;

namespace Downfall
{
	namespace It
	{
		namespace Core
		{
			public ref class Merger
			{
			public:
				array<System::Byte> ^AddSubtitles(array<System::Byte> ^subtitleFileContents);
			};
		}
	}
}
