/* See LICENSE for license details. */

/*

Module: bfst_tty.h

*/

/* Arbitrary sizes */
#define ESC_BUF_SIZ   (128*BFST_UTF8_SIZE)
#define ESC_ARG_SIZ   16
#define STR_BUF_SIZ   ESC_BUF_SIZ
#define STR_ARG_SIZ   ESC_ARG_SIZ

/* macros */
#define IS_SET(p_term, flag) (((p_term)->mode & (flag)) != 0)
#define TIMEDIFF(t1, t2) ((t1.tv_sec-t2.tv_sec)*1000 + (t1.tv_nsec-t2.tv_nsec)/1E6)
#define MODBIT(x, set, bit) ((set) ? ((x) |= (bit)) : ((x) &= ~(bit)))

enum glyph_attribute {
        ATTR_NULL       = 0,
        ATTR_BOLD       = 1 << 0,
        ATTR_FAINT      = 1 << 1,
        ATTR_ITALIC     = 1 << 2,
        ATTR_UNDERLINE  = 1 << 3,
        ATTR_BLINK      = 1 << 4,
        ATTR_REVERSE    = 1 << 5,
        ATTR_INVISIBLE  = 1 << 6,
        ATTR_STRUCK     = 1 << 7,
        ATTR_WRAP       = 1 << 8,
        ATTR_WIDE       = 1 << 9,
        ATTR_WDUMMY     = 1 << 10,
        ATTR_CURSOR     = 1 << 11,
        ATTR_BOLD_FAINT = ATTR_BOLD | ATTR_FAINT,
};

enum term_mode {
        MODE_WRAP        = 1 << 0,
        MODE_INSERT      = 1 << 1,
        MODE_APPKEYPAD   = 1 << 2,
        MODE_ALTSCREEN   = 1 << 3,
        MODE_CRLF        = 1 << 4,
        MODE_MOUSEBTN    = 1 << 5,
        MODE_MOUSEMOTION = 1 << 6,
        MODE_REVERSE     = 1 << 7,
        MODE_KBDLOCK     = 1 << 8,
        MODE_HIDE        = 1 << 9,
        MODE_ECHO        = 1 << 10,
        MODE_APPCURSOR   = 1 << 11,
        MODE_MOUSESGR    = 1 << 12,
        MODE_8BIT        = 1 << 13,
        MODE_BLINK       = 1 << 14,
        MODE_FBLINK      = 1 << 15,
        MODE_FOCUS       = 1 << 16,
        MODE_MOUSEX10    = 1 << 17,
        MODE_MOUSEMANY   = 1 << 18,
        MODE_BRCKTPASTE  = 1 << 19,
        MODE_PRINT       = 1 << 20,
        MODE_MOUSE       = MODE_MOUSEBTN|MODE_MOUSEMOTION|MODE_MOUSEX10\
                          |MODE_MOUSEMANY,
};

enum cursor_movement {
        CURSOR_SAVE,
        CURSOR_LOAD
};

enum cursor_state {
        CURSOR_DEFAULT  = 0,
        CURSOR_WRAPNEXT = 1,
        CURSOR_ORIGIN   = 2
};

enum charset {
        CS_GRAPHIC0,
        CS_GRAPHIC1,
        CS_UK,
        CS_USA,
        CS_MULTI,
        CS_GER,
        CS_FIN
};

enum escape_state {
        ESC_START      = 1,
        ESC_CSI        = 2,
        ESC_STR        = 4,  /* DCS, OSC, PM, APC */
        ESC_ALTCHARSET = 8,
        ESC_STR_END    = 16, /* a final string was encountered */
        ESC_TEST       = 32, /* Enter in test mode */
};

enum glyph_flags
{
    GF_TOP_LEFT = 1,
    GF_TOP = 2,
    GF_TOP_RIGHT = 4,
    GF_LEFT = 8,
    GF_RIGHT = 16,
    GF_BOTTOM_LEFT = 32,
    GF_BOTTOM = 64,
    GF_BOTTOM_RIGHT = 128
};

struct bfst_glyph
{
        unsigned short int u;           /* character code */
        unsigned char fg;      /* foreground  */
        unsigned char bg;      /* background  */
        unsigned short int mode;      /* attribute flags */
        unsigned short int flags;
};

/* typedef struct bfst_glyph *Line; */

typedef struct {
        struct bfst_glyph attr; /* current char attributes */
        int x;
        int y;
        char state;
        unsigned int shape;
} TCursor;

/* CSI Escape sequence structs */
/* ESC '[' [[ [<priv>] <arg> [;]] <mode> [<mode>]] */
typedef struct {
        char buf[ESC_BUF_SIZ]; /* raw string */
        int len;               /* raw string length */
        char priv;
        int arg[ESC_ARG_SIZ];
        int narg;              /* nb of args */
        char mode[2];
} CSIEscape;

/* STR Escape sequence structs */
/* ESC type [[ [<priv>] <arg> [;]] <mode>] ESC '\' */
typedef struct {
        char type;             /* ESC type ... */
        char buf[STR_BUF_SIZ]; /* raw string */
        int len;               /* raw string length */
        char *args[STR_ARG_SIZ];
        int narg;              /* nb of args */
} STREscape;

struct bfst_line
{
    /* 64-bit unique id for line. */
    /* This helps detect view changes and switch to alt. */
    unsigned long long int i_unique;

    /* Pointer to array of glyph objects */
    struct bfst_glyph * a_glyph;

    /* Current length of the line (other characters are blank until end of the screen) */
    /* unsigned short int i_count; */

    /* Maximum length allocated for the line, make sure count never exceeds this. */
    /* unsigned short int i_max_count; */

    /* Number of characters with a blink attribute set */
    /* unsigned short int i_blink; */

    /* Number of characters with a special attribute or color set (different than first character of line). */
    /* unsigned short int i_special; */

};

struct bfst_line_array
{
    /* Pointer to array of line objects */
    struct bfst_line * a_line;

    /* Current number of lines (other lines are blank lines) */
    /* unsigned short int i_count; */

    /* unsigned short int i_max_count; */
};

/* Internal representation of the screen */
typedef struct bfst_tty
{
    struct bfst_tty_ctxt o_term_ctxt;

    int row;      /* nb row */
    int col;      /* nb col */
    struct bfst_line_array * p_cur;
    struct bfst_line_array * p_other;
    struct bfst_line_array o_pri;
    struct bfst_line_array o_alt;
    TCursor c;    /* cursor */
    TCursor c_save[2];
    int top;      /* top    scroll limit */
    int bot;      /* bottom scroll limit */
    int mode;     /* terminal mode flags */
    int esc;      /* escape state flags */
    char trantbl[4]; /* charset table translation */
    int charset;  /* current charset */
    int icharset; /* selected charset for sequence */
    char *tabs;
    CSIEscape csiescseq;
    STREscape strescseq;
    struct bfst_child child;
    struct bfst_sel sel;
    struct bfst_log log;

} Term;

/* Create a bfst_tty object */
struct bfst_tty * bfst_tty_create(
    struct bfst_view_ctxt const * const p_view_ctxt);

/* Release all resources allocated for bfst_tty module. */
void bfst_tty_destroy(
    struct bfst_tty_ctxt const * const p_term_ctxt);

void tresize(Term* p_term, int, int);

void techostr(Term* p_term, char const * s, unsigned long int n);

int tputstr(Term* p_term, char * buf, unsigned long int buflen);

/* end-of-file: bfst_tty.h */
