
#define IDM_MAINMENU		1
#define IDM_NETSELECT		2
#define IDM_NETCONFIG		3
#define IDM_SOUND			4

static MENU tbl_menus[] = {
	{IDM_MAINMENU,	0,			"    Azaria",		"menu.one"},
	{IDM_NETSELECT,	IDM_MAINMENU,	"Confound Network",	"menu.one"},
	{IDM_SOUND,		IDM_MAINMENU,	"Sound Setup",		"menu.one"},
};

static MENU_ITEM tbl_menu_items[] = 
{
	{IDM_MAINMENU,	"Play the Game",	menuCallView,	IDV_CONTINENT},
	{IDM_MAINMENU,	"Network Play",		menuCallMenu,	IDM_NETSELECT},
	{IDM_MAINMENU,	"Audio Setup",		menuCallMenu,	IDM_SOUND},
	{IDM_MAINMENU,	"Exit",				menuEsc,		0},

	{IDM_NETSELECT,	"Exit",				menuCallMenu,	IDM_MAINMENU},
	{IDM_NETSELECT,	"Service Providers",menuCallView,	IDV_PROVIDERS},
	{IDM_NETSELECT,	"Join Session",		menuCallView,	IDV_SESSIONS},
	{IDM_NETSELECT,	"Create Session",	menuCallView,	IDV_NEWSESSION},
	{IDM_NETSELECT,	"Create Player",	menuCallView,	IDV_NEWPLAYER},

	{IDM_SOUND,		"Exit",				menuCallMenu,	IDM_MAINMENU},
};
