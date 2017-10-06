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
	FileStreams(AVFormatContext *file, const std::uint32_t subtitleIndex, AVStream *subtitle, const std::uint32_t audioIndex, AVStream *audio, const std::uint32_t videoIndex, AVStream *video);
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

	AVFormatContext *GetFormatContext() const;

	bool HasVideoStream() const;
	std::uint32_t GetVideoIndex() const;
	AVStream *GetVideoStream() const;
	AVCodecParameters *GetVideoCodec() const;

	bool HasAudioStream() const;
	std::uint32_t GetAudioIndex() const;
	AVStream *GetAudioStream() const;
	AVCodecParameters *GetAudioCodec() const;

	bool HasSubtitleStream() const;
	std::uint32_t GetSubtitleIndex() const;
	AVStream *GetSubtitleStream() const;
	AVCodecParameters *GetSubtitleCodec() const;

private:
	AVFormatContext *_file;

	std::uint32_t _subtitleIndex;
	AVStream *_subtitle;

	std::uint32_t _audioIndex;
	AVStream *_audio;

	std::uint32_t _videoIndex;
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

FileStreams::FileStreams(AVFormatContext *file, const std::uint32_t subtitleIndex, AVStream *subtitle, const std::uint32_t audioIndex, AVStream *audio, const std::uint32_t videoIndex, AVStream *video) :
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
//}

FileStreams::FileStreams(FileStreams &&other) :
	  FileStreams()
{
	swap(*this, other);
}

FileStreams::~FileStreams()
{
	avformat_close_input(&_file);
}

FileStreams &FileStreams::operator =(FileStreams other)
{
	swap(*this, other);

	return *this;
}

AVFormatContext *FileStreams::GetFormatContext() const
{
	return _file;
}

bool FileStreams::HasVideoStream() const
{
	bool result = (_video != __nullptr);

	return result;
}

std::uint32_t FileStreams::GetVideoIndex() const
{
	return _videoIndex;
}

AVCodecParameters *FileStreams::GetVideoCodec() const
{
	return _video->codecpar;
}

AVStream *FileStreams::GetVideoStream() const
{
	return _video;
}

bool FileStreams::HasAudioStream() const
{
	bool result = (_audio != __nullptr);

	return result;
}

std::uint32_t FileStreams::GetAudioIndex() const
{
	return _audioIndex;
}

AVCodecParameters *FileStreams::GetAudioCodec() const
{
	return _audio->codecpar;
}

AVStream *FileStreams::GetAudioStream() const
{
	return _audio;
}

bool FileStreams::HasSubtitleStream() const
{
	bool result = (_subtitle != __nullptr);

	return result;
}

std::uint32_t FileStreams::GetSubtitleIndex() const
{
	return _subtitleIndex;
}

AVCodecParameters *FileStreams::GetSubtitleCodec() const
{
	return _subtitle->codecpar;
}

AVStream *FileStreams::GetSubtitleStream() const
{
	return _subtitle;
}

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

	FileStreams downfall = GetStreams("C:\\projects\\downfall.it\\resources\\videos\\downfall.wmv", "asf");

	FileStreams subtitles = GetStreams("C:\\projects\\downfall.it\\resources\\videos\\downfall.subtitles.srt", "srt");

	AVFormatContext *output = __nullptr;
	std::string outfile = "C:\\projects\\downfall.it\\resources\\videos\\downfall.output.mkv";
	AVOutputFormat *outfmt = av_guess_format(__nullptr, outfile.c_str(), __nullptr);

	if (outfmt == __nullptr)
	{
		std::exception e("Unknown output format.");

		throw e;
	}
	else
	{
		output = avformat_alloc_context();
		if (output)
		{
			output->oformat = outfmt;
			sprintf_s(output->filename, "%s", outfile.c_str());
		}
		else
		{
			std::exception e("Unable to allocate output context.");

			throw e;
		}
	}

	AVCodec *out_vid_codec = __nullptr;
	AVCodec *out_aud_codec = __nullptr;
	AVCodec *out_sub_codec = __nullptr;

	if ((outfmt->video_codec != AV_CODEC_ID_NONE) && downfall.HasVideoStream())
	{
		out_vid_codec = avcodec_find_encoder(outfmt->video_codec);

		if (out_vid_codec == __nullptr)
		{
			std::exception e("Could not find video codec.");

			throw e;
		}
		else
		{
			AVStream *m_out_vid_strm = avformat_new_stream(output, out_vid_codec);
			if (m_out_vid_strm == __nullptr)
			{
				std::exception e("Could not allocate video stream.");

				throw e;
			}
			else
			{
				auto result = avcodec_parameters_copy(m_out_vid_strm->codecpar, downfall.GetVideoCodec());

				HandleError(result);

				//auto m_fps = 30;

				//AVCodec *pCodec = avcodec_find_decoder(downfall.GetVideoCodec()->codec_id);
				//AVCodecContext *pCodecCtx = avcodec_alloc_context3(pCodec);
				//avcodec_parameters_to_context(pCodecCtx, downfall.GetVideoCodec());
				//avcodec_open2(pCodecCtx, pCodec, __nullptr);

				//AVCodecContext *pOutputCodecContext = avcodec_alloc_context3(out_vid_codec);

				//m_out_vid_strm->sample_aspect_ratio.den = pOutputCodecContext->sample_aspect_ratio.den;
				//m_out_vid_strm->sample_aspect_ratio.num = downfall.GetVideoCodec()->sample_aspect_ratio.num;
				//m_out_vid_strm->codec->codec_id = downfall.GetVideoCodec()->codec_id;
				//m_out_vid_strm->codec->time_base.num = 1;
				//m_out_vid_strm->codec->time_base.den = m_fps * (pOutputCodecContext->ticks_per_frame);
				m_out_vid_strm->time_base.num = downfall.GetVideoStream()->time_base.num;
				m_out_vid_strm->time_base.den = downfall.GetVideoStream()->time_base.den;
				//m_out_vid_strm->r_frame_rate.num = m_fps;
				//m_out_vid_strm->r_frame_rate.den = 1;
				//m_out_vid_strm->avg_frame_rate.den = 1;
				//m_out_vid_strm->avg_frame_rate.num = m_fps;
				m_out_vid_strm->duration = downfall.GetVideoStream()->duration; // (m_out_end_time - m_out_start_time) * 1000;
			}
		}
	}

	if (outfmt->audio_codec != AV_CODEC_ID_NONE && downfall.HasAudioStream())
	{
		out_aud_codec = avcodec_find_encoder(outfmt->audio_codec);
		if (out_aud_codec == __nullptr)
		{
			std::exception e("Could not find audio codec.");

			throw e;
		}
		else
		{
			AVStream *m_out_aud_strm = avformat_new_stream(output, out_aud_codec);
			if (m_out_aud_strm == __nullptr)
			{
				std::exception e("Could not allocate audio stream.");

				throw e;
			}
			else
			{
				auto result = avcodec_parameters_copy(m_out_aud_strm->codecpar, downfall.GetAudioCodec());

				HandleError(result);

				//m_out_aud_strm->codec->codec_id = m_in_aud_strm->codec->codec_id;
				//m_out_aud_strm->codec->codec_tag = 0;
				//m_out_aud_strm->pts = m_in_aud_strm->pts;
				m_out_aud_strm->duration = downfall.GetAudioStream()->duration;
				m_out_aud_strm->time_base.num = downfall.GetAudioStream()->time_base.num;
				m_out_aud_strm->time_base.den = downfall.GetAudioStream()->time_base.den;
			}
		}
	}

	if (outfmt->subtitle_codec != AV_CODEC_ID_NONE && downfall.HasSubtitleStream())
	{
		out_sub_codec = avcodec_find_encoder(outfmt->subtitle_codec);
		if (out_sub_codec == __nullptr)
		{
			std::exception e("Could not find subtitle codec.");

			throw e;
		}
		else
		{
			AVStream *m_out_sub_strm = avformat_new_stream(output, out_sub_codec);
			if (m_out_sub_strm == __nullptr)
			{
				std::exception e("Could not allocate subtitle stream.");

				throw e;
			}
			else
			{
				auto result = avcodec_parameters_copy(m_out_sub_strm->codecpar, downfall.GetSubtitleCodec());

				HandleError(result);

				//m_out_sub_strm->codec->codec_id = m_in_aud_strm->codec->codec_id;
				//m_out_sub_strm->codec->codec_tag = 0;
				//m_out_sub_strm->pts = m_in_aud_strm->pts;
				m_out_sub_strm->duration = downfall.GetSubtitleStream()->duration;
				m_out_sub_strm->time_base.num = downfall.GetSubtitleStream()->time_base.num;
				m_out_sub_strm->time_base.den = downfall.GetSubtitleStream()->time_base.den;
			}
		}
	}

	if (!(outfmt->flags & AVFMT_NOFILE))
	{
		auto result = avio_open2(&output->pb, outfile.c_str(), AVIO_FLAG_WRITE, __nullptr, __nullptr);

		HandleError(result);
	}

	/* Write the stream header, if any. */
	auto result = avformat_write_header(output, __nullptr);

	HandleError(result);

	AVPacket pkt;
	while (av_read_frame(downfall.GetFormatContext(), &pkt) >= 0)
	{
		if (pkt.stream_index == downfall.GetVideoIndex())
		{
			auto result = av_interleaved_write_frame(output, &pkt);

			HandleError(result);
		}
		else if (pkt.stream_index == downfall.GetAudioIndex())
		{
			auto result = av_interleaved_write_frame(output, &pkt);

			HandleError(result);
		}
		else if (pkt.stream_index == downfall.GetSubtitleIndex())
		{
			auto result = av_interleaved_write_frame(output, &pkt);

			HandleError(result);
		}
		else
		{
			std::exception e("Unknown packet.");

			throw e;
		}
		
		av_init_packet(&pkt);
		pkt.size = 0;
		pkt.data = __nullptr;
	}

	while (av_read_frame(subtitles.GetFormatContext(), &pkt) >= 0)
	{
		if (pkt.stream_index == subtitles.GetVideoIndex())
		{
			auto result = av_interleaved_write_frame(output, &pkt);

			HandleError(result);
		}
		else if (pkt.stream_index == subtitles.GetAudioIndex())
		{
			auto result = av_interleaved_write_frame(output, &pkt);

			HandleError(result);
		}
		else if (pkt.stream_index == subtitles.GetSubtitleIndex())
		{
			// this seems to not work, perhaps because we're at the end of the output "stream"?
			auto result = av_interleaved_write_frame(output, &pkt);

			HandleError(result);
		}
		else
		{
			std::exception e("Unknown packet.");

			throw e;
		}

		av_init_packet(&pkt);
		pkt.size = 0;
		pkt.data = __nullptr;
	}

	av_write_trailer(output);
}

Downfall::It::Core::LibAvFormatWrapper::~LibAvFormatWrapper()
{
}
