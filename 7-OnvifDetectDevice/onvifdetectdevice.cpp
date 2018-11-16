#include "onvifdetectdevice.h"
#include "stdsoap2.h"
#include "wsdd.nsmap"
#include "soapStub.h"
#include <assert.h>
#include "soapH.h"
#include "QTextEdit"

OnvifDetectDevice::OnvifDetectDevice(QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, flags)
{
	ui.setupUi(this);
}

OnvifDetectDevice::~OnvifDetectDevice()
{

}

void OnvifDetectDevice::pushButtonSearchSlotClicked()
{
	detectDevice(NULL);
}

void OnvifDetectDevice::detectDevice(void (*cb)(char *DeviceXAddr))
{
	int result = 0;
	unsigned int count = 0;                                                     // ���������豸����
	soap* pSoap = NULL;                                                   // soap��������
	wsdd__ProbeType      stuReq;                                            // ���ڷ���Probe��Ϣ
	__wsdd__ProbeMatches stuRep;                                            // ���ڽ���ProbeӦ��
	wsdd__ProbeMatchType *probeMatch;

	SOAP_ASSERT(NULL != (pSoap = initOnvifSoap(SOAP_SOCK_TIMEOUT)));

	initOnvifHeader(pSoap);                                                    // ������Ϣͷ����
	initOnvifProbeType(pSoap, &stuReq);                                           // ����Ѱ�ҵ��豸�ķ�Χ������
	result = soap_send___wsdd__Probe(pSoap, SOAP_MCAST_ADDR, NULL, &stuReq);        // ���鲥��ַ�㲥Probe��Ϣ
	while (SOAP_OK == result)                                                   // ��ʼѭ�������豸���͹�������Ϣ
	{
		result = soap_recv___wsdd__ProbeMatches(pSoap, &stuRep);
		if (SOAP_OK == result)
		{
			if (pSoap->error)
			{
				printSoapError(pSoap, "ProbeMatches");
			}
			else
			{                                                            // �ɹ����յ��豸��Ӧ����Ϣ
				printProbeMatches(&stuRep);

				if (NULL != stuRep.wsdd__ProbeMatches)
				{
					count += stuRep.wsdd__ProbeMatches->__sizeProbeMatch;
					for(int i = 0; i < stuRep.wsdd__ProbeMatches->__sizeProbeMatch; i++)
					{
						probeMatch = stuRep.wsdd__ProbeMatches->ProbeMatch + i;
						if (NULL != cb)
						{
							cb(probeMatch->XAddrs);                             // ʹ���豸�����ִַ�к����ص�
						}
					}
				}
			}
		}
		else if (pSoap->error)
		{
			break;
		}
	}

	printMsg(QString("detect end! It has detected %1 devices!").arg(count));

	if (NULL != pSoap)
	{
		deleteOnvifSoap(pSoap);
	}
}

soap* OnvifDetectDevice::initOnvifSoap(int timeout)
{
	soap *pSoap = NULL;                                                   // soap��������

	SOAP_ASSERT(NULL != (pSoap = soap_new()));

	soap_set_namespaces(pSoap, namespaces);                                      // ����soap��namespaces
	pSoap->recv_timeout    = timeout;                                            // ���ó�ʱ������ָ��ʱ��û�����ݾ��˳���
	pSoap->send_timeout    = timeout;
	pSoap->connect_timeout = timeout;

#if defined(__linux__) || defined(__linux)                                      // �ο�https://www.genivia.com/dev.html#client-c���޸ģ�
	pSoap->socket_flags = MSG_NOSIGNAL;                                          // To prevent connection reset errors
#endif

	soap_set_mode(pSoap, SOAP_C_UTFSTRING);                                      // ����ΪUTF-8���룬�����������OSD������

	return pSoap;
}

void OnvifDetectDevice::initOnvifHeader(soap* pSoap)
{

	struct SOAP_ENV__Header *header = NULL;

	SOAP_ASSERT(NULL != pSoap);

	header = (struct SOAP_ENV__Header *)mallocOnvifSoap(pSoap, sizeof(struct SOAP_ENV__Header));
	soap_default_SOAP_ENV__Header(pSoap, header);
	header->wsa__MessageID = (char*)soap_wsa_rand_uuid(pSoap);
	header->wsa__To        = (char*)mallocOnvifSoap(pSoap, strlen(SOAP_TO) + 1);
	header->wsa__Action    = (char*)mallocOnvifSoap(pSoap, strlen(SOAP_ACTION) + 1);
	strcpy(header->wsa__To, SOAP_TO);
	strcpy(header->wsa__Action, SOAP_ACTION);
	pSoap->header = header;
}

void OnvifDetectDevice::initOnvifProbeType(soap* pSoap, wsdd__ProbeType* pstuReq)
{
	struct wsdd__ScopesType *scope = NULL;                                      // �����������������Web����

	SOAP_ASSERT(NULL != pSoap);
	SOAP_ASSERT(NULL != pstuReq);

	scope = (struct wsdd__ScopesType *)mallocOnvifSoap(pSoap, sizeof(struct wsdd__ScopesType));
	soap_default_wsdd__ScopesType(pSoap, scope);                                 // ����Ѱ���豸�ķ�Χ
	scope->__item = (char*)mallocOnvifSoap(pSoap, strlen(SOAP_ITEM) + 1);
	strcpy(scope->__item, SOAP_ITEM);

//	memset(pstuReq, 0x00, sizeof(struct wsdd__ProbeType));
	soap_default_wsdd__ProbeType(pSoap, pstuReq);
	pstuReq->Scopes = scope;
	pstuReq->Types  = (char*)mallocOnvifSoap(pSoap, strlen(SOAP_TYPES) + 1);     // ����Ѱ���豸������
	strcpy(pstuReq->Types, SOAP_TYPES);
}

void OnvifDetectDevice::printSoapError(soap* pSoap, const char* szMsg)
{
	QString qsTest;
	if (NULL == szMsg)
	{
		qsTest.sprintf("[soap] error: %d, %s, %s", pSoap->error, *soap_faultcode(pSoap), *soap_faultstring(pSoap));
	}
	else
	{
		qsTest.sprintf("[soap] %s error: %d, %s, %s", szMsg, pSoap->error, *soap_faultcode(pSoap), *soap_faultstring(pSoap));
	}

	printMsg(qsTest);
}

void OnvifDetectDevice::printProbeMatches(const __wsdd__ProbeMatches* pstuRep)
{

}

void OnvifDetectDevice::printMsg(QString qsText)
{
	ui.textEditResult->append(qsText);
}

void OnvifDetectDevice::deleteOnvifSoap(soap* pSoap)
{
	soap_destroy(pSoap);                                                         // remove deserialized class instances (C++ only)
	soap_end(pSoap);                                                             // Clean up deserialized data (except class instances) and temporary data
	soap_done(pSoap);                                                            // Reset, close communications, and remove callbacks
	soap_free(pSoap);                                                            // Reset and deallocate the context created with soap_new or soap_copy
}

void* OnvifDetectDevice::mallocOnvifSoap(soap* pSoap, int nLen)
{
	void *p = NULL;

	if (nLen > 0) {
		p = soap_malloc(pSoap, nLen);
		SOAP_ASSERT(NULL != p);
//		memset(p, 0x00 ,nLen);
	}
	return p;
}

const char* OnvifDetectDevice::soap_wsa_rand_uuid(soap* pSoap)
{
	return soap_strdup(pSoap, soap_rand_uuid(pSoap, "urn:uuid:"));
}
