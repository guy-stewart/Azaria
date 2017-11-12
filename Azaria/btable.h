/*----------------------------------------------------------------------------
; Copyright (C) 1995, 1996 Walter Bennett Advanced Technologies.  
;
; All Rights Reserved.  The source  code contained  in this  file is the 
; confidential property of Walter Bennett Advanced Technologies. No part
; of  this document  may be reproduced  or transmitted in any form or by
; any means,  electronic  or  mechanical,  for any purpose,  without the 
; express  written  permission of  Walter Bennett Advanced Technologies.
;
; File:       btable.h
;
;	REASON:
;	Most tables in Kamaria are sorted by view and have a unique but
;	unsorted primary key.  This btable implementation supports just
;	such tables.
;
;	Implements tables that are sorted by groups.  binary searches are
;	used to find specific items and groups of items.
;
;	Supports tables that are sorted by groups of items (GROUPKEY)
;	that also have a unique sorted key (PKEY)
;	as will as a unique unsorted secondary key (UKEY).
;
;	Get  functions expect table to be sorted by GROUPKEY.
;	UGet methods do not expect table to be sorted by UKEY
;	PGet methods expect table to be sorted by PKEY
;
; NOTES:
; Tables that do not have a primary key can use the groupkey for primary
; key methods such as insert and delete.
;
;
; Description:
;
; HISTORY
;
;   Date    Who  Comment
; --------  ---  -------------------------------------------------------------
; 04/04/96  gas  Created.
; 
----------------------------------------------------------------------------*/
#ifndef __BTABLE_INCLUDED__
#define __BTABLE_INCLUDED__

#define B_TABLE_PARMS B_TABLE<T, PKEY, UKEY, GROUPKEY>
#define B_TABLE_TEMPLATE template<class T, class PKEY, class UKEY, class GROUPKEY>
B_TABLE_TEMPLATE class B_TABLE
{
protected:
	T *m_table;
	int m_count;
	int m_max;		// max entries until full.
public:
	B_TABLE(T *table, int count, int initial_size);

public:
	int Count() {return m_count;}

private:
	BOOL _delete(int position);

protected:
	T *PGet(PKEY key);
	T *PGet(PKEY key, int *position);

	T *UGet(UKEY key);
	T *GetFirst(GROUPKEY key, int *position);
	T *GetNext(GROUPKEY key, int *position);
	T *GetLast(GROUPKEY key, int *position);
	T *GetPrev(GROUPKEY key, int *position);
	T *Insert(T *t);
	T *GetInsertPosition(GROUPKEY key, int *position);

	BOOL Validate();
public:
	BOOL Delete(PKEY pkey);
	T *Hit(GROUPKEY key, POINT p);
};

//----------------------------------------------------------------------------
B_TABLE_TEMPLATE
B_TABLE_PARMS::B_TABLE(T *table, int count, int initial_size)
{
	m_table = table;
	m_count = count;
	m_max = initial_size;

	Validate();
}

//----------------------------------------------------------------------------
B_TABLE_TEMPLATE
BOOL B_TABLE_PARMS::Validate()
{
	int i;

	// ### Assure that the table is sorted by GROUPKEY
	for (i=1;i<m_count;i++)
	{
		assert(m_table[i-1].GetGroupKey() <= m_table[i].GetGroupKey());
	}

	return TRUE;
}

//----------------------------------------------------------------------------
// this is any old unsorted key that must use a linear search:
B_TABLE_TEMPLATE
T * B_TABLE_PARMS::UGet(UKEY key)
{
	int i;
	for (i=0;i<m_count; (i)++)
		if (m_table[i].GetUKey() == key)
			return &m_table[i];

	return NULL;
}

//----------------------------------------------------------------------------
// This is a unique sorted key that can be searched on with a binary search:
B_TABLE_TEMPLATE
T * B_TABLE_PARMS::PGet(PKEY key, int *position)
{
  int hi, lo, mid;

  // search doesn't work on an empty table:
  if (!m_count)
	return NULL;

  hi = m_count;
  lo = 0;

  while (hi>lo)
  {
	mid = (hi+lo)>>1;

	if (key <= m_table[mid].GetPKey())
	  hi = mid; // look in the lower partition
	else // must be >
	  lo = mid+1; // look in the upper partition
  }

  // did we find it?
  if (m_table[hi].GetPKey() == key)
  {
	  *position = hi;
	  return &m_table[hi];
  }

  // never found it
  return NULL;
}

//----------------------------------------------------------------------------
// This is a unique sorted key that can be searched on with a binary search:
B_TABLE_TEMPLATE
T * B_TABLE_PARMS::PGet(PKEY key)
{
	int position;
	return PGet(key, &position);
}

//----------------------------------------------------------------------------
B_TABLE_TEMPLATE
T * B_TABLE_PARMS::GetInsertPosition(GROUPKEY key, int *position)
{
	for (*position=0;*position<m_count; (*position)++)
		if (m_table[*position].GetGroupKey() >= key)
			return &m_table[*position];

	// append to end of table:
	return &m_table[*position];
}

//----------------------------------------------------------------------------
B_TABLE_TEMPLATE
T * B_TABLE_PARMS::Insert(T *t)
{
	T *insert_pos;
	int position;

	GROUPKEY key = t->GetGroupKey();

	if (m_count == m_max)
	{
		insert_pos = NULL;
	}
	// if the table is empty then insert at 0th position:
	else if (m_count == 0)
	{
		insert_pos = m_table;
		position = 0;
	}
	// otherwise find the best place to insert:
	else
	{
		insert_pos = GetInsertPosition(key, &position);
	}

	if (insert_pos)
	{
		memmove(insert_pos+1, insert_pos, sizeof(T)*(m_count-position) );
		memcpy(insert_pos, t, sizeof(T));
		m_count++;
	}

	return insert_pos;
}

//----------------------------------------------------------------------------
B_TABLE_TEMPLATE
BOOL B_TABLE_PARMS::_delete(int position)
{
	if (position < 0 || position >= m_count)
		return FALSE;

	m_count--;

	if (position < m_count)
		memmove(m_table+position, m_table+position+1, sizeof(T)*(m_count-position) );

	return TRUE;
}

//----------------------------------------------------------------------------
B_TABLE_TEMPLATE
BOOL B_TABLE_PARMS::Delete(PKEY pkey)
{
	int position;
	if (PGet(pkey, &position))
	{
		return _delete(position);
	}
	else
		return FALSE;
}

//----------------------------------------------------------------------------
// Selects the element within the group that has been 'hit'
B_TABLE_TEMPLATE
T * B_TABLE_PARMS::Hit(GROUPKEY key, POINT p)
{
	T *t;
	int position;

	t = GetFirst(key, &position);
	while (t)
	{
		if (sPtInRect(t->GetRegion(), p))
		{
			return t;
		}
		t = GetNext(key, &position);
	}
	return NULL;
}

//----------------------------------------------------------------------------
// Find first element in a group.  The table must be sorted by GROUPKEY.
B_TABLE_TEMPLATE
T * B_TABLE_PARMS::GetFirst(GROUPKEY key, int *position)
{
  int hi, lo, mid;

  // search doesn't work on an empty table:
  if (!m_count)
	return NULL;

  hi = m_count;
  lo = 0;

  while (hi>lo)
  {
	mid = (hi+lo)>>1;

	if (key <= m_table[mid].GetGroupKey())
	  hi = mid; // look in the lower partition
	else // must be >
	  lo = mid+1; // look in the upper partition
  }

  // did we find it?
  if (m_table[hi].GetGroupKey() == key)
  {
	  *position = hi;
	  return &m_table[hi];
  }

  // never found it
  return NULL;
}

//----------------------------------------------------------------------------
B_TABLE_TEMPLATE
T * B_TABLE_PARMS::GetNext(GROUPKEY key, int *position)
{
	int i;
	GROUPKEY test_key;

	if (*position < 0)
		return NULL;

	for (i=*position+1;i<m_count; i++)
	{
		test_key = m_table[i].GetGroupKey();

		if (test_key == key)
		{
			*position = i;
			return &m_table[i];
		}
		else if (test_key < key)
			break; // we have passed the end of the group.
	}
	return NULL;
}

//----------------------------------------------------------------------------
// Find last element in a group
B_TABLE_TEMPLATE
T * B_TABLE_PARMS::GetLast(GROUPKEY key, int *position)
{
	int i;
	*position=0;
	for (i=m_count-1;i>=0; i--)
	{
		if (m_table[i].GetGroupKey() == key)
		{
			*position = i;
			return &m_table[i];
		}
	}
	return NULL;
}

//----------------------------------------------------------------------------
B_TABLE_TEMPLATE
T * B_TABLE_PARMS::GetPrev(GROUPKEY key, int *position)
{
	int i;
	GROUPKEY test_key;

	if (*position < 0)
		return NULL;

	for (i=*position-1;i>=0; i--)
	{
		test_key = m_table[i].GetGroupKey();

		if (test_key == key)
		{
			*position = i;
			return &m_table[i];
		}
		else if (test_key > key)
			break; // we have passed the start of the group.
	}
	return NULL;
}


#endif // __BTABLE_INCLUDED__