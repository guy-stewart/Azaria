
#include "mask.h"


//----------------------------------------------------------------------------
int MASK::Read(const char *path, const char *filename, MASK **vector)
{
	int fh;
	int rc = 0;
	MASK header;
	char lpszPathName[_MAX_PATH];

	// build complete path the the file
	makepath( lpszPathName, NULL, path, filename, NULL);

	/* Open file for input: */
	if( (fh = caseopen( lpszPathName, O_RDONLY, S_IREAD|S_IWRITE )) != -1 )
	{
		/* Read in size of the mask */
		if( read( fh, &header, sizeof(MASK) ) == sizeof(MASK) )
		{
			/* allocate memory for the mask */
			*vector = (MASK*)malloc((header.length+1) * sizeof(MASK));
			if (vector)
			{
				/* copy the header */
				*vector[0] = header;

				/* Read the rest of the mask */
				// ### For osme reason,  the masks are not the correct length according to header.length???
				rc = read( fh, *vector+1, sizeof(MASK)*header.length);
				rc = rc > 0;
			}
		}
		close( fh );
	}

	return rc;
}

MASKS::MASKS(int c) {
	count = c;
	masks = (MASK**)malloc(count * sizeof(MASK*));
}
MASKS::~MASKS() {
	free(masks);
}


//----------------------------------------------------------------------------
MASKS * MASK::Create(PIXEL *x_bits, int f, int h, int w, int pitch, PIXEL ck) {
	MASKS *masks;
	masks = new MASKS(f);

	for (int i=0; i<f; i++) {
		MASK *mask = MASK::Create(x_bits, h/f, w, pitch, ck);
		x_bits += pitch * h/f;
		masks->setMask(i, mask);
	}
	return masks;
}

//----------------------------------------------------------------------------
MASK * MASK::Create(PIXEL *x_bits, int h, int w, int pitch, PIXEL ck)
{
	int runs; // how many runs in the image.
	int index;
	MASK *mask;
	int x, y;
	int top=0, left=0;

	PIXEL *lm_bits;

	runs = 0;
	lm_bits = x_bits;
	// figure out the size of the mask:
	for (y=0; y<h; y++)
	{
		x=0;
		while (x<w)
		{
			/* find start of run (or end of row) */
			while (x<w && (*lm_bits==ck))
			{
				lm_bits++;
				x++; 
			}
			if (x<w) // found a run
			{
				while (x<w && (*lm_bits!=ck))
				{ 
					lm_bits++;
					x++;
				}  /* find end of run */
				runs++;
			}
		}

		while (x<pitch)
		{
			lm_bits++;
			x++;
		}
	}

	mask = (MASK*)malloc(sizeof(MASK)*(runs+1));
	if (!mask)
		return 0;

	lm_bits = x_bits;
	// Now do it for real:
	index = 1;
	for (y=0; y<h; y++)
	{
		x=0;
		while (x<w)
		{
			/* find start of run (or end of row) */
			while (x<w && (*lm_bits==ck)) 
			{ 
				x++;
				lm_bits++;
			}
			if (x<w) // found a run
			{
				if (y<top)	top = y;
				if (x<left)	left=x;
				mask[index].y = y;
				mask[index].x = x;
				while (x<w && (*lm_bits!=ck))  /* find end of run */
				{ 
					x++;
					lm_bits++;
				}
				mask[index].length = x - mask[index].x;
				index++;
			}
		}

		while (x<pitch)
		{
			lm_bits++;
			x++;
		}
	}

	mask[0].length = runs;
	mask[0].x = left;
	mask[0].y = top;
	return mask;
}

//----------------------------------------------------------------------------
/*  returns true if the vector could be freed */
int MASK::Free(MASK **vector)
{
	if (vector && *vector)
		free(*vector);
	else
		return FALSE;
	*vector = NULL;
	return TRUE;
}

//----------------------------------------------------------------------------
int MASK::Write(const char *lpszPathName, MASK *vector)
{
	XFile xfile(lpszPathName);
	int rc = 0;

	xfile.SetExtension("vct");

	/* Open file for output: */
	if(xfile.Open(XFile::modeWrite))
	{
		rc = xfile.Write(vector, sizeof(MASK)*(vector[0].length + 1));
	}

	return rc;	
}

//----------------------------------------------------------------------------
void MASK::GetBoundingRect(RECT *rect)
{
    MASK *mask = this;
	int i, x, y, length;

	/* keep track of dirty rectangle */
	rect->left = mask[1].x;
	rect->right = mask[1].x + mask[1].length;
	rect->top = mask[1].y;
	rect->bottom = mask[1].y;

	for (i=1; i<=mask[0].length; i++)
	{
		length = mask[i].length;
		x = mask[i].x;
		y = mask[i].y;

		/* keep track of dirty rectangle */
		if (x < rect->left) rect->left = x;
		else if ((x+length) > rect->right) rect->right = x+length;
		if (y < rect->top) rect->top = y;
		else if (y > rect->bottom) rect->bottom = y;
	}

}

//----------------------------------------------------------------------------
// ### This is certainly the least efficient method of doing a hit test.
// This function gets called every time the mouse moves.  It would
// be nice if I could create an 'inner bounding box' that could be tested
// before doing all this work:

BOOL MASK::PtInRect(POINT p)
{
    MASK *mask = this;
  int i, x;

  for (i=1; i<=mask[0].length; i++)
  {
	x = mask[i].x;
	if ((p.y == mask[i].y) && (p.x > x) && (p.x < x+mask[i].length))
	  return TRUE;
  }

  return FALSE;
}

//----------------------------------------------------------------------------
void MASK::Move(int x_delta, int y_delta)
{
    MASK *mask = this;
  int i;

  for (i=1; i<=mask[0].length; i++)
  {
	mask[i].x += x_delta;
	mask[i].y += y_delta;
  }
}
