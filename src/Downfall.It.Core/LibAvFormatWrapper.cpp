#include "stdafx.h"

#include "LibAvFormatWrapper.h"
#include <iostream>
#include <fstream>

extern "C"
{
	#include "libavformat\avformat.h"
	#include "libavformat\avio.h"
}

DownfallItCore::LibAvFormatWrapper::LibAvFormatWrapper()
{
	av_register_all();

	auto input_format = av_find_input_format("asf");
	auto output_format = av_guess_format(__nullptr, "output.mkv", __nullptr);

	//auto context = avformat_alloc_context();

	AVFormatContext *input = __nullptr;
	auto result = avformat_open_input(&input, "E:\\Projects\\downfall.it\\resources\\videos\\downfall.wmv", input_format, __nullptr);

	avformat_close_input(&input);
}

DownfallItCore::LibAvFormatWrapper::~LibAvFormatWrapper()
{
}
