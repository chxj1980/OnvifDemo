#ifndef ONVIFDETECTDEVICE_H
#define ONVIFDETECTDEVICE_H

#include <QtGui/QWidget>
#include "ui_onvifdetectdevice.h"
#include <vector>
using namespace std;

#define SOAP_ASSERT     assert

#define SOAP_TO         "urn:schemas-xmlsoap-org:ws:2005:04:discovery"
#define SOAP_ACTION     "http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe"

#define SOAP_MCAST_ADDR "soap.udp://239.255.255.250:3702"                       // onvif�涨���鲥��ַ

#define SOAP_ITEM       ""                                                      // Ѱ�ҵ��豸��Χ
#define SOAP_TYPES      "dn:NetworkVideoTransmitter"                            // Ѱ�ҵ��豸����

#define SOAP_SOCK_TIMEOUT    (10)                                               // socket��ʱʱ�䣨�����룩
#define ONVIF_ADDRESS_SIZE   (128)                                              // URI��ַ����

#define USERNAME    "admin"
#define PASSWORD    "98765432i"

class soap;
class wsdd__ProbeType;
class __wsdd__ProbeMatches;

class CVideoEncoderConfiguration			// ��Ƶ������������Ϣ
{
public:
	CVideoEncoderConfiguration();
	~CVideoEncoderConfiguration();

	QString m_qsToken;					// Ψһ��ʶ����Ƶ�������������ַ���
	int m_nWidth;                       // �ֱ���
	int m_Height;
};

/* �豸������Ϣ */
class CProfile
{
public:
	CProfile();
	~CProfile();

	QString m_qsToken;						// Ψһ��ʶ�豸�����ļ��������ַ���
	CVideoEncoderConfiguration m_objVideoEncoderConfiguration;
protected:

private:
};

class OnvifDetectDevice : public QWidget
{
	Q_OBJECT

public:
	OnvifDetectDevice(QWidget *parent = 0, Qt::WFlags flags = 0);
	~OnvifDetectDevice();

private slots:
	void pushButtonSearchSlotClicked();

private:
	void detectDevice();
	soap* initOnvifSoap(int timeout);

/************************************************************************
**������initOnvifHeader
**���ܣ���ʼ��soap������Ϣͷ
**������
        [in] soap - soap��������
**���أ���
**��ע��
    1). �ڱ������ڲ�ͨ��ONVIF_soap_malloc������ڴ棬����ONVIF_soap_delete�б��ͷ�
************************************************************************/
	void initOnvifHeader(soap* pSoap);

/************************************************************************
**������initOnvifProbeType
**���ܣ���ʼ��̽���豸�ķ�Χ������
**������
        [in]  soap  - soap��������
        [out] probe - ���Ҫ̽����豸��Χ������
**���أ�
        0����̽�⵽����0����δ̽�⵽
**��ע��
    1). �ڱ������ڲ�ͨ��ONVIF_soap_malloc������ڴ棬����ONVIF_soap_delete�б��ͷ�
************************************************************************/
	void initOnvifProbeType(soap* pSoap, wsdd__ProbeType* stuReq);
	void printSoapError(soap* pSoap, const char* szMsg = NULL);
	void printProbeMatches(const __wsdd__ProbeMatches* pstuRep);
	void printMsg(QString qsText);
	void deleteOnvifSoap(soap* pSoap);
	void* mallocOnvifSoap(soap* pSoap, int nLen);

/************************************************************************
**������getOnvifDeviceInformation
**���ܣ���ȡ�豸������Ϣ
**������
        [in] szAddrs - �豸�����ַ
**���أ�
        0�����ɹ�����0����ʧ��
**��ע��
************************************************************************/
	int getOnvifDeviceInformation(const char* szAddrs);


/************************************************************************
**������setOnvifAuthInfo
**���ܣ�������֤��Ϣ
**������
        [in] soap     - soap��������
        [in] username - �û���
        [in] password - ����
**���أ�
        0�����ɹ�����0����ʧ��
**��ע��
************************************************************************/
	int setOnvifAuthInfo(soap* pSoap, const char* szUsername, const char* szPassword);

/************************************************************************
**������getOnvifCapabilities
**���ܣ���ȡ�豸������Ϣ
**������
        [in] szAddrs - �豸�����ַ
		[out] qsMediaXAddr - �豸ý���ַ
**���أ�
        0�����ɹ�����0����ʧ��
**��ע��
    1). ��������Ҫ�Ĳ���֮һ��ý������ַ
************************************************************************/
	int getOnvifCapabilities(const char* szAddrs, QString& qsMediaXAddr);

/************************************************************************
**������ONVIF_GetProfiles
**���ܣ���ȡ�豸������Ƶ����������Ϣ
**������
        [in] qsMediaXAddr - ý������ַ
        [out] vecProfile  - ���ص��豸����Ƶ����������Ϣ�б�
**���أ�
        �����豸��֧�ֵ�����������ͨ������/��������������profiles�б����
**��ע��
        1). ע�⣺һ���������������������԰�����Ƶ����Ƶ���ݣ�Ҳ���Խ���������Ƶ���ݡ�
************************************************************************/
	int getOnvifProfiles(const QString& qsMediaXAddr, vector<CProfile>& vecProfile);

/************************************************************************
**������getOnvifStreamUri
**���ܣ���ȡ�豸������ַ(RTSP)
**������
        [in]  qsMediaXAddr    - ý������ַ
        [in]  qsToken  - the media profile token
        [out] uri           - ���صĵ�ַ
**���أ�
        0�����ɹ�����0����ʧ��
**��ע��
************************************************************************/
	int getOnvifStreamUri(const QString& qsMediaXAddr,const QString& qsToken, QString& qsUri);

/************************************************************************
**������make_uri_withauth
**���ܣ����������֤��Ϣ��URI��ַ
**������
        [in]  qsUri       - δ����֤��Ϣ��URI��ַ
        [in]  username      - �û���
        [in]  password      - ����
        [out] qsUriAuth      - ���صĴ���֤��Ϣ��URI��ַ
**���أ�
        0�ɹ�����0ʧ��
**��ע��
    1). ���ӣ�
    ����֤��Ϣ��uri��rtsp://100.100.100.140:554/av0_0
    ����֤��Ϣ��uri��rtsp://username:password@100.100.100.140:554/av0_0
************************************************************************/
	int makeUriWithauth(const QString& qsUri, const char* szUsername, const char * szPassword, QString& qsUriAuth);
private:
	Ui::OnvifDetectDeviceClass ui;
};

#endif // ONVIFDETECTDEVICE_H
