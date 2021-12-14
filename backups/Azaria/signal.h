
enum {
  /////////////////////////////
  // system sigs defined in machterp.h
	SIG_NULL = 0,
	SIG_INIT,			// Sent to all machines when a game is loaded (not transfered!)
	SIG_VIEWINIT,		// sent to all machines in a view when the view is loaded
  /////////////////////////////

	SIG_OBJECT,
	SIG_HAPPY,
	SIG_HURT,
	SIG_KISS,
	SIG_MAD,
	SIG_SAD,
	SIG_SURPRISED,
	SIG_STRESS,
	SIG_WAVE,

	SIG_BURST,
	SIG_BOMB,
	SIG_BURN,
	SIG_OPEN,
	SIG_CLOSE,
	SIG_ON,
	SIG_OFF,
	SIG_ENTER_1,

	SIG_ADD, 
	SIG_SUB,
	SIG_SET,
	SIG_CLEAR,

	SIG_INC1,
	SIG_DEC1,
	SIG_INC10,
	SIG_DEC10,
};


