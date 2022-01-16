/* See LICENSE file for copyright and license details. */

#include "layouts.c"

/* appearance */
static const unsigned int borderpx  = 4;        /* border pixel of windows */
static const unsigned int snap      = 10;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const unsigned int barheight = 30;       /* status bar height */
static const char *fonts[]          = { "DejaVu Sans Mono:size=12",
										"Font Awesome 5 Brands:size=11",
										"Font Awesome 5 Free Regular:size=11",
										"Font Awesome 5 Free Solid:size=11"};
static const int stackbar_width     = 10;      /* width of stackbar blocks */
static const int stackbar_padding   = 2;       /* padding for stackbar blocks */
static const int focusonwheel       = 1;
static const int unhiden_statusbar  = 1;       /*  shows status bar on all monitors */
static const char *colors[][2]      = {
	/*             fg         bg       */
	[Color0]  = { "#e6beae", "#000000" },
	[Color1]  = { "#ffffff", "#7818b4" },
	[Color2]  = { "#654d4d", "#000000" },
	[Color3]  = { "#000000", "#3ee2b4" },
	[Color4]  = { "#000000", "#ffa500" },
	[Color5]  = { "#000000", "#ff6347" },
	[Color6]  = { "#e6beae", "#392626" },
	[Color7]  = { "#000000", "#a7e23e" },
	[Color8]  = { "#6fd8ff", "#000000" },
	[Color9]  = { "#ffffff", "#292121" },
	[Color10] = { "#000000", "#e6beae" },
};

/* tagging */
static const char *tags[] = { "", "", "", "" , "", "", "", "", ""};

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance       title tags mask isfloating islocked follow iscentered skip last  monitor */
	{ "URxvt",    "htop",        NULL, 0,        0,         0,       1,     0,         0,   0,    -1 },
	{ "URxvt",    "log",         NULL, 0,        0,         0,       1,     0,         0,   0,    -1 },
	{ "URxvt",    "pulsemixer",  NULL, 0,        1,         0,       0,     1,         0,   0,    -1 },
	{ "Zathura",  NULL,          NULL, 1 << 3,   0,         0,       1,     0,         0,   0,     1 },
};

/* layout(s) */
static const float mfact     = 0.5;  /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 0; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[@]",      spiral }, /* first entry is default */
	{ "[F]",      monocle },
	{ "[\\]",     dwindle },
	{ "[+]",      grid },
	{ "><>",      NULL },    /* no layout function means floating behavior */
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static const char *termcmd[]  = { "/usr/bin/urxvt", NULL };

// System-wide keybindings that are related to DWM. Other system-wide
// keybindings are managed by SXHKD. For more information, check
// github.com/yad-tahir/dotfiles
static Key keys[] = {
	/* modifier                     key         function             argument */
	{ MODKEY|ShiftMask,             XK_Escape, spawn,               {.v = termcmd } },
	{ MODKEY|ShiftMask,				XK_grave,  spawn,               {.v = termcmd } },
	{ 0,                            XK_Print,  togglebar,           {0} },

	{ MODKEY,                       XK_h,      focusstack,          {.i = -1 } },
	{ MODKEY,                       XK_n,      focusstack,          {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_h,      focusstack,          {.i = -2 } },
	{ MODKEY|ShiftMask,             XK_n,      focusstack,          {.i = +2 } },
	{ MODKEY|ControlMask,           XK_h,      movestack,           {.i = -1 } },
	{ MODKEY|ControlMask,           XK_n,      movestack,           {.i = +1 } },
	{ MODKEY|ControlMask|ShiftMask, XK_h,      movestack,           {.i = -2 } },
	{ MODKEY|ControlMask|ShiftMask, XK_n,      movestack,           {.i = +2 } },
	{ MODKEY,                       XK_Tab,    focusstack,          {.i = +2 } },
	{ MODKEY|ShiftMask,             XK_Tab,    focusstack,          {.i = -2 } },

	{ MODKEY,                       XK_c,      focusmon,            {.i = -1 } },
	{ MODKEY,                       XK_t,      focusmon,            {.i = +1 } },
	{ MODKEY|ControlMask,           XK_c,      tagmon,              {.i = -1 } },
	{ MODKEY|ControlMask,           XK_t,      tagmon,              {.i = +1 } },

	{ MODKEY,                       XK_Up,     incnmaster,          {.i = +1 } },
	{ MODKEY,                       XK_Down,   incnmaster,          {.i = -1 } },
	{ MODKEY,                       XK_Left,   setmfact,            {.f = -0.025 } },
	{ MODKEY,                       XK_Right,  setmfact,            {.f = +0.025 } },

	{ MODKEY,                       XK_s,      zoom,                {.i = 0} },
	{ MODKEY|ShiftMask,             XK_s,      zoom,                {.i = +1} },
	{ MODKEY,                       XK_r,      inplacerotate,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_r,      inplacerotate,       {.i = +2 } },
	{ MODKEY,                       XK_b,      rotatestack,         {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_b,      rotatestack,         {.i = +2 } },
	{ MODKEY,                       XK_Return, cyclelayout,         {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_Return, cyclelayout,         {.i = -1 } },
	{ MODKEY,                       XK_space,  togglefloating,      {0} },
	{ MODKEY,                       XK_f,      stickyClient,        {.i = +1} },
	{ MODKEY|ShiftMask,             XK_f,      stickyClient,        {.i = -1} },
	{ MODKEY,                       XK_z,      togglelast,          {0} },
	{ MODKEY,                       XK_l,      togglelocked,        {0} },
	{ MODKEY,                       XK_x,      toggleskip,          {0} },
	{ MODKEY|ShiftMask,             XK_x,      unskipall,           {0} },
	{ MODKEY,                       XK_minus,  tocenter,            {0} },

	{ MODKEY,                       XK_0,      view,                {.ui = ~0 } },
	{ MODKEY|ControlMask,           XK_0,      tag,                 {.ui = ~0 } },
	TAGKEYS(                        XK_1,                           0)
	TAGKEYS(                        XK_2,                           1)
	TAGKEYS(                        XK_3,                           2)
	TAGKEYS(                        XK_4,                           3)
	TAGKEYS(                        XK_5,                           4)
	TAGKEYS(                        XK_6,                           5)
	TAGKEYS(                        XK_7,                           6)
	TAGKEYS(                        XK_8,                           7)
	TAGKEYS(                        XK_9,                           8)

	{ MODKEY,                       XK_q,      killclient,          {0} },
	{ MODKEY|ShiftMask,             XK_q,      killforceclient,     {0} },
	{ MODKEY|ControlMask,           XK_Delete, quit,                {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkStackBar,          0,              Button1,        setclientfocus, {0} },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        resizemouse,    {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

/* signal definitions */
/* signum must be greater than 0 */
/* trigger signals using `xsetroot -name "fsignal:<signame> [<type> <value>]"` */
static Signal signals[] = {
	/* signum           function */
	{ "focusstack",     focusstack },
	{ "setmfact",       setmfact },
	{ "setmfacts",		setmfacts },
	{ "togglebar",      togglebar },
	{ "incnmaster",     incnmaster },
	{ "setnmasters",     setnmasters },
	{ "togglefloating", togglefloating },
	{ "focusmon",       focusmon },
	{ "tagmon",         tagmon },
	{ "zoom",           zoom },
	{ "view",           view },
	{ "tag",            tag },
	{ "toggletag",      tag },
	{ "killclient",     killclient },
	{ "quit",           quit },
};
