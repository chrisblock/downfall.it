#include "stdafx.h"

#include "Merger.h"

array<System::Byte> ^DownfallItCore::Merger::AddSubtitles(array<System::Byte> ^subtitleFileContents)
{
	auto str = Encoding::UTF8->GetString(subtitleFileContents);

	return Encoding::UTF8->GetBytes(str);
}