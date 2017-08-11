#include "stdafx.h"

#include "LibAvFormatWrapper.h"

extern "C"
{
	#include "libavformat\avformat.h"
	#include "libavformat\avio.h"
}

DownfallItCore::LibAvFormatWrapper::LibAvFormatWrapper()
{
	av_register_all();
}

DownfallItCore::LibAvFormatWrapper::~LibAvFormatWrapper()
{
}
