/*----------------------------------------------------------------------------
; Copyright (C) 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       textview.c
;
; Description:
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 05/29/96  gas  Created.
; 
----------------------------------------------------------------------------*/

#include "../include/ptypes.h"
#include "maya.h"
#include "user.h"
#include "gdb.h"
#include "player.h"
#include "teletype.h"
#include "spell.h"
#include <stdarg.h>
#include <stdio.h>

#include "Data/Database/Ids.h"

#define BLACK	  0
#define RED		  249
#define BLUE	  252
#define YELLOW	  114
#define WHITE	  255
#define GREEN	  85

BYTE color_freq[] = {RED, YELLOW, GREEN, WHITE, BLUE};

GDBT_MESSAGES gdbt_messages;
BYTE GDBT_MESSAGES::c_m = PUBLIC_CODE;
TELETYPE *TELETYPE::c_teletype = NULL;
BYTE TELETYPE::c_freq = 0;
BYTE TELETYPE::c_lang = 0;
BYTE TELETYPE::c_pitch = 0;
void GDBT_MESSAGES::chk(char*s, int l){for(int i=0;i<l;i++)s[i]^=c_m;}
//----------------------------------------------------------------------------
TELETYPE::TELETYPE(int view_id):
SCROLLVIEW(view_id)
{
  InitBuffer();

  c_teletype = this;

  // init all message pids to zero:
  for (int i=0; i<MAX_TELETYPE_MESSAGES;i++)
  {
		m_message[i].name[0] = 0;
		m_message[i].pid = 0;
  }

	// get a pointer to the font:
  font[0] = new CFONT(IDS_FONTTNR12);
  font[1] = new CFONT(IDS_FONTPAR);
  font[2] = new CFONT(IDS_FONTVILL);
  font[3] = new CFONT(IDS_FONTCITY);
  font[4] = new CFONT(IDS_FONTTNB18);
  font[5] = new CFONT(IDS_FONTPAR_BIG);
  font[6] = new CFONT(IDS_FONTVILL_BIG);
  font[7] = new CFONT(IDS_FONTCITY_BIG);

	m_text_dirty = TRUE;

	m_message_count = 0;
	m_last_message = 0;
	m_first_message = 0;
	m_Slew_Y = 0;
}

//----------------------------------------------------------------------------
TELETYPE::~TELETYPE()
{
  // release the font:
  delete font[0];
  delete font[1];
  delete font[2];
  delete font[3];
  delete font[4];
  delete font[5];
  delete font[6];
  delete font[7];

  c_teletype = NULL;
}

//----------------------------------------------------------------------------
WORD TELETYPE::QueryCursor(SRECT r)
{
	int i;
	POINT p;
	p.x = r.left;
	p.y = r.top;

	p = MapToCanvas(p);

	i = p.y/font[c_pitch*4]->GetHeight();
	if (m_message[i].pid)
		  return IDS_HANDMACH;
	else
		  return IDS_HANDIDLE;
}

//----------------------------------------------------------------------------
WORD TELETYPE::Grab(POINT p, BOOL bClickOnly, POINT *obj_pos)
{
	int i;

	p = MapToCanvas(p);

	i = p.y/font[c_pitch*4]->GetHeight();
	if (m_message[i].pid
	 && m_message[i].pid != local_player->GetPid())
		gdbt_players.SendIntimate(m_message[i].pid, TRUE);

	return VIEW::Grab(p, bClickOnly, obj_pos);
}

//----------------------------------------------------------------------------
void TELETYPE::Dump()
{
  c_teletype->surface.Write("textdump", FALSE, FALSE, 1, 100);
}

//----------------------------------------------------------------------------
void TELETYPE::Paint(int etime, int flags)
{
  CFONT *f;
  CFONT *tnr = font[c_pitch*4];

  int i;
  char buffer[MAX_NAME];
  int color;
  int x,y;
  x=0;
  y=0;
  BOOL city_folk = local_player->GetCult() == CULT_CITY;

  {
	surface.Clear(BLACK);

	// print all the buffered messages:
	i=m_first_message;
	while (i!= m_last_message)
	{
		int lang = m_message[i].lang-1;
		if (local_player->knows(lang)
		  || (city_folk && lang == LANG_CITY)
		  || (!city_folk && lang == LANG_VILLAGE))
			f = font[0+c_pitch*4];
		else
			f = font[m_message[i].lang+c_pitch*4];

	  tnr->SetPos(x,y);

	  color = color_freq[m_message[i].freq];

	  if (m_message[i].pid != 0)
	  {
		gdbt_players.GetPlayerName(m_message[i].pid, buffer, MAX_NAME-1);
		tnr->Print(&surface, buffer, color);
		tnr->Print(&surface, ": ", color);
	  }
	  else if (m_message[i].name[0] != 0)
	  {
		tnr->Print(&surface, m_message[i].name, color);
		tnr->Print(&surface, ": ", color);
	  }

	  x = tnr->GetX();
	  y = tnr->GetY();
	  f->SetPos(x,y);

	  f->Print(&surface, m_message[i].buffer, color);
	  f->CR();

	  x = f->GetX();
	  y = f->GetY();

	  i++;
	  if (i >= MAX_TELETYPE_MESSAGES)
		i = 0;
	}

	f = font[c_lang+c_pitch*4];
	f->SetPos(x,y);
    // print the current input buffer:
	color = color_freq[c_freq];
    gdbt_players.GetPlayerName(local_player->GetPid(), buffer, MAX_NAME-1);
    f->Print(&surface, buffer, color);
	f->Print(&surface, ": ", color);
	f->Print(&surface, m_buffer, color);

	if (m_Slew_Y != f->GetY())
	{
	  m_Slew_Y = f->GetY();
	  if (m_Slew_Y > (viewHeight()-20))
		SlewTo(0,m_Slew_Y-(viewHeight()-20));
	  else
		SlewTo(0,0);
	}
  }

  SCROLLVIEW::Paint(etime, flags);
}

//----------------------------------------------------------------------------
int TELETYPE::Msg(int msg_id, int flags)
{
  switch(msg_id)
  {
		case 0:
		  local_player->SetListenGlobal(FALSE);
		  gdbt_messages.SendFreqData(ALL_PLAYERS);
		  break;
		case 1:
		  local_player->SetListenView(FALSE);
		  gdbt_messages.SendFreqData(ALL_PLAYERS);
		  break;
		case 2:
		  local_player->SetListenFreq(FALSE);
		  gdbt_messages.SendFreqData(ALL_PLAYERS);
		  break;
		case 3:
		  local_player->SetListenIntimate(FALSE);
		  gdbt_messages.SendFreqData(ALL_PLAYERS);
		  break;
		case 4:
		  local_player->SetListenGlobal(TRUE);
		  gdbt_messages.SendFreqData(ALL_PLAYERS);
		  break;
		case 5:
		  local_player->SetListenView(TRUE);
		  gdbt_messages.SendFreqData(ALL_PLAYERS);
		  break;
		case 6:
		  local_player->SetListenFreq(TRUE);
		  gdbt_messages.SendFreqData(ALL_PLAYERS);
		  break;
		case 7:
		  local_player->SetListenIntimate(TRUE);
		  gdbt_messages.SendFreqData(ALL_PLAYERS);
		  break;
		case 8:
		  SlewRel(0, -50);
		  break;
		case 9:
		  break;
		case 10:
		  SlewRel(0, +50);
		  break;
		case 11:
		  break;
  }
  return 0;
}

//----------------------------------------------------------------------------
int TELETYPE::Keycmd(int key, int flags)
{
  	if (flags & KEY_PRESSED)
	{
	  if (key >= ' ' && key <= 128)
	  {
		if (m_index <= MAX_TELETYPE_MESSAGE_LEN-1)
		{
		  m_buffer[m_index++] = key;  // add char to the buffer
		  Invalidate();	  // and force an update.
		}
	  }
	  else
	    switch( key )
	    {
		case 0:
		case 1:
		case 2:
		case 3:
		  c_freq = key;
		  Invalidate();
		  break;

		case KEY_RETURN:
		  // send any text in the buffer,  ready new buffer.
		  if (!WIZARD::Parse(m_buffer))
		  {
			if (m_index > 0)
			{
				int lang;
				if (c_lang)
					lang = c_lang;
				else
				{
					if (CULT_VILLAGE == local_player->GetCult())
						lang = LANG_VILLAGE+1;
					else
						lang = LANG_CITY+1;
				}
				_AppendMessage(local_player->GetPid(), m_buffer, c_freq, lang);
				gdbt_messages.SendMessage(m_buffer, m_index, c_freq, lang);
			}
			else
			  m_Slew_Y = 0;
		  }
		  InitBuffer();
		  Invalidate();
		  break;
		case KEY_BACK:
		  if (m_index)
		  {
			m_buffer[--m_index] = 0;
			Invalidate();
		  }
		  break;
		default:
		  key = SCROLLVIEW::Keycmd(key, flags);
	    }
	}
	else
	{
	    switch( key )
	    {
		case KEY_RETURN:
		  key = 0;
		  break;
		default:
		  key = SCROLLVIEW::Keycmd(key, flags);
	    }
	}

  return 0; // use up any keystroke.
}

//----------------------------------------------------------------------------
void TELETYPE::Msg( DWORD pid, const LPSTR fmt, ... )
{
    char        buff[256];
    
    va_list a_list;
    va_start( a_list, fmt );
    
    vsprintf( buff, fmt, a_list);
	AppendMessage(pid, buff, 4, 0);
} /* Msg */

//----------------------------------------------------------------------------
void TELETYPE::_AppendMessage(DWORD pid, char *message, BYTE freq, BYTE lang)
{
  strncpy(m_message[m_last_message].buffer, message, MAX_TELETYPE_MESSAGE_LEN-1);
  m_message[m_last_message].buffer[MAX_TELETYPE_MESSAGE_LEN-1] = 0;
  m_message[m_last_message].pid = pid;
  m_message[m_last_message].freq = freq;
  m_message[m_last_message].lang = lang;
  m_message[m_last_message].name[0] = 0;
  
  if (++m_last_message >= MAX_TELETYPE_MESSAGES)
	m_last_message = 0;

  if (++m_message_count >= MAX_TELETYPE_MESSAGES)
  {
	m_message_count = MAX_TELETYPE_MESSAGES;

	// first message was overwritten
	if (++m_first_message >= MAX_TELETYPE_MESSAGES)
		m_first_message = 0;
  }

  Invalidate();
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

#define PROC_TEXT_MESSAGE	1
#define PROC_FREQ_DATA		2

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void GDBT_MESSAGES::SendMessage(char * msg, int length, BYTE freq, BYTE lang)
{
  TELE_MESSAGE pkt;
  int header_size = sizeof(TELE_MESSAGE) - sizeof(pkt.buffer);

  pkt.pid = local_player->GetPid();
  pkt.freq = freq;
  pkt.lang = lang;
  pkt.length = strlen(msg);
  strncpy(pkt.buffer, msg, MAX_TELETYPE_MESSAGE_LEN);
  chk(pkt.buffer, pkt.length);

  SendUpdate(0, GLOBAL_LISTENERS<<TELETYPE::GetFreq(), PROC_TEXT_MESSAGE, (UDATA*)&pkt, header_size + length + 1);
}

//----------------------------------------------------------------------------
void GDBT_MESSAGES::SendFreqData(WORD to_group)
{
  FREQOUT pkt;

  pkt = local_player->GetFreqOut();
  SendUpdate(0, to_group, PROC_FREQ_DATA, (UDATA*)&pkt, sizeof(pkt));
}

//----------------------------------------------------------------------------
void GDBT_MESSAGES::ReceiveUpdate(DPID from, DPID to, WORD proc, UDATA *data)
{
  switch (proc)
  {
	case PROC_TEXT_MESSAGE:
	  {
	  TELE_MESSAGE *pkt = (TELE_MESSAGE*)data;
	  chk(pkt->buffer, pkt->length);
	  TELETYPE::AppendMessage(pkt->pid, pkt->buffer, pkt->freq, pkt->lang);
	  }
	  break;
	case PROC_FREQ_DATA:
	  {
	  FREQOUT *pkt = (FREQOUT*)data;
	  PLAYER *player = gdbt_players.Get(from);
	  if (player)
		player->SetFreqOut(*pkt);
	  }
	  break;
  }
}

void TELETYPE::_DelPlayer(DWORD pid)
{
  char name[MAX_NAME];

  gdbt_players.GetPlayerName(pid, name, MAX_NAME-1);

  Msg(0, "%s has left the game", name);

  // convert all her messages to system messages:
  for (int i=0; i<MAX_TELETYPE_MESSAGES;i++)
  {
		if (m_message[i].pid == pid)
		{
			m_message[i].pid = 0;
			strncpy(m_message[i].name, name, MAX_NAME-1);
			m_message[i].name[MAX_NAME-1] = 0;
		}
  }
}

