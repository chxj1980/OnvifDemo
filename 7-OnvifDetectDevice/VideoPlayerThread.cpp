#include "VideoPlayerThread.h"

extern "C"
{
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
};

QVideoPlayerThread::QVideoPlayerThread(QObject *parent, const QString& qsUri)
	: QThread(parent), m_qsUri(qsUri)
{
	m_bRunning = false;
	m_pFormatCtx = NULL;
	m_nVideoStream = -1;
	m_pCodecCtx = NULL;
	m_pFrame = NULL;
	m_pFrameRGB = NULL;
	m_pSwsContext = NULL;
	m_pOutBuffer = NULL;
	m_pPacket = NULL;
	m_nGotPicture = 0;

	initFFmpeg();
}

QVideoPlayerThread::~QVideoPlayerThread()
{
	m_bRunning = false;
	wait();

	av_free(m_pOutBuffer);
	av_free(m_pFrameRGB);
	avcodec_close(m_pCodecCtx);
	avformat_close_input(&m_pFormatCtx);
}

void QVideoPlayerThread::startThread()
{
	m_bRunning = true;
	start();
}

void QVideoPlayerThread::stopThread()
{
	m_bRunning = false;
}

void QVideoPlayerThread::run()
{
	while (m_bRunning)
	{
		if (av_read_frame(m_pFormatCtx, m_pPacket) >= 0)
		{
			if (m_pPacket->stream_index == m_nVideoStream) 
			{
				int ret = avcodec_decode_video2(m_pCodecCtx, m_pFrame, &m_nGotPicture, m_pPacket);
				if (ret < 0) 
				{
					emit signalSendMsg(QString("decode error."));
					return;
				}

				emit signalSendMsg(QString("Video Packet size = %1").arg(m_pPacket->size));

				if (m_nGotPicture) 
				{
					sws_scale(m_pSwsContext,(uint8_t const * const *)m_pFrame->data, m_pFrame->linesize, 0, m_pCodecCtx->height, m_pFrameRGB->data, m_pFrameRGB->linesize);				
					//�����RGB���� ��QImage����
					QImage tmpImg((uchar *)m_pOutBuffer, m_pCodecCtx->width, m_pCodecCtx->height, QImage::Format_RGB32);
					emit signalSendImage(tmpImg); 														
				}
			}

			av_free_packet(m_pPacket); //�ͷ���Դ,�����ڴ��һֱ����
		}
		msleep(0.05); //ͣһͣ  ��Ȼ�ŵ�̫����
	}
}

void QVideoPlayerThread::initFFmpeg()
{
	//Ffmpeg��ʽ����	
	AVCodec* pAVCodec = NULL;

	avformat_network_init();   ///��ʼ��FFmpeg����ģ�飬
	av_register_all();         //��ʼ��FFMPEG  ��������������������ñ������ͽ�����
	//Allocate an AVFormatContext.
	m_pFormatCtx = avformat_alloc_context();

	AVDictionary* pAVDic = NULL;
	char szOptionKey1[] = "rtsp_transport";
	char szOptionValue1[] = "tcp";
	av_dict_set(&pAVDic, szOptionKey1, szOptionValue1, 0);
	char szOptionKey2[] = "max_delay";
	char szOptionValue2[] = "100";
	av_dict_set(&pAVDic, szOptionKey2, szOptionValue2, 0);

	if (avformat_open_input(&m_pFormatCtx, m_qsUri.toUtf8().data(), NULL, &pAVDic) != 0)
	{
		emit signalSendMsg(QString("can't open the file."));
		return;
	}

	if (avformat_find_stream_info(m_pFormatCtx, NULL) < 0)
	{
		emit signalSendMsg(QString("Could't find stream infomation."));
		return;
	}

	//ѭ��������Ƶ�а���������Ϣ��ֱ���ҵ���Ƶ���͵���
	//�㽫���¼���� ���浽videoStream������
	//������������ֻ������Ƶ��  ��Ƶ���Ȳ�����
	for (int i = 0; i < m_pFormatCtx->nb_streams; i++)
	{
		if (m_pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			m_nVideoStream = i;
		}
	}

	//���videoStreamΪ-1 ˵��û���ҵ���Ƶ��
	if (m_nVideoStream == -1)
	{
		emit signalSendMsg(QString("Didn't find a video stream."));
		return;
	}

	//���ҽ�����
	m_pCodecCtx = m_pFormatCtx->streams[m_nVideoStream]->codec;
	pAVCodec = avcodec_find_decoder(m_pCodecCtx->codec_id);

	m_pCodecCtx->bit_rate = 0;   //��ʼ��Ϊ0
	m_pCodecCtx->time_base.num = 1;  //�������У�һ����25֡
	m_pCodecCtx->time_base.den = 10;
	m_pCodecCtx->frame_number = 1;  //ÿ��һ����Ƶ֡

	if (pAVCodec == NULL)
	{
		emit signalSendMsg(QString("Codec not found."));
		return;
	}

	//�򿪽�����
	if (avcodec_open2(m_pCodecCtx, pAVCodec, NULL) < 0)
	{
		emit signalSendMsg(QString("Could not open codec."));
		return;
	}

	m_pFrame = av_frame_alloc();
	m_pFrameRGB = av_frame_alloc();

	//��������YUV����ת����RGB32
	m_pSwsContext = sws_getContext(m_pCodecCtx->width, m_pCodecCtx->height, m_pCodecCtx->pix_fmt, m_pCodecCtx->width, m_pCodecCtx->height, AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);

	int nBytes = avpicture_get_size(AV_PIX_FMT_RGB32, m_pCodecCtx->width, m_pCodecCtx->height);

	m_pOutBuffer = (uint8_t *)av_malloc(nBytes * sizeof(uint8_t));
	avpicture_fill((AVPicture *)m_pFrameRGB, m_pOutBuffer, AV_PIX_FMT_RGB32, m_pCodecCtx->width, m_pCodecCtx->height);

	int y_size = m_pCodecCtx->width * m_pCodecCtx->height;

	m_pPacket = (AVPacket *)malloc(sizeof(AVPacket)); //����һ��packet
	av_new_packet(m_pPacket, y_size); //����packet������
}
