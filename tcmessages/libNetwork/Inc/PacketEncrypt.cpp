#include "PacketEncrypt.h"

unsigned char Key[20] = {	0x1b, 0x1f, 0x0f, 0x18, 0x19, 0x16, 0x10, 0x1a, 0x1d, 0x1e, 0x0b, 0x15, 0x0d, 0x11, 0x0e, 0x13, 0x12, 0x1c, 0x0c, 0x14 }; 

void CPacketEncrypt::Init()
{
	m_bKeyIndex			= 0;

	m_bMask				= MASKKEY;

	m_bSendSequenceNum	= 0;
	m_sRecvSequenceNum	= (-1);
}


void CPacketEncrypt::SetNextMask(unsigned char MaskKey)
{ 
	m_bMask = MaskKey;
};

bool CPacketEncrypt::CheckSequenceNum(unsigned char Num)
{
	(m_sRecvSequenceNum<100) ? (m_sRecvSequenceNum++) : (m_sRecvSequenceNum=1);

	return (m_sRecvSequenceNum == Num);
}

bool CPacketEncrypt::CheckCheckSum(PACKET* packet)
{
	unsigned short CheckSum=0;
	char* data = (char*)packet;

	for(int i=4; i<packet->size; i++)
		CheckSum += (unsigned char)data[i];

	return (packet->CheckSum == CheckSum);
}

void CPacketEncrypt::ApplyUnionMark(PACKET* packet)
{	
	// 패킷 마크
	packet->Mark = STARTMARK;

	// 시퀀스 발행
	packet->Mark += (m_bSendSequenceNum << 5);

	// 더미키 발행
	packet->Mark += Key[m_bKeyIndex];
	(m_bKeyIndex<19) ? (m_bKeyIndex++) : (m_bKeyIndex=0);

	// 시퀀스 제어
	(m_bSendSequenceNum<100) ? (m_bSendSequenceNum++) : (m_bSendSequenceNum=1);
}

void CPacketEncrypt::ApplyCheckSum(PACKET* packet)
{
	unsigned short CheckSum=0;
	char* data = (char*)packet;

	for(int i=4; i<packet->size; i++)
		CheckSum += (unsigned char)data[i];

	packet->CheckSum = CheckSum;
}

void CPacketEncrypt::ApplyMask(char* buffer,int size)
{
	for(int i=0;i<size;i++)
		buffer[i] ^= m_bMask;
}