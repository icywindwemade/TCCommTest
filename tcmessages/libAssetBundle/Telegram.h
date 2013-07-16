#ifndef TELEGRAM_H
#define TELEGRAM_H
//------------------------------------------------------------------------
//
//  Name:   Telegram.h
//
//  Desc:   This defines a telegram. A telegram is a data structure that
//          records information required to dispatch messages. Messages 
//          are used by game agents to communicate with each other.
//
//  Author: Mat Buckland (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include <iostream>
#include <math.h>
#include "IUDefine.h"


class Telegram
{
public :
	UINT	Sender;
	UINT	Receiver;
	UINT	Msg;
	DWORD	DispatchTime;


	Telegram()
		: DispatchTime(0)
		, Sender(0)
		, Receiver(0)
		, Msg(0)
		//, ExtraInfo( NULL )
	{}
	virtual ~Telegram() {}


	Telegram(DWORD time, int sender, int receiver, int msg )
		: DispatchTime(time)
		, Sender(sender)
		, Receiver(receiver)
		, Msg(msg)
		//, ExtraInfo(info)
	{}

	virtual void * GetExtraData() { return NULL; }
};

class TelegramPos : public Telegram
{
public :
	TelegramPos()
		: Telegram()
	{
	}

	TelegramPos( DWORD time, int sender, int reciever, int msg, int pos[2] )
		: Telegram( time, sender, reciever, msg )
	{
		Pos[0] = pos[0];
		Pos[1] = pos[1];
	}
	void * GetExtraData() { return Pos; }

	INT Pos[2];
};

class TelegramInt : public Telegram
{
public :
	TelegramInt()
		: Telegram()
	{
	}

	TelegramInt( DWORD time, int sender, int reciever, int msg, int data )
		: Telegram( time, sender, reciever, msg )
	{
		iData = data;
	}
	void * GetExtraData() { return &iData; }

	INT iData;
};

class TelegramFloat : public Telegram
{
public :
	TelegramFloat()
		: Telegram()
	{
	}

	TelegramFloat( DWORD time, int sender, int reciever, int msg, float data )
		: Telegram( time, sender, reciever, msg )
	{
		fData = data;
	}
	void * GetExtraData() { return &fData; }

	float fData;
};

class TelegramInt3 : public Telegram
{
public :
	TelegramInt3()
		: Telegram()
	{
	}

	TelegramInt3( DWORD time, int sender, int reciever, int msg, int data[3] )
		: Telegram( time, sender, reciever, msg )
	{
		iData[0] = data[0];
		iData[1] = data[1];
		iData[2] = data[2];
	}
	void * GetExtraData() { return &iData; }

	INT iData[3];
};

class TelegramInt4 : public Telegram
{
public :
	TelegramInt4()
		: Telegram()
	{
	}

	TelegramInt4( DWORD time, int sender, int reciever, int msg, int data[] )
		: Telegram( time, sender, reciever, msg )
	{
		iData[0] = data[0];
		iData[1] = data[1];
		iData[2] = data[2];
		iData[3] = data[3];
	}
	void * GetExtraData() { return &iData; }

	INT iData[4];
};

class TelegramInt5 : public Telegram
{
public :
	TelegramInt5()
		: Telegram()
	{
	}

	TelegramInt5( DWORD time, int sender, int reciever, int msg, int data[] )
		: Telegram( time, sender, reciever, msg )
	{
		iData[0] = data[0];
		iData[1] = data[1];
		iData[2] = data[2];
		iData[3] = data[3];
		iData[4] = data[4];
	}
	void * GetExtraData() { return &iData; }

	INT iData[5];
};

class TelegramSzString : public Telegram
{
public :
	TelegramSzString()
		: Telegram()
		, m_szText( NULL )
	{
	}

	~TelegramSzString()
	{
		SAFE_DELETE_ARRAY( m_szText );
	}

	TelegramSzString( DWORD time, int sender, int reciever, int msg, const char * szText )
		: Telegram( time, sender, reciever, msg )
	{
		m_szText = new char[strlen( szText )+1];
		strcpy( m_szText, szText );
	}
	void *GetExtraData() { return m_szText; }
	char *m_szText;
};

class TelegramPointer : public Telegram
{
public :
	TelegramPointer()
		: Telegram()
		, m_pPointer( NULL )
	{
	}

	~TelegramPointer()
	{
	}

	TelegramPointer( DWORD time, int sender, int reciever, int msg, void * pPointer )
		: Telegram( time, sender, reciever, msg )
	{
		m_pPointer = pPointer;
	}
	void *GetExtraData() { return m_pPointer; }
	void *m_pPointer;
};

class TelegramSound : public Telegram
{
public :
	TelegramSound()
		: Telegram()
		, m_pPointer( NULL )
	{
	}

	~TelegramSound()
	{
	}

	TelegramSound( DWORD time, int sender, int reciever, int msg, void * pPointer, int len, int id )
		: Telegram( time, sender, reciever, msg )
	{
		m_pPointer = pPointer;
		m_Len = len;
		m_ID = id;
	}
	void *GetExtraData() { return m_pPointer; }
	void *m_pPointer;
	int m_Len;
	int m_ID;
};

namespace EngineMessage
{
	enum MsgType
	{
		MouseDown = 1,
		MouseDoubleClick,
		MouseMove,
		MouseUp,
		MultiDown,
		MultiMove,
		MultiUp,

        // UI
		UIDown = 20,
		UIMove,
		UIUp,
		UIAnimationEnd,
		UICurrentUIChange,
		UIChangeImage,
        
		UIButton,
		UIButtonDisabled,
		UIButtonDown,
		UIButtonDisabledDown,

		UIEditBoxText,
		UIEditBoxBegin,
		UIEditBoxEnd,
		UIEditBoxUnFocus,
        
		UIListFocusChange,
		UIListScrollToEnd,

        // Game
		AttackEvent = 100,
		PlaySound,
		AnimationEnd,
		AnimationLoopEnd,
		Loaded,
		MapLoaded,
        
        // Loader Thread
		Downloaded = 200,
		DownloadFail,
		SoundLoaded,
        
        // Device
		KeyDown = 300,
		KeyboardMode,
		Vibrate,
	};
};


#endif