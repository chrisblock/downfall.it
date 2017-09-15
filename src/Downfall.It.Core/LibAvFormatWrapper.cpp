#include "stdafx.h"

#include "LibAvFormatWrapper.h"
#include <iostream>
#include <fstream>

extern "C"
{
	#include "libavformat\avformat.h"
	#include "libavformat\avio.h"
}

void HandleError(const std::int32_t status)
{
	if (status < 0)
	{
		char error[1024] = {};
		auto result = av_strerror(status, error, _countof(error));

		if (result < 0)
		{
			::sprintf_s(error, "Unknown error code: %d (%x)", status, status);
		}

		std::exception e(error);

		throw e;
	}
}

class FileStreams
{
public:
	FileStreams();
	FileStreams(AVFormatContext *file, const std::int32_t subtitleIndex, AVStream *subtitle, const std::int32_t audioIndex, AVStream *audio, const std::int32_t videoIndex, AVStream *video);
	FileStreams(const FileStreams &other) = delete;

	//FileStreams(const FileStreams &other) :
	//	  _file(other._file)
	//	, _subtitleIndex(other._subtitleIndex)
	//	, _subtitle(other._subtitle)
	//	, _audioIndex(other._audioIndex)
	//	, _audio(other._audio)
	//	, _videoIndex(other._videoIndex)
	//	, _video(other._video)
	//{
	//};

	FileStreams(FileStreams &&other);

	~FileStreams();

	FileStreams &operator =(FileStreams other);

	friend void swap(FileStreams &left, FileStreams &right);

private:
	AVFormatContext *_file;

	std::int32_t _subtitleIndex;
	AVStream *_subtitle;

	std::int32_t _audioIndex;
	AVStream *_audio;

	std::int32_t _videoIndex;
	AVStream *_video;
};

void swap(FileStreams &left, FileStreams &right)
{
	using std::swap;

	swap(left._file, right._file);
	swap(left._subtitleIndex, right._subtitleIndex);
	swap(left._subtitle, right._subtitle);
	swap(left._audioIndex, right._audioIndex);
	swap(left._audio, right._audio);
	swap(left._videoIndex, right._videoIndex);
	swap(left._video, right._video);
}

FileStreams::FileStreams() :
	  FileStreams(__nullptr, -1, __nullptr, -1, __nullptr, -1, __nullptr)
{
}

FileStreams::FileStreams(AVFormatContext *file, const std::int32_t subtitleIndex, AVStream *subtitle, const std::int32_t audioIndex, AVStream *audio, const std::int32_t videoIndex, AVStream *video) :
	  _file(file)
	, _subtitleIndex(subtitleIndex)
	, _subtitle(subtitle)
	, _audioIndex(audioIndex)
	, _audio(audio)
	, _videoIndex(videoIndex)
	, _video(video)
{
}

//FileStreams::FileStreams(const FileStreams &other) :
//	  _file(other._file)
//	, _subtitleIndex(other._subtitleIndex)
//	, _subtitle(other._subtitle)
//	, _audioIndex(other._audioIndex)
//	, _audio(other._audio)
//	, _videoIndex(other._videoIndex)
//	, _video(other._video)
//{
//};

FileStreams::FileStreams(FileStreams &&other) :
	  FileStreams()
{
	swap(*this, other);
};

FileStreams::~FileStreams()
{
	avformat_close_input(&_file);
};

FileStreams &FileStreams::operator =(FileStreams other)
{
	swap(*this, other);

	return *this;
};

FileStreams GetStreams(const std::string &path, const std::string &format)
{
	auto input_format = av_find_input_format(format.c_str());

	AVFormatContext *input = __nullptr;
	auto result = avformat_open_input(&input, path.c_str(), input_format, __nullptr);

	HandleError(result);

	result = avformat_find_stream_info(input, __nullptr);

	HandleError(result);

	std::uint32_t subtitle_index = -1;
	std::uint32_t audio_index = -1;
	std::uint32_t video_index = -1;

	AVStream *subtitle = __nullptr;
	AVStream *audio = __nullptr;
	AVStream *video = __nullptr;

	for (std::uint32_t i = 0; i < input->nb_streams; i++)
	{
		if (input->streams[i]->codecpar->codec_type == AVMediaType::AVMEDIA_TYPE_SUBTITLE)
		{
			subtitle_index = i;
			subtitle = input->streams[i];
		}

		if (input->streams[i]->codecpar->codec_type == AVMediaType::AVMEDIA_TYPE_AUDIO)
		{
			audio_index = i;
			audio = input->streams[i];
		}

		if (input->streams[i]->codecpar->codec_type == AVMediaType::AVMEDIA_TYPE_VIDEO)
		{
			video_index = i;
			video = input->streams[i];
		}
	}

	FileStreams streams(input, subtitle_index, subtitle, audio_index, audio, video_index, video);

	return streams;
}

Downfall::It::Core::LibAvFormatWrapper::LibAvFormatWrapper()
{
	av_register_all();

	auto output_format = av_guess_format(__nullptr, "output.mkv", __nullptr);

	//auto context = avformat_alloc_context();

	FileStreams downfall = GetStreams("C:\\projects\\downfall.it\\resources\\videos\\downfall.wmv", "asf");

	FileStreams subtitles = GetStreams("C:\\projects\\downfall.it\\resources\\videos\\downfall.subtitles.srt", "srt");
}

Downfall::It::Core::LibAvFormatWrapper::~LibAvFormatWrapper()
{
}
