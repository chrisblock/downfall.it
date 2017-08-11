// Downfall.It.Core.h

#pragma once

using namespace System;
using namespace System::Text;

namespace DownfallItCore {

	public ref class Merger
	{
	public:
		array<System::Byte> ^AddSubtitles(array<System::Byte> ^subtitleFileContents)
		{
			auto str = Encoding::UTF8->GetString(subtitleFileContents);

			return Encoding::UTF8->GetBytes(str);
		}
	};
}
