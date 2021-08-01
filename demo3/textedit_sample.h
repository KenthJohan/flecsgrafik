// I haven't actually tested this yet, this is just to make sure it compiles
/*
https://github.com/tf478/chord-recogniser-public/blob/master/IGraphics/Controls/ITextEntryControl.cpp
https://github.com/kritzikratzi/ofxMightyUI/blob/master/src/MuiTextArea.cpp
https://github.com/styluslabs/ugui/blob/master/textedit.cpp
https://github.com/WB-OPENMCU/awtk/blob/master/src/base/text_edit.c
https://github.com/tommitytom/RetroPlug/blob/master/thirdparty/iPlug2/IGraphics/Controls/ITextEntryControl.cpp
https://github.com/chinachuli/awtk-imxrt1052-raw/blob/master/middleware/zlg_awtk/awtk/src/base/text_edit.c
*/

#include <stdlib.h>
#include <string.h> // memmove
#include <ctype.h>  // isspace
#include "csc/csc_xlog.h"

#define STB_TEXTEDIT_CHARTYPE   char
#define STB_TEXTEDIT_STRING     text_control

// get the base type
#include "stb_textedit.h"

// define our editor structure
typedef struct
{
   char *string;
   int stringlen;
   STB_TexteditState state;
} text_control;

// define the functions we need
void layout_func(StbTexteditRow *row, STB_TEXTEDIT_STRING *str, int start_i)
{
	XLOG(XLOG_INF, XLOG_GENERAL, "%i", start_i);
	int remaining_chars = str->stringlen - start_i;
	row->num_chars = remaining_chars > 20 ? 20 : remaining_chars; // should do real word wrap here
	row->x0 = 0;
	row->x1 = 20; // need to account for actual size of characters
	row->baseline_y_delta = 1.25;
	row->ymin = -1;
	row->ymax =  1;
}

int delete_chars(STB_TEXTEDIT_STRING *str, int pos, int num)
{
   memmove(&str->string[pos], &str->string[pos+num], str->stringlen - (pos+num));
   str->stringlen -= num;
   return 1; // always succeeds
}

int insert_chars(STB_TEXTEDIT_STRING *str, int pos, STB_TEXTEDIT_CHARTYPE *newtext, int num)
{
	str->string = realloc(str->string, str->stringlen + num);
	memmove(&str->string[pos+num], &str->string[pos], str->stringlen - pos);
	memcpy(&str->string[pos], newtext, num);
	str->stringlen += num;
	XLOG(XLOG_INF, XLOG_GENERAL, "Insert chars %s %i %s", newtext, num, str->string);
	return 1; // always succeeds
}

// define all the #defines needed 

#define KEYDOWN_BIT                    0x80000000

#define STB_TEXTEDIT_STRINGLEN(tc)     ((tc)->stringlen)
#define STB_TEXTEDIT_LAYOUTROW         layout_func
#define STB_TEXTEDIT_GETWIDTH(tc,n,i)  (1) // quick hack for monospaced
#define STB_TEXTEDIT_KEYTOTEXT(key)    (((key) & KEYDOWN_BIT) ? 0 : (key))
#define STB_TEXTEDIT_GETCHAR(tc,i)     ((tc)->string[i])
#define STB_TEXTEDIT_NEWLINE           '\n'
#define STB_TEXTEDIT_IS_SPACE(ch)      isspace(ch)
#define STB_TEXTEDIT_DELETECHARS       delete_chars
#define STB_TEXTEDIT_INSERTCHARS       insert_chars

#define STB_TEXTEDIT_K_SHIFT           0x40000000
#define STB_TEXTEDIT_K_CONTROL         0x20000000
#define STB_TEXTEDIT_K_LEFT            (KEYDOWN_BIT | 1) // actually use VK_LEFT, SDLK_LEFT, etc
#define STB_TEXTEDIT_K_RIGHT           (KEYDOWN_BIT | 2) // VK_RIGHT
#define STB_TEXTEDIT_K_UP              (KEYDOWN_BIT | 3) // VK_UP
#define STB_TEXTEDIT_K_DOWN            (KEYDOWN_BIT | 4) // VK_DOWN
#define STB_TEXTEDIT_K_LINESTART       (KEYDOWN_BIT | 5) // VK_HOME
#define STB_TEXTEDIT_K_LINEEND         (KEYDOWN_BIT | 6) // VK_END
#define STB_TEXTEDIT_K_TEXTSTART       (STB_TEXTEDIT_K_LINESTART | STB_TEXTEDIT_K_CONTROL)
#define STB_TEXTEDIT_K_TEXTEND         (STB_TEXTEDIT_K_LINEEND   | STB_TEXTEDIT_K_CONTROL)
#define STB_TEXTEDIT_K_DELETE          (KEYDOWN_BIT | 7) // VK_DELETE
#define STB_TEXTEDIT_K_BACKSPACE       (KEYDOWN_BIT | 8) // VK_BACKSPACE
#define STB_TEXTEDIT_K_UNDO            (KEYDOWN_BIT | STB_TEXTEDIT_K_CONTROL | 'z')
#define STB_TEXTEDIT_K_REDO            (KEYDOWN_BIT | STB_TEXTEDIT_K_CONTROL | 'y')
#define STB_TEXTEDIT_K_INSERT          (KEYDOWN_BIT | 9) // VK_INSERT
#define STB_TEXTEDIT_K_WORDLEFT        (STB_TEXTEDIT_K_LEFT  | STB_TEXTEDIT_K_CONTROL)
#define STB_TEXTEDIT_K_WORDRIGHT       (STB_TEXTEDIT_K_RIGHT | STB_TEXTEDIT_K_CONTROL)
#define STB_TEXTEDIT_K_PGUP            (KEYDOWN_BIT | 10) // VK_PGUP -- not implemented
#define STB_TEXTEDIT_K_PGDOWN          (KEYDOWN_BIT | 11) // VK_PGDOWN -- not implemented

#define STB_TEXTEDIT_IMPLEMENTATION
#include "stb_textedit.h"

void dummy3(void)
{
	STB_TexteditState s = {0};
	text_control c = {0};
	stb_textedit_initialize_state (&s, 0);
	stb_textedit_key (&c, &s, 'H');
	stb_textedit_key (&c, &s, 'e');
	stb_textedit_key (&c, &s, 'l');
	stb_textedit_key (&c, &s, 'l');
	stb_textedit_key (&c, &s, 'o');
	stb_textedit_key (&c, &s, ' ');
	stb_textedit_key (&c, &s, 'W');
	stb_textedit_key (&c, &s, 'o');
	stb_textedit_key (&c, &s, 'r');
	stb_textedit_key (&c, &s, 'l');
	stb_textedit_key (&c, &s, 'd');
	stb_textedit_key (&c, &s, '!');
	stb_textedit_click (&c, &s, 0, 0);
	stb_textedit_key (&c, &s, 'Q');
	stb_textedit_click (&c, &s, 5, 0);
	stb_textedit_key (&c, &s, 'X');
	//stb_textedit_drag (&c, &s, 0, 0);
	//stb_textedit_cut (&c, &s);
	//stb_textedit_paste (&c, &s, 0, 0);
}
