/* See LICENSE for license details. */

/*

Module: bfst_key.c

Description:

    Translation of X11 key codes into terminal escape sequences.

*/

#include "bfst_os.h"

#include "bfst_body.h"

/*
 * Special keys (change & recompile st.info accordingly)
 *
 * Mask value:
 * * Use XK_ANY_MOD to match the key no matter modifiers state
 * * Use XK_NO_MOD to match the key alone (no modifiers)
 * appkey value:
 * * 0: no value
 * * > 0: keypad application mode enabled
 * *   = 2: term.numlock = 1
 * * < 0: keypad application mode disabled
 * appcursor value:
 * * 0: no value
 * * > 0: cursor application mode enabled
 * * < 0: cursor application mode disabled
 * crlf value
 * * 0: no value
 * * > 0: crlf mode is enabled
 * * < 0: crlf mode is disabled
 *
 * Be careful with the order of the definitions because st searches in
 * this table sequentially, so any XK_ANY_MOD must be in the last
 * position for a key.
 */

/*
 * If you want keys other than the X11 function keys (0xFD00 - 0xFFFF)
 * to be mapped below, add them to this array.
 */
static KeySym mappedkeys[] = { -1 };

typedef struct {
        KeySym k;
        unsigned int mask;
        char *s;
        /* three valued logic variables: 0 indifferent, 1 on, -1 off */
        signed char appkey;    /* application keypad */
        signed char appcursor; /* application cursor */
        signed char crlf;      /* crlf mode          */
} Key;

#define BFST_MOD_SAC (ShiftMask|Mod1Mask|ControlMask)
#define BFST_MOD_AC (Mod1Mask|ControlMask)
#define BFST_MOD_SC (ShiftMask|ControlMask)
#define BFST_MOD_C (ControlMask)
#define BFST_MOD_SA (ShiftMask|Mod1Mask)
#define BFST_MOD_A (Mod1Mask)
#define BFST_MOD_S (ShiftMask)
#define BFST_MOD_ANY (XK_ANY_MOD)

static Key key[] =
{
        /* keysym           mask            string      appkey appcursor crlf */
#if 0
        { XK_KP_Home,       ShiftMask,      "\033[2J",       0,   -1,    0},
        { XK_KP_Home,       ShiftMask,      "\033[1;2H",     0,   +1,    0},
        { XK_KP_Home,       XK_ANY_MOD,     "\033[H",        0,   -1,    0},
        { XK_KP_Home,       XK_ANY_MOD,     "\033[1~",       0,   +1,    0},
        { XK_KP_Up,         XK_ANY_MOD,     "\033Ox",       +1,    0,    0},
        { XK_KP_Up,         XK_ANY_MOD,     "\033[A",        0,   -1,    0},
        { XK_KP_Up,         XK_ANY_MOD,     "\033OA",        0,   +1,    0},
        { XK_KP_Down,       XK_ANY_MOD,     "\033Or",       +1,    0,    0},
        { XK_KP_Down,       XK_ANY_MOD,     "\033[B",        0,   -1,    0},
        { XK_KP_Down,       XK_ANY_MOD,     "\033OB",        0,   +1,    0},
        { XK_KP_Left,       XK_ANY_MOD,     "\033Ot",       +1,    0,    0},
        { XK_KP_Left,       XK_ANY_MOD,     "\033[D",        0,   -1,    0},
        { XK_KP_Left,       XK_ANY_MOD,     "\033OD",        0,   +1,    0},
        { XK_KP_Right,      XK_ANY_MOD,     "\033Ov",       +1,    0,    0},
        { XK_KP_Right,      XK_ANY_MOD,     "\033[C",        0,   -1,    0},
        { XK_KP_Right,      XK_ANY_MOD,     "\033OC",        0,   +1,    0},
        { XK_KP_Prior,      ShiftMask,      "\033[5;2~",     0,    0,    0},
        { XK_KP_Prior,      XK_ANY_MOD,     "\033[5~",       0,    0,    0},
        { XK_KP_Begin,      XK_ANY_MOD,     "\033[E",        0,    0,    0},
        { XK_KP_End,        ControlMask,    "\033[J",       -1,    0,    0},
        { XK_KP_End,        ControlMask,    "\033[1;5F",    +1,    0,    0},
        { XK_KP_End,        ShiftMask,      "\033[K",       -1,    0,    0},
        { XK_KP_End,        ShiftMask,      "\033[1;2F",    +1,    0,    0},
        { XK_KP_End,        XK_ANY_MOD,     "\033[F",        0,    0,    0},
        { XK_KP_Next,       ShiftMask,      "\033[6;2~",     0,    0,    0},
        { XK_KP_Next,       XK_ANY_MOD,     "\033[6~",       0,    0,    0},
        { XK_KP_Insert,     ShiftMask,      "\033[2;2~",    +1,    0,    0},
        { XK_KP_Insert,     ShiftMask,      "\033[4l",      -1,    0,    0},
        { XK_KP_Insert,     ControlMask,    "\033[L",       -1,    0,    0},
        { XK_KP_Insert,     ControlMask,    "\033[2;5~",    +1,    0,    0},
        { XK_KP_Insert,     XK_ANY_MOD,     "\033[2~",      -1,    0,    0},
        { XK_KP_Insert,     XK_ANY_MOD,     "\033[2~",      +1,    0,    0},
        { XK_KP_Delete,     ControlMask,    "\033[M",       -1,    0,    0},
        { XK_KP_Delete,     ControlMask,    "\033[3;5~",    +1,    0,    0},
        { XK_KP_Delete,     ShiftMask,      "\033[2K",      -1,    0,    0},
        { XK_KP_Delete,     ShiftMask,      "\033[3;2~",    +1,    0,    0},
        { XK_KP_Delete,     XK_ANY_MOD,     "\033[3~",      -1,    0,    0},
        { XK_KP_Delete,     XK_ANY_MOD,     "\033[3~",      +1,    0,    0},
        { XK_KP_Multiply,   XK_ANY_MOD,     "\033Oj",       +2,    0,    0},
        { XK_KP_Add,        XK_ANY_MOD,     "\033Ok",       +2,    0,    0},
        { XK_KP_Enter,      XK_ANY_MOD,     "\033OM",       +2,    0,    0},
        { XK_KP_Enter,      XK_ANY_MOD,     "\r",           -1,    0,   -1},
        { XK_KP_Enter,      XK_ANY_MOD,     "\r\n",         -1,    0,   +1},
        { XK_KP_Subtract,   XK_ANY_MOD,     "\033Om",       +2,    0,    0},
        { XK_KP_Decimal,    XK_ANY_MOD,     "\033On",       +2,    0,    0},
        { XK_KP_Divide,     XK_ANY_MOD,     "\033Oo",       +2,    0,    0},
        { XK_KP_0,          XK_ANY_MOD,     "\033Op",       +2,    0,    0},
        { XK_KP_1,          XK_ANY_MOD,     "\033Oq",       +2,    0,    0},
        { XK_KP_2,          XK_ANY_MOD,     "\033Or",       +2,    0,    0},
        { XK_KP_3,          XK_ANY_MOD,     "\033Os",       +2,    0,    0},
        { XK_KP_4,          XK_ANY_MOD,     "\033Ot",       +2,    0,    0},
        { XK_KP_5,          XK_ANY_MOD,     "\033Ou",       +2,    0,    0},
        { XK_KP_6,          XK_ANY_MOD,     "\033Ov",       +2,    0,    0},
        { XK_KP_7,          XK_ANY_MOD,     "\033Ow",       +2,    0,    0},
        { XK_KP_8,          XK_ANY_MOD,     "\033Ox",       +2,    0,    0},
        { XK_KP_9,          XK_ANY_MOD,     "\033Oy",       +2,    0,    0},
#endif

        { XK_Up,            BFST_MOD_SAC,   "\033[1;8A",     0,    0,    0},
        { XK_Up,            BFST_MOD_AC,    "\033[1;7A",     0,    0,    0},
        { XK_Up,            BFST_MOD_SC,    "\033[1;6A",     0,    0,    0},
        { XK_Up,            BFST_MOD_C,     "\033[1;5A",     0,    0,    0},
        { XK_Up,            BFST_MOD_SA,    "\033[1;4A",     0,    0,    0},
        { XK_Up,            BFST_MOD_A,     "\033[1;3A",     0,    0,    0},
        { XK_Up,            BFST_MOD_S,     "\033[1;2A",     0,    0,    0},
        { XK_Up,            BFST_MOD_ANY,   "\033[A",        0,   -1,    0},
        { XK_Up,            BFST_MOD_ANY,   "\033OA",        0,   +1,    0},
        { XK_Down,          BFST_MOD_SAC,   "\033[1;8B",     0,    0,    0},
        { XK_Down,          BFST_MOD_AC,    "\033[1;7B",     0,    0,    0},
        { XK_Down,          BFST_MOD_SC,    "\033[1;6B",     0,    0,    0},
        { XK_Down,          BFST_MOD_C,     "\033[1;5B",     0,    0,    0},
        { XK_Down,          BFST_MOD_SA,    "\033[1;4B",     0,    0,    0},
        { XK_Down,          BFST_MOD_A,     "\033[1;3B",     0,    0,    0},
        { XK_Down,          BFST_MOD_S,     "\033[1;2B",     0,    0,    0},
        { XK_Down,          BFST_MOD_ANY,   "\033[B",        0,   -1,    0},
        { XK_Down,          BFST_MOD_ANY,   "\033OB",        0,   +1,    0},
        { XK_Left,          BFST_MOD_SAC,   "\033[1;8D",     0,    0,    0},
        { XK_Left,          BFST_MOD_AC,    "\033[1;7D",     0,    0,    0},
        { XK_Left,          BFST_MOD_SC,    "\033[1;6D",     0,    0,    0},
        { XK_Left,          BFST_MOD_C,     "\033[1;5D",     0,    0,    0},
        { XK_Left,          BFST_MOD_SA,    "\033[1;4D",     0,    0,    0},
        { XK_Left,          BFST_MOD_A,     "\033[1;3D",     0,    0,    0},
        { XK_Left,          BFST_MOD_S,     "\033[1;2D",     0,    0,    0},
        { XK_Left,          BFST_MOD_ANY,   "\033[D",        0,   -1,    0},
        { XK_Left,          BFST_MOD_ANY,   "\033OD",        0,   +1,    0},
        { XK_Right,         BFST_MOD_SAC,   "\033[1;8C",     0,    0,    0},
        { XK_Right,         BFST_MOD_AC,    "\033[1;7C",     0,    0,    0},
        { XK_Right,         BFST_MOD_SC,    "\033[1;6C",     0,    0,    0},
        { XK_Right,         BFST_MOD_C,     "\033[1;5C",     0,    0,    0},
        { XK_Right,         BFST_MOD_SA,    "\033[1;4C",     0,    0,    0},
        { XK_Right,         BFST_MOD_A,     "\033[1;3C",     0,    0,    0},
        { XK_Right,         BFST_MOD_S,     "\033[1;2C",     0,    0,    0},
        { XK_Right,         BFST_MOD_ANY,   "\033[C",        0,   -1,    0},
        { XK_Right,         BFST_MOD_ANY,   "\033OC",        0,   +1,    0},
        { XK_ISO_Left_Tab,  BFST_MOD_S,     "\033[Z",        0,    0,    0},
        { XK_Return,        BFST_MOD_A,     "\033\r",        0,    0,   -1},
        { XK_Return,        BFST_MOD_A,     "\033\r\n",      0,    0,   +1},
        { XK_Return,        BFST_MOD_ANY,   "\r",            0,    0,   -1},
        { XK_Return,        BFST_MOD_ANY,   "\r\n",          0,    0,   +1},
        { XK_Insert,        BFST_MOD_SAC,   "\033[2;8~",     0,    0,    0},
        { XK_Insert,        BFST_MOD_AC,    "\033[2;7~",     0,    0,    0},
        { XK_Insert,        BFST_MOD_SC,    "\033[2;6~",     0,    0,    0},
        { XK_Insert,        BFST_MOD_C,     "\033[2;5~",     0,    0,    0},
        { XK_Insert,        BFST_MOD_SA,    "\033[2;4~",     0,    0,    0},
        { XK_Insert,        BFST_MOD_A,     "\033[2;3~",     0,    0,    0},
        { XK_Insert,        BFST_MOD_S,     "\033[2;2~",     0,    0,    0},
        { XK_Insert,        BFST_MOD_ANY,   "\033[2~",       0,    0,    0},
        { XK_Delete,        BFST_MOD_SAC,   "\033[3;8~",     0,    0,    0},
        { XK_Delete,        BFST_MOD_AC,    "\033[3;7~",     0,    0,    0},
        { XK_Delete,        BFST_MOD_SC,    "\033[3;6~",     0,    0,    0},
        { XK_Delete,        BFST_MOD_C,     "\033[3;5~",     0,    0,    0},
        { XK_Delete,        BFST_MOD_SA,    "\033[3;4~",     0,    0,    0},
        { XK_Delete,        BFST_MOD_A,     "\033[3;3~",     0,    0,    0},
        { XK_Delete,        BFST_MOD_S,     "\033[3;2~",     0,    0,    0},
        { XK_Delete,        BFST_MOD_ANY,   "\033[3~",       0,    0,    0},
        { XK_BackSpace,     XK_NO_MOD,      "\177",          0,    0,    0},
        { XK_Home,          BFST_MOD_S,     "\033[2J",       0,   +1,    0},
        { XK_Home,          BFST_MOD_SAC,   "\033[1;8H",     0,    0,    0},
        { XK_Home,          BFST_MOD_AC,    "\033[1;7H",     0,    0,    0},
        { XK_Home,          BFST_MOD_SC,    "\033[1;6H",     0,    0,    0},
        { XK_Home,          BFST_MOD_C,     "\033[1;5H",     0,    0,    0},
        { XK_Home,          BFST_MOD_SA,    "\033[1;4H",     0,    0,    0},
        { XK_Home,          BFST_MOD_A,     "\033[1;3H",     0,    0,    0},
        { XK_Home,          BFST_MOD_S,     "\033[1;2H",     0,    0,    0},
        { XK_Home,          BFST_MOD_ANY,   "\033[H",        0,   -1,    0},
        { XK_Home,          BFST_MOD_ANY,   "\033[1~",       0,   +1,    0},
        { XK_End,           BFST_MOD_SAC,   "\033[1;8F",     0,    0,    0},
        { XK_End,           BFST_MOD_AC,    "\033[1;7F",     0,    0,    0},
        { XK_End,           BFST_MOD_SC,    "\033[1;6F",     0,    0,    0},
        { XK_End,           BFST_MOD_C,     "\033[1;5F",     0,    0,    0},
        { XK_End,           BFST_MOD_SA,    "\033[1;4F",     0,    0,    0},
        { XK_End,           BFST_MOD_A,     "\033[1;3F",     0,    0,    0},
        { XK_End,           BFST_MOD_S,     "\033[1;2F",     0,    0,    0},
        { XK_End,           BFST_MOD_ANY,   "\033[F",        0,   -1,    0},
        { XK_End,           BFST_MOD_ANY,   "\033OF",        0,   +1,    0},
        { XK_Prior,         BFST_MOD_SAC,   "\033[5;8~",     0,    0,    0},
        { XK_Prior,         BFST_MOD_AC,    "\033[5;7~",     0,    0,    0},
        { XK_Prior,         BFST_MOD_SC,    "\033[5;6~",     0,    0,    0},
        { XK_Prior,         BFST_MOD_C,     "\033[5;5~",     0,    0,    0},
        { XK_Prior,         BFST_MOD_SA,    "\033[5;4~",     0,    0,    0},
        { XK_Prior,         BFST_MOD_A,     "\033[5;3~",     0,    0,    0},
        { XK_Prior,         BFST_MOD_S,     "\033[5;2~",     0,    0,    0},
        { XK_Prior,         BFST_MOD_ANY,   "\033[5~",       0,    0,    0},
        { XK_Next,          BFST_MOD_SAC,   "\033[6;8~",     0,    0,    0},
        { XK_Next,          BFST_MOD_AC,    "\033[6;7~",     0,    0,    0},
        { XK_Next,          BFST_MOD_SC,    "\033[6;6~",     0,    0,    0},
        { XK_Next,          BFST_MOD_C,     "\033[6;5~",     0,    0,    0},
        { XK_Next,          BFST_MOD_SA,    "\033[6;4~",     0,    0,    0},
        { XK_Next,          BFST_MOD_A,     "\033[6;3~",     0,    0,    0},
        { XK_Next,          BFST_MOD_S,     "\033[6;2~",     0,    0,    0},
        { XK_Next,          BFST_MOD_ANY,   "\033[6~",       0,    0,    0},
        { XK_F1,            BFST_MOD_SAC,   "\033[1;8P",     0,    0,    0},
        { XK_F1,            BFST_MOD_AC,    "\033[1;7P",     0,    0,    0},
        { XK_F1,            BFST_MOD_SC,    "\033[1;6P",     0,    0,    0},
        { XK_F1,            BFST_MOD_C,     "\033[1;5P",     0,    0,    0},
        { XK_F1,            BFST_MOD_SA,    "\033[1;4P",     0,    0,    0},
        { XK_F1,            BFST_MOD_A,     "\033[1;3P",     0,    0,    0},
        { XK_F1,            BFST_MOD_S,     "\033[1;2P",     0,    0,    0},
        { XK_F1,            BFST_MOD_ANY,   "\033OP" ,       0,    0,    0},
        { XK_F2,            BFST_MOD_SAC,   "\033[1;8Q",     0,    0,    0},
        { XK_F2,            BFST_MOD_AC,    "\033[1;7Q",     0,    0,    0},
        { XK_F2,            BFST_MOD_SC,    "\033[1;6Q",     0,    0,    0},
        { XK_F2,            BFST_MOD_C,     "\033[1;5Q",     0,    0,    0},
        { XK_F2,            BFST_MOD_SA,    "\033[1;4Q",     0,    0,    0},
        { XK_F2,            BFST_MOD_A,     "\033[1;3Q",     0,    0,    0},
        { XK_F2,            BFST_MOD_S,     "\033[1;2Q",     0,    0,    0},
        { XK_F2,            BFST_MOD_ANY,   "\033OQ" ,       0,    0,    0},
        { XK_F3,            BFST_MOD_SAC,   "\033[1;8R",     0,    0,    0},
        { XK_F3,            BFST_MOD_AC,    "\033[1;7R",     0,    0,    0},
        { XK_F3,            BFST_MOD_SC,    "\033[1;6R",     0,    0,    0},
        { XK_F3,            BFST_MOD_C,     "\033[1;5R",     0,    0,    0},
        { XK_F3,            BFST_MOD_SA,    "\033[1;4R",     0,    0,    0},
        { XK_F3,            BFST_MOD_A,     "\033[1;3R",     0,    0,    0},
        { XK_F3,            BFST_MOD_S,     "\033[1;2R",     0,    0,    0},
        { XK_F3,            BFST_MOD_ANY,   "\033OR" ,       0,    0,    0},
        { XK_F4,            BFST_MOD_SAC,   "\033[1;8S",     0,    0,    0},
        { XK_F4,            BFST_MOD_AC,    "\033[1;7S",     0,    0,    0},
        { XK_F4,            BFST_MOD_SC,    "\033[1;6S",     0,    0,    0},
        { XK_F4,            BFST_MOD_C,     "\033[1;5S",     0,    0,    0},
        { XK_F4,            BFST_MOD_SA,    "\033[1;4S",     0,    0,    0},
        { XK_F4,            BFST_MOD_A,     "\033[1;3S",     0,    0,    0},
        { XK_F4,            BFST_MOD_S,     "\033[1;2S",     0,    0,    0},
        { XK_F4,            BFST_MOD_ANY,   "\033OS" ,       0,    0,    0},
        { XK_F5,            BFST_MOD_SAC,   "\033[15;8~",    0,    0,    0},
        { XK_F5,            BFST_MOD_AC,    "\033[15;7~",    0,    0,    0},
        { XK_F5,            BFST_MOD_SC,    "\033[15;6~",    0,    0,    0},
        { XK_F5,            BFST_MOD_C,     "\033[15;5~",    0,    0,    0},
        { XK_F5,            BFST_MOD_SA,    "\033[15;4~",    0,    0,    0},
        { XK_F5,            BFST_MOD_A,     "\033[15;3~",    0,    0,    0},
        { XK_F5,            BFST_MOD_S,     "\033[15;2~",    0,    0,    0},
        { XK_F5,            BFST_MOD_ANY,   "\033[15~" ,     0,    0,    0},
        { XK_F6,            BFST_MOD_SAC,   "\033[17;8~",    0,    0,    0},
        { XK_F6,            BFST_MOD_AC,    "\033[17;7~",    0,    0,    0},
        { XK_F6,            BFST_MOD_SC,    "\033[17;6~",    0,    0,    0},
        { XK_F6,            BFST_MOD_C,     "\033[17;5~",    0,    0,    0},
        { XK_F6,            BFST_MOD_SA,    "\033[17;4~",    0,    0,    0},
        { XK_F6,            BFST_MOD_A,     "\033[17;3~",    0,    0,    0},
        { XK_F6,            BFST_MOD_S,     "\033[17;2~",    0,    0,    0},
        { XK_F6,            BFST_MOD_ANY,   "\033[17~" ,     0,    0,    0},
        { XK_F7,            BFST_MOD_SAC,   "\033[18;8~",    0,    0,    0},
        { XK_F7,            BFST_MOD_AC,    "\033[18;7~",    0,    0,    0},
        { XK_F7,            BFST_MOD_SC,    "\033[18;6~",    0,    0,    0},
        { XK_F7,            BFST_MOD_C,     "\033[18;5~",    0,    0,    0},
        { XK_F7,            BFST_MOD_SA,    "\033[18;4~",    0,    0,    0},
        { XK_F7,            BFST_MOD_A,     "\033[18;3~",    0,    0,    0},
        { XK_F7,            BFST_MOD_S,     "\033[18;2~",    0,    0,    0},
        { XK_F7,            BFST_MOD_ANY,   "\033[18~" ,     0,    0,    0},
        { XK_F8,            BFST_MOD_SAC,   "\033[19;8~",    0,    0,    0},
        { XK_F8,            BFST_MOD_AC,    "\033[19;7~",    0,    0,    0},
        { XK_F8,            BFST_MOD_SC,    "\033[19;6~",    0,    0,    0},
        { XK_F8,            BFST_MOD_C,     "\033[19;5~",    0,    0,    0},
        { XK_F8,            BFST_MOD_SA,    "\033[19;4~",    0,    0,    0},
        { XK_F8,            BFST_MOD_A,     "\033[19;3~",    0,    0,    0},
        { XK_F8,            BFST_MOD_S,     "\033[19;2~",    0,    0,    0},
        { XK_F8,            BFST_MOD_ANY,   "\033[19~" ,     0,    0,    0},
        { XK_F9,            BFST_MOD_SAC,   "\033[20;8~",    0,    0,    0},
        { XK_F9,            BFST_MOD_AC,    "\033[20;7~",    0,    0,    0},
        { XK_F9,            BFST_MOD_SC,    "\033[20;6~",    0,    0,    0},
        { XK_F9,            BFST_MOD_C,     "\033[20;5~",    0,    0,    0},
        { XK_F9,            BFST_MOD_SA,    "\033[20;4~",    0,    0,    0},
        { XK_F9,            BFST_MOD_A,     "\033[20;3~",    0,    0,    0},
        { XK_F9,            BFST_MOD_S,     "\033[20;2~",    0,    0,    0},
        { XK_F9,            BFST_MOD_ANY,   "\033[20~" ,     0,    0,    0},
        { XK_F10,           BFST_MOD_SAC,   "\033[21;8~",    0,    0,    0},
        { XK_F10,           BFST_MOD_AC,    "\033[21;7~",    0,    0,    0},
        { XK_F10,           BFST_MOD_SC,    "\033[21;6~",    0,    0,    0},
        { XK_F10,           BFST_MOD_C,     "\033[21;5~",    0,    0,    0},
        { XK_F10,           BFST_MOD_SA,    "\033[21;4~",    0,    0,    0},
        { XK_F10,           BFST_MOD_A,     "\033[21;3~",    0,    0,    0},
        { XK_F10,           BFST_MOD_S,     "\033[21;2~",    0,    0,    0},
        { XK_F10,           BFST_MOD_ANY,   "\033[21~" ,     0,    0,    0},
        { XK_F11,           BFST_MOD_SAC,   "\033[23;8~",    0,    0,    0},
        { XK_F11,           BFST_MOD_AC,    "\033[23;7~",    0,    0,    0},
        { XK_F11,           BFST_MOD_SC,    "\033[23;6~",    0,    0,    0},
        { XK_F11,           BFST_MOD_C,     "\033[23;5~",    0,    0,    0},
        { XK_F11,           BFST_MOD_SA,    "\033[23;4~",    0,    0,    0},
        { XK_F11,           BFST_MOD_A,     "\033[23;3~",    0,    0,    0},
        { XK_F11,           BFST_MOD_S,     "\033[23;2~",    0,    0,    0},
        { XK_F11,           BFST_MOD_ANY,   "\033[23~" ,     0,    0,    0},
        { XK_F12,           BFST_MOD_SAC,   "\033[24;8~",    0,    0,    0},
        { XK_F12,           BFST_MOD_AC,    "\033[24;7~",    0,    0,    0},
        { XK_F12,           BFST_MOD_SC,    "\033[24;6~",    0,    0,    0},
        { XK_F12,           BFST_MOD_C,     "\033[24;5~",    0,    0,    0},
        { XK_F12,           BFST_MOD_SA,    "\033[24;4~",    0,    0,    0},
        { XK_F12,           BFST_MOD_A,     "\033[24;3~",    0,    0,    0},
        { XK_F12,           BFST_MOD_S,     "\033[24;2~",    0,    0,    0},
        { XK_F12,           BFST_MOD_ANY,   "\033[24~" ,     0,    0,    0},

#if 0
        { XK_F13,           XK_NO_MOD,      "\033[1;2P",     0,    0,    0},
        { XK_F14,           XK_NO_MOD,      "\033[1;2Q",     0,    0,    0},
        { XK_F15,           XK_NO_MOD,      "\033[1;2R",     0,    0,    0},
        { XK_F16,           XK_NO_MOD,      "\033[1;2S",     0,    0,    0},
        { XK_F17,           XK_NO_MOD,      "\033[15;2~",    0,    0,    0},
        { XK_F18,           XK_NO_MOD,      "\033[17;2~",    0,    0,    0},
        { XK_F19,           XK_NO_MOD,      "\033[18;2~",    0,    0,    0},
        { XK_F20,           XK_NO_MOD,      "\033[19;2~",    0,    0,    0},
        { XK_F21,           XK_NO_MOD,      "\033[20;2~",    0,    0,    0},
        { XK_F22,           XK_NO_MOD,      "\033[21;2~",    0,    0,    0},
        { XK_F23,           XK_NO_MOD,      "\033[23;2~",    0,    0,    0},
        { XK_F24,           XK_NO_MOD,      "\033[24;2~",    0,    0,    0},
        { XK_F25,           XK_NO_MOD,      "\033[1;5P",     0,    0,    0},
        { XK_F26,           XK_NO_MOD,      "\033[1;5Q",     0,    0,    0},
        { XK_F27,           XK_NO_MOD,      "\033[1;5R",     0,    0,    0},
        { XK_F28,           XK_NO_MOD,      "\033[1;5S",     0,    0,    0},
        { XK_F29,           XK_NO_MOD,      "\033[15;5~",    0,    0,    0},
        { XK_F30,           XK_NO_MOD,      "\033[17;5~",    0,    0,    0},
        { XK_F31,           XK_NO_MOD,      "\033[18;5~",    0,    0,    0},
        { XK_F32,           XK_NO_MOD,      "\033[19;5~",    0,    0,    0},
        { XK_F33,           XK_NO_MOD,      "\033[20;5~",    0,    0,    0},
        { XK_F34,           XK_NO_MOD,      "\033[21;5~",    0,    0,    0},
        { XK_F35,           XK_NO_MOD,      "\033[23;5~",    0,    0,    0},
#endif
};

static char i_body_numlock_flag = 1;

/*
 * State bits to ignore when matching key or button events.  By default,
 * numlock (Mod2Mask) and keyboard layout (XK_SWITCH_MOD) are ignored.
 */
static unsigned int const ignoremod = Mod2Mask|XK_SWITCH_MOD;

static
char
match(unsigned int mask, unsigned int state)
{
    return mask == XK_ANY_MOD || mask == (state & ~ignoremod);
}

/*

Function: bfst_key_lookup

Description:

    Lookup the given X11 key code and return corresponding terminal string.

*/
char const *
bfst_key_lookup(
    struct bfst_tty const * const p_term,
    KeySym const k,
    unsigned int const state)
{
    Key *kp;
    unsigned int i;

    /* Check for mapped keys out of X11 function keys. */
    for (i = 0; i < BFST_TOOLS_LEN(mappedkeys); i++)
    {
        if (mappedkeys[i] == k)
        {
            break;
        }
    }

    if(i == BFST_TOOLS_LEN(mappedkeys))
    {
        if((k & 0xFFFF) < 0xFD00)
            return NULL;
    }

    for(kp = key; kp < key + BFST_TOOLS_LEN(key); kp++)
    {
        if(kp->k != k)
            continue;

        if(!match(kp->mask, state))
            continue;

        if(IS_SET(p_term, MODE_APPKEYPAD) ? kp->appkey < 0 : kp->appkey > 0)
            continue;

        if(i_body_numlock_flag && kp->appkey == 2)
            continue;

        if(IS_SET(p_term, MODE_APPCURSOR) ? kp->appcursor < 0 : kp->appcursor > 0)
            continue;

        if(IS_SET(p_term, MODE_CRLF) ? kp->crlf < 0 : kp->crlf > 0)
            continue;

        return kp->s;
    }

    return NULL;

} /* bfst_key_lookup() */

void bfst_key_toggle_numlock_flag(void)
{
    i_body_numlock_flag ^= 1;

} /* bfst_key_toggle_numlock_flag() */

/* end-of-file: bfst_key.c */
