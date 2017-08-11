#pragma once

using namespace System;
using namespace System::Text;

namespace DownfallItCore
{
	public ref class Merger
	{
	public:
		array<System::Byte> ^AddSubtitles(array<System::Byte> ^subtitleFileContents);
	};
}
