/* If no other argument vector is specified via the UNIX socket, then a
 * newly created terminal window will launch the user's shell. This
 * option specifies whether the shell will be a login shell or not. */
gboolean login_shell = TRUE;

/* Whether or not to enable the usage of bold fonts. See also c_bold,
 * which is always used for bold text with the default color, regardless
 * of this setting. */
gboolean enable_bold = TRUE;

/* Default fonts and font sizes. These strings will be parsed by pango,
 * see the following URL:
 * https://developer.gnome.org/pango/stable/pango-Fonts.html#pango-font-description-from-string
 *
 * You can define up to 9 fonts, at least one font must be defined. You
 * can switch between these fonts during runtime using Ctrl+Keypad$n.
 * The first font will be index 0 and can be activated using
 * Ctrl+Keypad1. Second one with Ctrl+Keypad2 and so on.
 */
char *fonts[] = {
		"Hermit 12",
		"monofur for Powerline 14",
};

/* Use 0 to disable scrolling completely or a negative value for
 * infinite scrolling. Keep the memory footprint in mind, though. */
guint scrollback_lines = 50000;

/* This regular expression is used to match URLs. You can easily spot
 * them by hovering over them with your mouse. Use your right mouse
 * button to copy the URL to your clipboard. */
char *url_regex = "[a-z]+://[[:graph:]]+";

/* Background color of text under the cursor. There's a special mode:
 * "If NULL, text under the cursor will be drawn with foreground and
 * background colors reversed." */
char *c_cursor = NULL;

/* Foreground color of text under the cursor. Just like the background
 * color, NULL reverses foreground and background. */
char *c_cursor_foreground = NULL;

/* Quoting from the VTE reference: "Sets the color used to draw bold
 * text in the default foreground color. If [...] NULL then the default
 * color is used." */
char *c_bold = NULL;

/* Set the terminal's color palette. Note that none of these values can
 * be NULL. */
char *c_foreground = "#c2b790";
char *c_background = "#222222";
char *c_palette[16] = {
    /* Dark */
    /* Black */   "#202020",
    /* Red */     "#8c3432",
    /* Green */   "#728271",
    /* Yellow */  "#e8bf6a",
    /* Blue */    "#69819e",
    /* Magenta */ "#896492",
    /* Cyan */    "#5b8390",
    /* White */   "#b9aa99",

    /* Bright */
    /* Black */   "#676767",
    /* Red */     "#b55242",
    /* Green */   "#869985",
    /* Yellow */  "#ebeb91",
    /* Blue */    "#60827e",
    /* Magenta */ "#de4974",
    /* Cyan */    "#38add8",
    /* White */   "#d6bfb8",
};
