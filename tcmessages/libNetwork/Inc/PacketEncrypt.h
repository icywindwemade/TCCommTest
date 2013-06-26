#pragma once

#include "Packet.h"

enum ePacketEncryptCode
{
	ePE_OK,
	ePE_StartMark,
	ePE_Checksum,
	ePE_Sequence,
};

class CPacketEncrypt
{
public:
	void Init();

	void SetNextMask(unsigned char MaskKey);

	bool CheckSequenceNum(unsigned char Num);			//Sequence 검사			
	bool CheckCheckSum(PACKET* packet);			//Checksum 검사

	void ApplyUnionMark(PACKET* packet);		//SequenceNum 적용
	void ApplyCheckSum(PACKET* packet);			//CheckSum	적용
	void ApplyMask(char* buffer,int size);		//Mask 적용

private:
	unsigned char	m_bMask;

	unsigned char	m_bKeyIndex;

	unsigned char	m_bSendSequenceNum;
	short			m_sRecvSequenceNum;
};

