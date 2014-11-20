/* valid curses attributes are listed below they can be ORed
 *
 * A_NORMAL        Normal display (no highlight)
 * A_STANDOUT      Best highlighting mode of the terminal.
 * A_UNDERLINE     Underlining
 * A_REVERSE       Reverse video
 * A_BLINK         Blinking
 * A_DIM           Half bright
 * A_BOLD          Extra bright or bold
 * A_PROTECT       Protected mode
 * A_INVIS         Invisible or blank mode
 */

enum {
	DEFAULT,
	BLUE,
};

static Color colors[] = {
	[DEFAULT] = { .fg = -1,         .bg = -1, .fg256 = -1, .bg256 = -1, },
	[BLUE]    = { .fg = COLOR_BLUE, .bg = -1, .fg256 = 68, .bg256 = -1, },
};

#define COLOR(c)        COLOR_PAIR(colors[c].pair)
/* curses attributes for the currently focused window */
#define SELECTED_ATTR   (COLOR(BLUE) | A_NORMAL)
/* curses attributes for normal (not selected) windows */
#define NORMAL_ATTR     (COLOR(DEFAULT) | A_NORMAL)
/* curses attributes for the status bar */
#define BAR_ATTR        (COLOR(BLUE) | A_NORMAL)
/* status bar (command line option -s) position */
#define BAR_POS         BAR_TOP /* BAR_BOTTOM, BAR_OFF */
/* whether status bar should be hidden if only one client exists */
#define BAR_AUTOHIDE    true
/* determines whether the statusbar text should be right or left aligned */
#define BAR_ALIGN       ALIGN_RIGHT
/* separator between window title and window number */
#define SEPARATOR " | "
/* printf format string for the window title, first %s
 * is replaced by the title, second %s is replaced by
 * the SEPARATOR, %d stands for the window number */
#define TITLE "[%s%s#%d]"
/* master width factor [0.1 .. 0.9] */
#define MFACT 0.5
/* scroll back buffer size in lines */
#define SCROLL_HISTORY 10000
/* printf format string for the tag in the status bar */
#define TAG_SYMBOL   "[%s]"
/* curses attributes for the currently selected tags */
#define TAG_SEL      (COLOR(BLUE) | A_BOLD)
/* curses attributes for not selected tags which contain no windows */
#define TAG_NORMAL   (COLOR(DEFAULT) | A_NORMAL)
/* curses attributes for not selected tags which contain windows */
#define TAG_OCCUPIED (COLOR(BLUE) | A_NORMAL)

const char tags[][8] = { "1", "2", "3", "4", "5" };

#include "tile.c"
#include "grid.c"
#include "bstack.c"
#include "fullscreen.c"

/* by default the first layout entry is used */
static Layout layouts[] = {
	{ "[]=", tile },
	{ "+++", grid },
	{ "TTT", bstack },
	{ "[ ]", fullscreen },
};

#define MOD CTRL('g')

#define __bindings(mod) \
	{ { mod, 'C',          }, { create,         { NULL }                    } }, \
	{ { mod, 'c',          }, { create,         { NULL, NULL, "$CWD" }      } }, \
	{ { mod, 'x',          }, { killclient,     { NULL }                    } }, \
	{ { mod, 'j',          }, { focusnext,      { NULL }                    } }, \
	{ { mod, 'u',          }, { focusnextnm,    { NULL }                    } }, \
	{ { mod, 'i',          }, { focusprevnm,    { NULL }                    } }, \
	{ { mod, 'k',          }, { focusprev,      { NULL }                    } }, \
	{ { mod, 'd',          }, { setlayout,      { "[]=" }                   } }, \
	{ { mod, 'g',          }, { setlayout,      { "+++" }                   } }, \
	{ { mod, 'b',          }, { setlayout,      { "TTT" }                   } }, \
	{ { mod, 'm',          }, { setlayout,      { "[ ]" }                   } }, \
	{ { mod, ' ',          }, { setlayout,      { NULL }                    } }, \
	{ { mod, 'h',          }, { setmfact,       { "-0.05" }                 } }, \
	{ { mod, 'l',          }, { setmfact,       { "+0.05" }                 } }, \
	{ { mod, '.',          }, { toggleminimize, { NULL }                    } }, \
	{ { mod, 's',          }, { togglebar,      { NULL }                    } }, \
	{ { mod, 'M',          }, { togglemouse,    { NULL }                    } }, \
	{ { mod, '\n',         }, { zoom ,          { NULL }                    } }, \
	{ { mod, '1',          }, { focusn,         { "1" }                     } }, \
	{ { mod, '2',          }, { focusn,         { "2" }                     } }, \
	{ { mod, '3',          }, { focusn,         { "3" }                     } }, \
	{ { mod, '4',          }, { focusn,         { "4" }                     } }, \
	{ { mod, '5',          }, { focusn,         { "5" }                     } }, \
	{ { mod, '6',          }, { focusn,         { "6" }                     } }, \
	{ { mod, '7',          }, { focusn,         { "7" }                     } }, \
	{ { mod, '8',          }, { focusn,         { "8" }                     } }, \
	{ { mod, '9',          }, { focusn,         { "9" }                     } }, \
	{ { mod, '\t',         }, { focuslast,      { NULL }                    } }, \
	{ { mod, 'q',          }, { quit,           { NULL }                    } }, \
	{ { mod, 'a',          }, { togglerunall,   { NULL }                    } }, \
	{ { mod, CTRL('L'),    }, { redraw,         { NULL }                    } }, \
	{ { mod, 'r',          }, { redraw,         { NULL }                    } }, \
	{ { mod, 'B',          }, { togglebell,     { NULL }                    } }, \
	{ { mod, 'e',          }, { copymode,       { NULL }                    } }, \
	{ { mod, '/',          }, { copymode,       { "/" }                     } }, \
	{ { mod, 'p',          }, { paste,          { NULL }                    } }, \
	{ { mod, KEY_PPAGE,    }, { scrollback,     { "-1" }                    } }, \
	{ { mod, KEY_NPAGE,    }, { scrollback,     { "1"  }                    } }, \
	{ { mod, '?',          }, { create,         { "man dvtm", "dvtm help" } } }, \
	{ { mod, mod,          }, { send,           { (const char []){mod, 0} } } }, \
	{ { KEY_SPREVIOUS,     }, { scrollback,     { "-1" }                    } }, \
	{ { KEY_SNEXT,         }, { scrollback,     { "1"  }                    } }, \
	{ { mod, '0',          }, { view,           { NULL }                    } }, \
	{ { mod, KEY_F(1),     }, { view,           { tags[0] }                 } }, \
	{ { mod, KEY_F(2),     }, { view,           { tags[1] }                 } }, \
	{ { mod, KEY_F(3),     }, { view,           { tags[2] }                 } }, \
	{ { mod, KEY_F(4),     }, { view,           { tags[3] }                 } }, \
	{ { mod, KEY_F(5),     }, { view,           { tags[4] }                 } }, \
	{ { mod, 'v', '1'      }, { view,           { tags[0] }                 } }, \
	{ { mod, 'v', '2'      }, { view,           { tags[1] }                 } }, \
	{ { mod, 'v', '3'      }, { view,           { tags[2] }                 } }, \
	{ { mod, 'v', '4'      }, { view,           { tags[3] }                 } }, \
	{ { mod, 'v', '5'      }, { view,           { tags[4] }                 } }, \
	{ { mod, 'v', '\t',    }, { viewprevtag,    { NULL }                    } }, \
	{ { mod, 't', '0'      }, { tag,            { NULL }                    } }, \
	{ { mod, 't', '1'      }, { tag,            { tags[0] }                 } }, \
	{ { mod, 't', '2'      }, { tag,            { tags[1] }                 } }, \
	{ { mod, 't', '3'      }, { tag,            { tags[2] }                 } }, \
	{ { mod, 't', '4'      }, { tag,            { tags[3] }                 } }, \
	{ { mod, 't', '5'      }, { tag,            { tags[4] }                 } }, \
	{ { mod, 'V', '1'      }, { toggleview,     { tags[0] }                 } }, \
	{ { mod, 'V', '2'      }, { toggleview,     { tags[1] }                 } }, \
	{ { mod, 'V', '3'      }, { toggleview,     { tags[2] }                 } }, \
	{ { mod, 'V', '4'      }, { toggleview,     { tags[3] }                 } }, \
	{ { mod, 'V', '5'      }, { toggleview,     { tags[4] }                 } }, \
	{ { mod, 'T', '1'      }, { toggletag,      { tags[0] }                 } }, \
	{ { mod, 'T', '2'      }, { toggletag,      { tags[1] }                 } }, \
	{ { mod, 'T', '3'      }, { toggletag,      { tags[2] }                 } }, \
	{ { mod, 'T', '4'      }, { toggletag,      { tags[3] }                 } }, \
	{ { mod, 'T', '5'      }, { toggletag,      { tags[4] }                 } }, \
	{ { mod, CTRL('v'), '1'      }, { view,           { tags[0] }                 } }, \
	{ { mod, CTRL('v'), '2'      }, { view,           { tags[1] }                 } }, \
	{ { mod, CTRL('v'), '3'      }, { view,           { tags[2] }                 } }, \
	{ { mod, CTRL('v'), '4'      }, { view,           { tags[3] }                 } }, \
	{ { mod, CTRL('v'), '5'      }, { view,           { tags[4] }                 } }, \
	{ { mod, CTRL('v'), '\t',    }, { viewprevtag,    { NULL }                    } }, \
	{ { mod, CTRL('t'), '0'      }, { tag,            { NULL }                    } }, \
	{ { mod, CTRL('t'), '1'      }, { tag,            { tags[0] }                 } }, \
	{ { mod, CTRL('t'), '2'      }, { tag,            { tags[1] }                 } }, \
	{ { mod, CTRL('t'), '3'      }, { tag,            { tags[2] }                 } }, \
	{ { mod, CTRL('t'), '4'      }, { tag,            { tags[3] }                 } }, \
	{ { mod, CTRL('t'), '5'      }, { tag,            { tags[4] }                 } }, \
	{ { mod, CTRL('V'), '1'      }, { toggleview,     { tags[0] }                 } }, \
	{ { mod, CTRL('V'), '2'      }, { toggleview,     { tags[1] }                 } }, \
	{ { mod, CTRL('V'), '3'      }, { toggleview,     { tags[2] }                 } }, \
	{ { mod, CTRL('V'), '4'      }, { toggleview,     { tags[3] }                 } }, \
	{ { mod, CTRL('V'), '5'      }, { toggleview,     { tags[4] }                 } }, \
	{ { mod, CTRL('T'), '1'      }, { toggletag,      { tags[0] }                 } }, \
	{ { mod, CTRL('T'), '2'      }, { toggletag,      { tags[1] }                 } }, \
	{ { mod, CTRL('T'), '3'      }, { toggletag,      { tags[2] }                 } }, \
	{ { mod, CTRL('T'), '4'      }, { toggletag,      { tags[3] }                 } }, \
	{ { mod, CTRL('T'), '5'      }, { toggletag,      { tags[4] }                 } }, \


/* you can at most specifiy MAX_ARGS (3) number of arguments */
static KeyBinding bindings[] = {
    __bindings(CTRL('z'))
    __bindings(CTRL('g'))
};

static const ColorRule colorrules[] = {
	{ "", A_NORMAL, &colors[DEFAULT] }, /* default */
};

/* possible values for the mouse buttons are listed below:
 *
 * BUTTON1_PRESSED          mouse button 1 down
 * BUTTON1_RELEASED         mouse button 1 up
 * BUTTON1_CLICKED          mouse button 1 clicked
 * BUTTON1_DOUBLE_CLICKED   mouse button 1 double clicked
 * BUTTON1_TRIPLE_CLICKED   mouse button 1 triple clicked
 * BUTTON2_PRESSED          mouse button 2 down
 * BUTTON2_RELEASED         mouse button 2 up
 * BUTTON2_CLICKED          mouse button 2 clicked
 * BUTTON2_DOUBLE_CLICKED   mouse button 2 double clicked
 * BUTTON2_TRIPLE_CLICKED   mouse button 2 triple clicked
 * BUTTON3_PRESSED          mouse button 3 down
 * BUTTON3_RELEASED         mouse button 3 up
 * BUTTON3_CLICKED          mouse button 3 clicked
 * BUTTON3_DOUBLE_CLICKED   mouse button 3 double clicked
 * BUTTON3_TRIPLE_CLICKED   mouse button 3 triple clicked
 * BUTTON4_PRESSED          mouse button 4 down
 * BUTTON4_RELEASED         mouse button 4 up
 * BUTTON4_CLICKED          mouse button 4 clicked
 * BUTTON4_DOUBLE_CLICKED   mouse button 4 double clicked
 * BUTTON4_TRIPLE_CLICKED   mouse button 4 triple clicked
 * BUTTON_SHIFT             shift was down during button state change
 * BUTTON_CTRL              control was down during button state change
 * BUTTON_ALT               alt was down during button state change
 * ALL_MOUSE_EVENTS         report all button state changes
 * REPORT_MOUSE_POSITION    report mouse movement
 */

#ifdef NCURSES_MOUSE_VERSION
# define CONFIG_MOUSE /* compile in mouse support if we build against ncurses */
#endif

#define ENABLE_MOUSE true /* whether to enable mouse events by default */

#ifdef CONFIG_MOUSE
static Button buttons[] = {
	{ BUTTON1_CLICKED,        { mouse_focus,      { NULL  } } },
	{ BUTTON1_DOUBLE_CLICKED, { mouse_fullscreen, { "[ ]" } } },
	{ BUTTON2_CLICKED,        { mouse_zoom,       { NULL  } } },
	{ BUTTON3_CLICKED,        { mouse_minimize,   { NULL  } } },
};
#endif /* CONFIG_MOUSE */

static Cmd commands[] = {
	{ "create", { create,	{ NULL } } },
};

/* gets executed when dvtm is started */
static Action actions[] = {
	{ create, { NULL } },
};

static char const * const keytable[] = {
	/* add your custom key escape sequences */
};

/* editor to use for copy mode. If neither of DVTM_EDITOR, EDITOR and PAGER is
 * set the first entry is chosen. Otherwise the array is consulted for supported
 * options. A %d in argv is replaced by the line number at which the file should
 * be opened. If filter is true the editor is expected to work even if stdout is
 * redirected (i.e. not a terminal).
 */
static Editor editors[] = {
	{ .name = "vis",         .argv = { "vis", "+%d", "-", NULL  }, .filter = true  },
	{ .name = "sandy",       .argv = { "sandy", "-d", "-", NULL }, .filter = true  },
	{ .name = "dvtm-editor", .argv = { "dvtm-editor", "-", NULL }, .filter = true  },
	{ .name = "vim",         .argv = { "vim", "+%d", "-", NULL  }, .filter = false },
	{ .name = "less",        .argv = { "less", "+%d", NULL      }, .filter = false },
	{ .name = "more",        .argv = { "more", "+%d", NULL      }, .filter = false },
};
