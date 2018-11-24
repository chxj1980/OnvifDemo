#ifndef VIDEOPLAYERTHREAD_H
#define VIDEOPLAYERTHREAD_H

#include <QThread>
#include "stdint.h"
#include <QImage>

class AVFormatContext;
class AVCodecContext;
class AVFrame;
class SwsContext;
class AVPacket;

class QVideoPlayerThread : public QThread
{
	Q_OBJECT

public:
	QVideoPlayerThread(QObject *parent, const QString& qsUri);
	~QVideoPlayerThread();

	void startThread();
	void stopThread();

protected:
	// �����߳�
	virtual void run();

signals:
	void signalSendMsg(const QString& qsMsg);
	void signalSendImage(const QImage& objImage);

private:
	// ��FFmpeg���г�ʼ��
	void initFFmpeg();

private:
	QString m_qsUri;		// rtsp��ַ
	bool m_bRunning;		// ���ű�־�������˳�
	AVFormatContext* m_pFormatCtx;
	int m_nVideoStream;		// ��Ƶ������
	AVCodecContext* m_pCodecCtx;
	AVFrame* m_pFrame;
	AVFrame* m_pFrameRGB;
	SwsContext* m_pSwsContext;
	uint8_t* m_pOutBuffer;
	AVPacket* m_pPacket;
	int m_nGotPicture;
};

#endif // VIDEOPLAYERTHREAD_H
