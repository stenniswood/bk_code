#ifndef _COLOR_
#define _COLOR_

// See:  http://wiki.tcl.tk/16166 
// For some samples 

class Color
{
public: 
	Color(byte Red, byte Green, byte Blue);
	long int	get_color(				);
	byte		get_alpha(				);	
	void		set_alpha( byte mAlpha );
	
private:
	lont int 	combo;
	byte red;
	byte green;
	byte blue;
	byte alpha;
};

// Predefined colors - symbolic color names
//Tk recognizes many symbolic color names (e.g., red) when specifying colors. 
//Name                       Red  Green  Blue
Color alice blue              (   240,   248,   255	);
Color AliceBlue               (   240,   248,   255	);
Color antique white           (   250,   235,   215	);
Color AntiqueWhite            (   250,   235,   215	);
Color AntiqueWhite1           (   255,   239,   219	);
Color AntiqueWhite2           (   238,   223,   204	);
Color AntiqueWhite3           (   205,   192,   176	);
Color AntiqueWhite4           (   139,   131,   120	);
Color aquamarine              (  127,    255,   212	);
Color aquamarine1             (  127,    255,   212	);
Color aquamarine2             (  118,    238,   198	);
Color aquamarine3             (  102,    205,   170	);
Color aquamarine4             (   69,    139,   116	);
Color azure                   (  240,    255,   255);
Color azure1                  (  240,    255,   255);
Color azure2                  (  224,    238,   238);
Color azure3                  (  193,    205,   205);
Color azure4                  (  131,    139,   139);
Color beige                   (   245,   245,   220);
Color bisque                  (   255,   228,   196);
Color bisque1                 (   255,   228,   196);
Color bisque2                 (   238,   213,   183);
Color bisque3                 (   205,   183,   158);
Color bisque4                 (   139,   125,   107);
Color black                   (    0 ,    0,     0);
Color blanched almond         (  255 ,  235,   205);
Color BlanchedAlmond          (  255 ,  235,   205);
Color blue                    (    0 ,    0,   255);
Color blue violet             (  138 ,   43,   226);
Color blue1                   (    0 ,    0,   255);
Color blue2                   (    0 ,    0,   238);
Color blue3                   (    0 ,    0,   205);
Color blue4                   (    0 ,    0,   139);
Color BlueViolet              (  138 ,   43,   226);
Color brown                   (  165 ,   42,    42);
Color brown1                  (  255 ,   64,    64);
Color brown2                  (   238,    59,    59);
Color brown3                  (   205,    51,    51);
Color brown4                  (   139,    35,    35);
Color burlywood               (   222,   184,   135);
Color burlywood1              (   255,   211,   155);
Color burlywood2              (   238,   197,   145);
Color burlywood3              (   205,   170,   125);
Color burlywood4              (   139,   115,    85);
Color cadet blue              (    95,   158,   160);
Color CadetBlue               (    95,   158,   160);
Color CadetBlue1              (   152,   245,   255);
Color CadetBlue2              (  142 ,  229,  238);
Color CadetBlue3              (  122 ,  197,   205);
Color CadetBlue4              (   83 ,  134,  139);
Color chartreuse              (  127 ,  255,     0);
Color chartreuse1             (  127 ,  255,     0);
Color chartreuse2             (  118 ,  238,     0);
Color chartreuse3             (  102 ,  205,     0);
Color chartreuse4             (   69 ,  139,     0);
Color chocolate               (  210 ,  105,    30);
Color chocolate1              (  255 ,  127,    36);
Color chocolate2              (  238 ,  118,    33);
Color chocolate3              (  205 ,  102,    29);
Color chocolate4              (  139 ,   69,    19);
Color coral                   (  255 ,  127,    80);
Color coral1                  (  255 ,  114,    86);
Color coral2                  (  238 ,  106,    80);
Color coral3                  (  205 ,   91,    69);
Color coral4                  (  139 ,   62,    47);
Color cornflower blue         (  100 ,  149,   237);
Color CornflowerBlue          (  100 ,  149,   237);
Color cornsilk                (  255 ,  248,   220);
Color cornsilk1               (   255,   248,   220);
Color cornsilk2               (   238,   232,   205);
Color cornsilk3               (  205 ,  200,  177);
Color cornsilk4               (  139 ,  136,   120);
Color cyan                    (    0 ,  255,  255);
Color cyan1                   (    0 ,  255,   255);
Color cyan2                   (    0 ,  238,   238);
Color cyan3                   (    0 ,  205,   205);
Color cyan4                   (    0 ,  139,   139);
Color dark blue               (    0 ,    0,   139);
Color dark cyan               (    0 ,  139,   139);
Color dark goldenrod          (  184 ,  134,    11);
Color dark gray               (  169 ,  169,   169);
Color dark green              (    0 ,  100,     0);
Color dark grey               (  169 ,  169,   169);
Color dark khaki              (  189 ,  183,   107);
Color dark magenta            (  139 ,    0,   139);
Color dark olive green        (   85 ,  107,    47);
Color dark orange             (   255,   140,     0);
Color dark orchid             (   153,    50,   204);
Color dark red                (   139,     0,     0);
Color dark salmon             (   233,   150,   122);
Color dark sea green          (   143,   188,   143);
Color dark slate blue         (    72,    61,   139);
Color dark slate gray         (    47,    79,    79);
Color dark slate grey         (   47 ,   79,    79);
Color dark turquoise          (    0 ,  206,   209);
Color dark violet             (  148 ,    0,   211);
Color DarkBlue                (    0 ,    0,   139);
Color DarkCyan                (    0 ,  139,   139);
Color DarkGoldenrod           (  184 ,  134,    11);
Color DarkGoldenrod1          (  255 ,  185,    15);
Color DarkGoldenrod2          (  238 ,  173,    14);
Color DarkGoldenrod3          (  205 ,  149,    12);
Color DarkGoldenrod4          (  139 ,  101,     8);
Color DarkGray                (  169 ,  169,   169);
Color DarkGreen               (    0 ,  100,     0);
Color DarkGrey                (  169 ,  169,   169);
Color DarkKhaki               (  189 ,  183,   107);
Color DarkMagenta             (  139 ,    0,   139);
Color DarkOliveGreen          (   85 ,  107,    47);
Color DarkOliveGreen1         (  202 ,  255,   112);
Color DarkOliveGreen2         (  188 ,  238,   104);
Color DarkOliveGreen3         (  162 ,  205,    90);
Color DarkOliveGreen4         (  110 ,  139,    61);
Color DarkOrange              (  255 ,  140,     0);
Color DarkOrange1             (   255,   127,     0);
Color DarkOrange2             (   238,   118,     0);
Color DarkOrange3             (  205,   102,     0);
Color DarkOrange4             (  139,    69,     0);
Color DarkOrchid              (  153,    50,   204);
Color DarkOrchid1             (  191,    62,   255);
Color DarkOrchid2             (  178,    58,   238);
Color DarkOrchid3             (  154,    50,   205);
Color DarkOrchid4             (  104,    34,   139);
Color DarkRed                 (  139,     0,     0);
Color DarkSalmon              (  233,   150,   122);
Color DarkSeaGreen            (  143,   188,   143);
Color DarkSeaGreen1           (  193,   255,   193);
Color DarkSeaGreen2           (  180,   238,   180);
Color DarkSeaGreen3           (  155,   205,   155);
Color DarkSeaGreen4           (  105,   139,   105);
Color DarkSlateBlue           (   72,    61,   139);
Color DarkSlateGray           (   47,    79,    79);
Color DarkSlateGray1          (  151,   255,   255);
Color DarkSlateGray2          (  141,   238,   238);
Color DarkSlateGray3          (  121,   205,   205);
Color DarkSlateGray4          (   82,   139,   139);
Color DarkSlateGrey           (   47,    79,    79);
Color DarkTurquoise           (    0,   206,   209);
Color DarkViolet              (  148,     0,   211);
Color deep pink               (  255,    20,   147);
Color deep sky blue           (    0,   191,   255);
Color DeepPink                (  255,    20,   147);
Color DeepPink1               (  255,    20,   147);
Color DeepPink2               (  238,    18,   137);
Color DeepPink3               (  205,    16,   118);
Color DeepPink4               (  139,    10,    80);
Color DeepSkyBlue             (    0,   191,   255);
Color DeepSkyBlue1            (    0,   191,   255);
Color DeepSkyBlue2            (    0,   178,   238);
Color DeepSkyBlue3            (    0,   154,   205);
Color DeepSkyBlue4            (    0,   104,   139);
Color dim gray                (  105,   105,   105);
Color dim grey                (  105,   105,   105);
Color DimGray                 (  105,   105,   105);
Color DimGrey                 (  105,   105,   105);
Color dodger blue             (   30,   144,   255);
Color DodgerBlue              (   30,   144,   255);
Color DodgerBlue1             (   30,   144,   255);
Color DodgerBlue2             (   28,   134,   238);
Color DodgerBlue3             (   24,   116,   205);
Color DodgerBlue4             (   16,    78,  139);
Color firebrick               (  178,    34,   34);
Color firebrick1              ( 255 ,   48 ,  48);
Color firebrick2              ( 238 ,   44 ,  44);
Color firebrick3              ( 205 ,   38 ,  38);
Color firebrick4              ( 139 ,   26 ,  26);
Color floral white            (  255,   250,  240);
Color FloralWhite             (  255,   250,  240);
Color forest green            (   34,   139,   34);
Color ForestGreen             (   34,   139,   34);
Color gainsboro               (  220,   220,  220);
Color ghost white             (  248,   248,  255);
Color GhostWhite              (  248,   248,  255);
Color gold                    (  255,   215,    0);
Color gold1                   (  255,   215,    0);
Color gold2                   (  238,   201,    0);
Color gold3                   (  205,   173,    0);
Color gold4                   (  139,   117,    0);
Color goldenrod               (  218,   165,   32);
Color goldenrod1              (  255,   193,   37);
Color goldenrod2              (  238,   180,   34);
Color goldenrod3              (  205,   155,    29);
Color goldenrod4              (   139,   105,    20);
Color gray                    (   190,   190,   190);
Color gray0                   (     0,     0,     0);
Color gray1                   (    3,     3,     3);
Color gray2                   (    5,     5,     5);
Color gray3                   (    8,     8,     8);
Color gray4                   (   10,    10,    10);
Color gray5                   (   13,    13,    13);
Color gray6                   (   15,    15,    15);
Color gray7                   (   18,    18,    18);
Color gray8                   (   20,    20,    20);
Color gray9                   (   23,    23,    23);
Color gray10                  (   26,    26,    26);
Color gray11                  (   28,    28,    28);
Color gray12                  (   31,    31,    31);
Color gray13                  (   33,    33,    33);
Color gray14                  (   36,    36,    36);
Color gray15                  (   38,    38,    38);
Color gray16                  (   41,    41,    41);
Color gray17                  (   43,    43,    43);
Color gray18                  (   46,    46,    46);
Color gray19                  (   48,    48,    48);
Color gray20                  (   51,    51,    51);
Color gray21                  (   54,    54,    54);
Color gray22                  (   56,    56,    56);
Color gray23                  (   59,    59,    59);
Color gray24                  (  61 ,   61 ,   61);
Color gray25                  (  64 ,   64 ,   64);
Color gray26                  (  66 ,   66 ,   66);
Color gray27                  (  69 ,   69 ,   69);
Color gray28                  (  71 ,   71 ,   71);
Color gray29                  (  74 ,   74 ,   74);
Color gray30                  (   77,    77,    77);
Color gray31                  (   79,    79,    79);
Color gray32                  (   82,    82,    82);
Color gray33                  (   84,    84,    84);
Color gray34                  (   87,    87,    87);
Color gray35                  (   89,    89,    89);
Color gray36                  (   92,    92,    92);
Color gray37                  (   94,    94,    94);
Color gray38                  (   97,    97,    97);
Color gray39                  (   99,    99,    99);
Color gray40                  (  102,   102,   102);
Color gray41                  (  105,   105,   105);
Color gray42                  (  107,   107,   107);
Color gray43                  (  110,   110,   110);
Color gray44                  (  112,   112,   112);
Color gray45                  (   115,   115,   115);
Color gray46                  (   117,   117,   117);
Color gray47                  (  120,   120,   120);
Color gray48                  (  122,   122,   122);
Color gray49                  (  125,   125,   125);
Color gray50                  (  127,   127,   127);
Color gray51                  (  130,   130,   130);
Color gray52                  (  133,   133,   133);
Color gray53                  (  135,   135,   135);
Color gray54                  (  138,   138,   138);
Color gray55                  (  140,   140,   140);
Color gray56                  (  143,   143,   143);
Color gray57                  (  145,   145,   145);
Color gray58                  (  148,   148,   148);
Color gray59                  (  150,   150,   150);
Color gray60                  (  153,   153,   153);
Color gray61                  (  156,   156,   156);
Color gray62                  (  158,   158,   158);
Color gray63                  (  161,   161,   161);
Color gray64                  (  163,   163,   163);
Color gray65                  (  166,   166,   166);
Color gray66                  (  168,   168,   168);
Color gray67                  (  171,   171,   171);
Color gray68                  (  173,   173,   173);
Color gray69                  (   176,   176,   176);
Color gray70                  (  179,  179,   179);
Color gray71                  (  181,   181,   181);
Color gray72                  (  184,   184,   184);
Color gray73                  (  186,   186,   186);
Color gray74                  (  189,   189,   189);
Color gray75                  (  191,   191,   191);
Color gray76                  (  194,   194,   194);
Color gray77                  (  196,   196,   196);
Color gray78                  (  199,   199,   199);
Color gray79                  (  201,   201,   201);
Color gray80                  (  204,   204,   204);
Color gray81                  (  207,   207,   207);
Color gray82                  (  209,   209,   209);
Color gray83                  (  212,   212,   212);
Color gray84                  (  214,   214,   214);
Color gray85                  (  217,   217,   217);
Color gray86                  (  219,   219,   219);
Color gray87                  (  222,   222,   222);
Color gray88                  (  224,   224,   224);
Color gray89                  (  227,   227,   227);
Color gray90                  (  229,   229,   229);
Color gray91                  (  232,   232,   232);
Color gray92                  (  235,   235,   235);
Color gray93                  ( 237,   237,   237);
Color gray94                  ( 240,   240,   240);
Color gray95                  ( 242,   242,   242);
Color gray96                  ( 245,   245,   245);
Color gray97                  ( 247,   247,   247);
Color gray98                  ( 250,   250,   250);
Color gray99                  (  252,   252,   252);
Color gray100                 (  255,   255,   255);
Color green                   (    0,   255,     0);
Color green yellow            (  173,   255,    47);
Color green1                  (    0,   255,     0);
Color green2                  (    0,   238,     0);
Color green3                  (    0,   205,     0);
Color green4                  (    0,   139,     0);
Color GreenYellow             (  173,   255,    47);
Color grey                    (  190,   190,   190);
Color grey0                   (    0,     0,     0);
Color grey1                   (    3,     3,     3);
Color grey2                   (    5,     5,     5);
Color grey3                   (    8,     8,     8);
Color grey4                   (   10,    10,    10);
Color grey5                   (   13,    13,    13);
Color grey6                   (   15,    15,    15);
Color grey7                   (  18 ,   18 ,   18);
Color grey8                   (   20,    20,    20);
Color grey9                   (   23,    23,    23);
Color grey10                  (   26,    26,    26);
Color grey11                  (   28,    28,    28);
Color grey12                  (   31,    31,    31);
Color grey13                  (   33,    33,    33);
Color grey14                  (   36,    36,    36);
Color grey15                  (   38,    38,    38);
Color grey16                  (   41,    41,    41);
Color grey17                  (   43,    43,    43);
Color grey18                  (   46,    46,    46);
Color grey19                  (   48,    48,    48);
Color grey20                  (   51,    51,    51);
Color grey21                  (   54,    54,    54);
Color grey22                  (   56,    56,    56);
Color grey23                  (   59,    59,    59);
Color grey24                  (   61,    61,    61);
Color grey25                  (   64,    64,    64);
Color grey26                  (   66,    66,    66);
Color grey27                  (   69,    69,    69);
Color grey28                  (   71,    71,    71);
Color grey29                  (   74,    74,    74);
Color grey30                  (  77,    77,    77);
Color grey31                  (  79,    79,    79);
Color grey32                  (  82,    82,    82);
Color grey33                  (  84,    84,    84);
Color grey34                  (  87,    87,    87);
Color grey35                  (  89,    89,    89);
Color grey36                  (  92,    92,    92);
Color grey37                  (  94,    94,    94);
Color grey38                  (  97,    97,    97);
Color grey39                  (  99,    99,    99);
Color grey40                  ( 102,   102,   102);
Color grey41                  ( 105,   105,   105);
Color grey42                  ( 107,   107,   107);
Color grey43                  ( 110,   110,   110);
Color grey44                  ( 112,   112,   112);
Color grey45                  ( 115,   115,   115);
Color grey46                  ( 117,   117,   117);
Color grey47                  ( 120,   120,   120);
Color grey48                  ( 122,   122,   122);
Color grey49                  ( 125,   125,   125);
Color grey50                  ( 127,   127,   127);
Color grey51                  ( 130,   130,   130);
Color grey52                  ( 133,   133,   133);
Color grey53                  ( 135,   135,   135);
Color grey54                  ( 138,   138,   138);
Color grey55                  ( 140,   140,   140);
Color grey56                  ( 143,   143,   143);
Color grey57                  ( 145,   145,   145);
Color grey58                  (  148,   148,   148);
Color grey59                  (  150,   150,   150);
Color grey60                  (  153,   153,   153);
Color grey61                  (  156,   156,   156);
Color grey62                  (  158,   158,   158);
Color grey63                  (  161,   161,   161);
Color grey64                  (  163,   163,   163);
Color grey65                  (  166,   166,   166);
Color grey66                  (  168,   168,   168);
Color grey67                  (  171,   171,   171);
Color grey68                  (  173,   173,   173);
Color grey69                  (  176,   176,   176);
Color grey70                  (  179,   179,   179);
Color grey71                  (  181,   181,   181);
Color grey72                  (  184,   184,   184);
Color grey73                  (  186,   186,   186);
Color grey74                  (  189,   189,   189);
Color grey75                  (  191,   191,   191);
Color grey76                  (  194,   194,   194);
Color grey77                  (  196,   196,   196);
Color grey78                  (  199,   199,   199);
Color grey79                  (  201,   201,   201);
Color grey80                  (  204,   204,   204);
Color grey81                  (  207,   207,   207);
Color grey82                  (  209,   209,   209);
Color grey83                  (  212,   212,   212);
Color grey84                  (  214,   214,   214);
Color grey85                  (  217,   217,   217);
Color grey86                  (  219,   219,   219);
Color grey87                  (  222,   222,   222);
Color grey88                  (  224,   224,   224);
Color grey89                  (  227,   227,   227);
Color grey90                  (   229,   229,   229);
Color grey91                  (   232,   232,   232);
Color grey92                  (   235,   235,   235);
Color grey93                  (   237,   237,   237);
Color grey94                  (  240,   240,   240);
Color grey95                  (  242,   242,   242);
Color grey96                  (  245,   245,   245);
Color grey97                  (  247,   247,   247);
Color grey98                  (  250,   250,   250);
Color grey99                  (  252,   252,   252);
Color grey100                 (  255,   255,   255);
Color honeydew                (  240,   255,   240);
Color honeydew1               (  240,   255,   240);
Color honeydew2               (  224,   238,   224);
Color honeydew3               (  193,   205,   193);
Color honeydew4               (  131,   139,   131);
Color hot pink                (  255,   105,   180);
Color HotPink                 (  255,   105,   180);
Color HotPink1                (  255,   110,   180);
Color HotPink2                (  238,   106,   167);
Color HotPink3                (  205,    96,   144);
Color HotPink4                (  139,    58,    98);
Color indian red              (  205,    92,    92);
Color IndianRed               (  205,    92,    92);
Color IndianRed1              (  255,   106,   106);
Color IndianRed2              (  238,    99,    99);
Color IndianRed3              (  205,    85,    85);
Color IndianRed4              (  139,    58,    58);
Color ivory                   (  255,   255,   240);
Color ivory1                  (  255,   255,   240);
Color ivory2                  (  238,   238,   224);
Color ivory3                  (  205,   205,   193);
Color ivory4                  (  139,   139,   131);
Color khaki                   (  240,   230,   140);
Color khaki1                  (  255,   246,   143);
Color khaki2                  (  238,   230,   133);
Color khaki3                  (  205,   198,   115);
Color khaki4                  (  139,   134,    78);
Color lavender                (  230,   230,   250);
Color lavender blush          (  255,   240,   245);
Color LavenderBlush           (  255,   240,   245);
Color LavenderBlush1          (  255,   240,   245);
Color LavenderBlush2          (  238,   224,   229);
Color LavenderBlush3          (  205,   193,   197);
Color LavenderBlush4          (  139,   131,   134);
Color lawn green              (  124,   252,     0);
Color LawnGreen               (  124,   252,     0);
Color lemon chiffon           (  255,   250,   205);
Color LemonChiffon            (  255,   250,   205);
Color LemonChiffon1           (  255,   250,   205);
Color LemonChiffon2           (  238,   233,   191);
Color LemonChiffon3           (  205,   201,   165);
Color LemonChiffon4           (   139,   137,   112);
Color light blue              (   173,   216,   230);
Color light coral             (   240,   128,   128);
Color light cyan              (   224,   255,   255);
Color light goldenrod         (   238,   221,   130);
Color light goldenrod yellow  (   250,   250,   210);
Color light gray              (   211,   211,   211);
Color light green             (   144,   238,   144);
Color light grey              (   211,   211,   211);
Color light pink              (   255,   182,   193);
Color light salmon            (   255,   160,   122);
Color light sea green         (    32,   178,   170);
Color light sky blue          (   135,   206,   250);
Color light slate blue        (   132,   112,   255);
Color light slate gray        (   119,   136,   153);
Color light slate grey        (   119,   136,   153);
Color light steel blue        (  176,   196 ,  222);
Color light yellow            (  255,   255 ,  224);
Color LightBlue               (  173,   216 ,  230);
Color LightBlue1              (  191,   239 ,  255);
Color LightBlue2              (  178,   223 ,  238);
Color LightBlue3              (  154,   192 ,  205);
Color LightBlue4              (  104,   131 ,  139);
Color LightCoral              (  240,   128 ,  128);
Color LightCyan               (  224,   255 ,  255);
Color LightCyan1              (  224,   255 ,  255);
Color LightCyan2              (  209,   238 ,  238);
Color LightCyan3              (  180,   205 ,  205);
Color LightCyan4              (  122,   139 ,  139);
Color LightGoldenrod          (  238,   221 ,  130);
Color LightGoldenrod1         (  255,   236 ,  139);
Color LightGoldenrod2         (  238,   220 ,  130);
Color LightGoldenrod3         (  205,   190 ,  112);
Color LightGoldenrod4         (  139,   129 ,   76);
Color LightGoldenrodYellow    (  250,   250 ,  210);
Color LightGray               (  211,   211 ,  211);
Color LightGreen              (  144,   238 ,  144);
Color LightGrey               (  211,   211 ,  211);
Color LightPink               (  255,   182 ,  193);
Color LightPink1              (  255,   174 ,  185);
Color LightPink2              (  238,   162 ,  173);
Color LightPink3              (  205,   140 ,  149);
Color LightPink4              (  139,    95 ,  101);
Color LightSalmon             (  255,   160 ,  122);
Color LightSalmon1            (  255,   160 , 122);
Color LightSalmon2            (  238,   149 , 114);
Color LightSalmon3            (   205,   129,   98);
Color LightSalmon4            (   139,    87,   66);
Color LightSeaGreen           (    32,   178,  170);
Color LightSkyBlue            (   135,   206,  250);
Color LightSkyBlue1           (   176,   226,  255);
Color LightSkyBlue2           (   164,   211,  238);
Color LightSkyBlue3           (   141,   182,  205);
Color LightSkyBlue4           (    96,   123,  139);
Color LightSlateBlue          (   132,   112,  255);
Color LightSlateGray          (   119,   136,  153);
Color LightSlateGrey          (   119,   136,  153);
Color LightSteelBlue          (   176,   196,  222);
Color LightSteelBlue1         (   202,   225,  255);
Color LightSteelBlue2         (   188,   210,  238);
Color LightSteelBlue3         (   162,   181,  205);
Color LightSteelBlue4         (   110,   123,  139);
Color LightYellow             (   255,   255,  224);
Color LightYellow1            (   255,   255,  224);
Color LightYellow2            (   238,   238,  209);
Color LightYellow3            (   205,   205,  180);
Color LightYellow4            (   139,   139,  122);
Color lime green              (    50,   205,   50);
Color LimeGreen               (    50,   205,   50);
Color linen                   (   250,   240,  230);
Color magenta                 (   255,     0,  255);
Color magenta1                (   255,     0,   255);
Color magenta2                (   238,     0,   238);
Color magenta3                (   205,     0,   205);
Color magenta4                (   139,     0,  139);
Color maroon                  (   176,    48,   96);
Color maroon1                 (   255,    52,  179);
Color maroon2                 (   238,    48,  167);
Color maroon3                 (   205,    41,  144);
Color maroon4                 (   139,    28,   98);
Color medium aquamarine       (  102,   205 , 170);
Color medium blue             (    0,     0 , 205);
Color medium orchid           (  186,    85 , 211);
Color medium purple           (  147,   112 , 219);
Color medium sea green        (   60,   179 , 113);
Color medium slate blue       (  123,   104 , 238);
Color medium spring green     (    0,   250 , 154);
Color medium turquoise        (   72,   209 , 204);
Color medium violet red       (  199,    21 , 133);
Color MediumAquamarine        (  102,   205 , 170);
Color MediumBlue              (    0,     0 , 205);
Color MediumOrchid            (  186,    85 , 211);
Color MediumOrchid1           (  224,   102 , 255);
Color MediumOrchid2           (  209,    95 , 238);
Color MediumOrchid3           (  180,    82 , 205);
Color MediumOrchid4           (  122,    55 , 139);
Color MediumPurple            (  147,   112 , 219);
Color MediumPurple1           (  171,   130 ,  255);
Color MediumPurple2           (  159,   121 ,  238);
Color MediumPurple3           (  137,   104 ,  205);
Color MediumPurple4           (   93,    71 ,  139);
Color MediumSeaGreen          (   60,   179 ,  113);
Color MediumSlateBlue         (  123,   104 ,  238);
Color MediumSpringGreen       (    0,   250 ,  154);
Color MediumTurquoise         (   72,   209 , 204);
Color MediumVioletRed         (  199,    21 , 133);
Color midnight blue           (   25,    25 , 112);
Color MidnightBlue            (   25,    25 , 112);
Color mint cream              (  245,   255 , 250);
Color MintCream               (  245,   255 , 250);
Color misty rose              (  255,   228 , 225);
Color MistyRose               (   255,   228,  225);
Color MistyRose1              (   255,   228,  225);
Color MistyRose2              (   238,   213,  210);
Color MistyRose3              (   205,   183,  181);
Color MistyRose4              (   139,   125,  123);
Color moccasin                (   255,   228,  181);
Color navajo white            (   255,   222,  173);
Color NavajoWhite             (   255,   222,  173);
Color NavajoWhite1            (   255,   222,  173);
Color NavajoWhite2            (   238,   207,  161);
Color NavajoWhite3            (   205,   179,  139);
Color NavajoWhite4            (   139,   121,   94);
Color navy                    (     0,     0,   128);
Color navy blue               (     0,     0,   128);
Color NavyBlue                (     0,     0,   128);
Color old lace                (   253,   245,   230);
Color OldLace                 (   253,   245,   230);
Color olive drab              (   107,   142,    35);
Color OliveDrab               (   107,   142,    35);
Color OliveDrab1              (   192,   255,    62);
Color OliveDrab2              (   179,   238,    58);
Color OliveDrab3              (   154,   205,    50);
Color OliveDrab4              (   105,   139,    34);
Color orange                  (   255,   165,     0);
Color orange red              (   255,    69,     0);
Color orange1                 (   255,   165,     0);
Color orange2                 (   238,   154,     0);
Color orange3                 (   205,   133,     0);
Color orange4                 (   139,    90,     0);
Color OrangeRed               (   255,    69,     0);
Color OrangeRed1              (   255,    69,     0);
Color OrangeRed2              (   238,    64,     0);
Color OrangeRed3              (   205,    55,     0);
Color OrangeRed4              (  139 ,   37 ,    0);
Color orchid                  (  218 ,  112 ,  214);
Color orchid1                 (  255 ,  131 ,  250);
Color orchid2                 (  238 ,  122 ,  233);
Color orchid3                 (  205 ,  105 ,  201);
Color orchid4                 (  139 ,   71 ,  137);
Color pale goldenrod          (  238 ,  232 ,  170);
Color pale green              (  152 ,  251 ,  152);
Color pale turquoise          (  175 ,  238 ,  238);
Color pale violet red         (  219 ,  112 ,  147);
Color PaleGoldenrod           (  238 ,  232 ,  170);
Color PaleGreen               (  152 ,  251 ,  152);
Color PaleGreen1              (  154 ,  255 ,  154);
Color PaleGreen2              (  144 ,  238 ,  144);
Color PaleGreen3              (  124 ,  205 ,  124);
Color PaleGreen4              (   84 ,  139 ,   84);
Color PaleTurquoise           (  175 ,  238 ,  238);
Color PaleTurquoise1          (  187 ,  255 ,  255);
Color PaleTurquoise2          (  174 ,  238 ,  238);
Color PaleTurquoise3          (  150 ,  205 ,  205);
Color PaleTurquoise4          (  102 ,  139 ,  139);

Color PaleVioletRed           (  219 ,  112,   147);
Color PaleVioletRed1          (  255 ,  130,   171);
Color PaleVioletRed2          (  238 ,  121,   159);
Color PaleVioletRed3          (  205 ,  104,   127);
Color PaleVioletRed4          (  139 ,   71,    93);
Color papaya whip             (   255,   239,   213);
Color PapayaWhip              (   255,   239,   213);
Color peach puff              (   255,   218,   185);
Color PeachPuff               (   255,   218,   185);
Color PeachPuff1              (   255,   218,   185);
Color PeachPuff2              (   238,  203,   173);
Color PeachPuff3              (   205,  175,   149);
Color PeachPuff4              (   139,  119,   101);
Color peru                    (   205,  133,    63);
Color pink                    (   255,  192,   203);
Color pink1                   (   255,  181,   197);
Color pink2                   (   238,  169,   184);
Color pink3                   (   205,  145,   158);
Color pink4                   (   139,   99,   108);
Color plum                    (   221,  160,   221);
Color plum1                   (   255,  187,   255);
Color plum2                   (   238,  174,   238);
Color plum3                   (   205,  150,   205);
Color plum4                   (   139,  102,   139);
Color powder blue             (   176,  224,   230);
Color PowderBlue              (   176,  224,   230);
Color purple                  (   160,   32,   240);
Color purple1                 (   155,   48,   255);
Color purple2                 (   145,   44,   238);
Color purple3                 (   125,   38,   205);
Color purple4                 (    85,   26,   139);
Color red                     (   255,    0,     0);
Color red1                    (   255,    0,     0);
Color red2                    (   238,    0,     0);
Color red3                    (   205,    0,     0);
Color red4                    (   139,     0,     0);
Color rosy brown              (   188,   143,   143);
Color RosyBrown               (   188,   143,   143);
Color RosyBrown1              (   255,   193,   193);
Color RosyBrown2              (   238,   180,   180);
Color RosyBrown3              (   205,   155,   155);
Color RosyBrown4              (   139,   105,   105);
Color royal blue              (   65 ,  105 ,  225);
Color RoyalBlue               (   65 ,  105 ,  225);
Color RoyalBlue1              (   72 ,  118 ,  255);
Color RoyalBlue2              (   67 ,  110 ,  238);
Color RoyalBlue3              (   58 ,   95 ,  205);
Color RoyalBlue4              (   39 ,   64 ,  139);
Color saddle brown            (  139 ,   69 ,   19);
Color SaddleBrown             (  139 ,   69 ,   19);
Color salmon                  (  250 ,  128 ,  114);
Color salmon1                 (  255 ,  140 ,  105);
Color salmon2                 (  238 ,  130 ,   98);
Color salmon3                 (  205 ,  112 ,   84);
Color salmon4                 (  139 ,   76 ,   57);
Color sandy brown             (  244 ,  164 ,   96);
Color SandyBrown              (  244 ,  164 ,   96);
Color sea green               (   46 ,  139 ,   87);
Color SeaGreen                (   46 ,  139 ,   87);
Color SeaGreen1               (   84 ,  255 ,  159);
Color SeaGreen2               (   78 ,  238 ,  148);
Color SeaGreen3               (   67 ,  205 ,  128);
Color SeaGreen4               (   46 ,  139 ,   87);
Color seashell                (  255 ,  245 ,  238);
Color seashell1               (  255 ,  245 ,  238);
Color seashell2               (  238 ,  229,   222);
Color seashell3               (  205 ,  197,   191);
Color seashell4               (  139 ,  134,   130);
Color sienna                  (  160 ,   82,    45);
Color sienna1                 (  255 ,  130,    71);
Color sienna2                 (  238 ,  121,    66);
Color sienna3                 (  205 ,  104,    57);
Color sienna4                 (  139 ,   71,    38);
Color sky blue                (  135 ,  206,   235);
Color SkyBlue                 (  135 ,  206,   235);
Color SkyBlue1                (  135 ,  206,   255);
Color SkyBlue2                (  126 ,  192,   238);
Color SkyBlue3                (  108 ,  166,   205);
Color SkyBlue4                (   74 ,  112,   139);
Color slate blue              (  106 ,   90,   205);
Color slate gray              (  112 ,  128,   144);
Color slate grey              (  112 ,  128,   144);
Color SlateBlue               (  106 ,   90,   205);
Color SlateBlue1              (  131 ,  111,   255);
Color SlateBlue2              (  122 ,  103,   238);
Color SlateBlue3              (  105 ,   89,   205);
Color SlateBlue4              (   71 ,   60,   139);
Color SlateGray               (  112 ,  128,   144);
Color SlateGray1              (  198 ,  226,   255);
Color SlateGray2              (  185 ,  211,   238);
Color SlateGray3              (  159 ,  182,   205);
Color SlateGray4              (  108 ,  123,   139);
Color SlateGrey               (  112 ,  128,   144);
Color snow                    (  255 ,  250,   250);
Color snow1                   (  255 ,  250,   250);
Color snow2                   (  238 ,  233,   233);
Color snow3                   (  205 ,  201,   201);
Color snow4                   (  139 ,  137,   137);
Color spring green            (    0 ,  255,   127);
Color SpringGreen             (    0 ,  255,   127);
Color SpringGreen1            (    0 ,  255,   127);
Color SpringGreen2            (    0 ,  238,   118);
Color SpringGreen3            (    0 ,  205,   102);
Color SpringGreen4            (    0 ,  139,    69);
Color steel blue              (   70 ,  130,   180);
Color SteelBlue               (   70 ,  130,   180);
Color SteelBlue1              (  99  , 184 ,  255);
Color SteelBlue2              (   92 ,  172,   238);
Color SteelBlue3              (   79 ,  148,   205);
Color SteelBlue4              (   54 ,  100,   139);
Color tan                     (  210 ,  180,   140);
Color tan1                    (  255 ,  165,    79);
Color tan2                    (  238 ,  154,    73);
Color tan3                    (  205 ,  133,    63);
Color tan4                    (  139 ,   90,    43);
Color thistle                 (  216 ,  191,   216);
Color thistle1                (  255 ,  225,   255);
Color thistle2                (  238 ,  210,   238);
Color thistle3                (  205 ,  181,   205);
Color thistle4                (  139 ,  123,   139);
Color tomato                  (  255 ,   99,    71 );
Color tomato1                 (  255 ,   99,    71);
Color tomato2                 (   238,    92,    66);
Color tomato3                 (  205 ,   79,    57);
Color tomato4                 (  139 ,   54,    38);
Color turquoise               (   64 ,  224,   208);
Color turquoise1              (    0 ,  245,   255);
Color turquoise2              (    0 ,  229,   238);
Color turquoise3              (    0 ,  197,   205);
Color turquoise4              (    0 ,  134,   139);
Color violet                  (  238 ,  130,   238);
Color violet red              (  208 ,   32,   144);
Color VioletRed               (  208 ,   32,   144);
Color VioletRed1              (  255 ,   62,   150);
Color VioletRed2              (  238 ,   58,   140);
Color VioletRed3              (  205 ,   50,   120);
Color VioletRed4              (  139 ,   34,    82);
Color wheat                   (  245 ,  222,   179);
Color wheat1                  (  255 ,  231,   186);
Color wheat2                  (  238 ,  216,   174);
Color wheat3                  ( 205  , 186 ,  150);
Color wheat4                  ( 139  , 126 ,  102);
Color white                   ( 255  , 255 ,  255);
Color white smoke             ( 245  , 245 ,  245);
Color WhiteSmoke              ( 245  , 245 ,  245);
Color yellow                  (  255 ,  255,     0);
Color yellow green            (  154 ,  205,    50);
Color yellow1                 (  255 ,  255,     0);
Color yellow2                 (  238 ,  238,     0);
Color yellow3                 (  205 ,  205,     0);
Color yellow4                 (  139 ,  139,     0);
Color YellowGreen             (  154 ,  205,    50);
,
/*Color PORTABILITY ISSUES
 Mac OS X
Color     On Mac OS X, the following additional system colors are available 
(note that the actual color values depend on the currently active OS theme, 
and typically many of these will in fact be patterns rather than pure colors):
Color 
    systemTransparent
    systemBlack
    systemWhite
    systemHighlight
    systemPrimaryHighlightColor
    systemHighlightSecondary
    systemSecondaryHighlightColor
    systemHighlightAlternate
    systemAlternatePrimaryHighlightColor
    systemHighlightText
    systemDialogBackgroundActive
    systemDialogBackgroundInactive
    systemAlertBackgroundActive
    systemAlertBackgroundInactive
    systemModelessDialogBackgroundActive
    systemModelessDialogBackgroundInactive
    systemUtilityWindowBackgroundActive
    systemUtilityWindowBackgroundInactive
    systemListViewSortColumnBackground
    systemListViewBackground
    systemIconLabelBackground
    systemListViewSeparator
    systemChasingArrows
    systemDragHilite
    systemWindowBody
    systemDocumentWindowBackground
    systemFinderWindowBackground
    systemScrollBarDelimiterActive
    systemScrollBarDelimiterInactive
    systemFocusHighlight
    systemPopupArrowActive
    systemPopupArrowPressed
    systemPopupArrowInactive
    systemAppleGuideCoachmark
    systemIconLabelBackgroundSelected
    systemStaticAreaFill
    systemActiveAreaFill
    systemButtonFrame
    systemButtonFrameActive
    systemButtonFrameInactive
    systemButtonFace
    systemButtonFaceActive
    systemButtonFaceInactive
    systemButtonFacePressed
    systemButtonActiveDarkShadow
    systemButtonActiveDarkHighlight
    systemButtonActiveLightShadow
    systemButtonActiveLightHighlight
    systemButtonInactiveDarkShadow
    systemButtonInactiveDarkHighlight
    systemButtonInactiveLightShadow
    systemButtonInactiveLightHighlight
    systemButtonPressedDarkShadow
    systemButtonPressedDarkHighlight
    systemButtonPressedLightShadow
    systemButtonPressedLightHighlight
    systemBevelActiveLight
    systemBevelActiveDark
    systemBevelInactiveLight
    systemBevelInactiveDark
    systemNotificationWindowBackground
    systemMovableModalBackground
    systemSheetBackground
    systemSheetBackgroundOpaque
    systemDrawerBackground
    systemToolbarBackground
    systemSheetBackgroundTransparent
    systemMenu
    systemMenuBackground
    systemMenuActive
    systemMenuBackgroundSelected
    systemListViewOddRowBackground
    systemListViewEvenRowBackground
    systemListViewColumnDivider
    systemTabPaneBackground
    systemPlacardBackground
    systemWindowHeaderBackground
    systemListViewWindowHeaderBackground
    systemSecondaryGroupBoxBackground
    systemMetalBackground
    systemBlackText
    systemWhiteText
    systemDialogActiveText
    systemDialogInactiveText
    systemAlertActiveText
    systemAlertInactiveText
    systemModelessDialogActiveText
    systemModelessDialogInactiveText
    systemWindowHeaderActiveText
    systemWindowHeaderInactiveText
    systemPlacardActiveText
    systemPlacardInactiveText
    systemPlacardPressedText
    systemButtonText
    systemPushButtonActiveText
    systemPushButtonInactiveText
    systemPushButtonPressedText
    systemBevelButtonActiveText
    systemBevelButtonInactiveText
    systemBevelButtonPressedText
    systemPopupButtonActiveText
    systemPopupButtonInactiveText
    systemPopupButtonPressedText
    systemIconLabelText
    systemListViewText
    systemDocumentWindowTitleActiveText
    systemDocumentWindowTitleInactiveText
    systemMovableModalWindowTitleActiveText
    systemMovableModalWindowTitleInactiveText
    systemUtilityWindowTitleActiveText
    systemUtilityWindowTitleInactiveText
    systemPopupWindowTitleActiveText
    systemPopupWindowTitleInactiveText
    systemRootMenuActiveText
    systemRootMenuSelectedText
    systemRootMenuDisabledText
    systemMenuText
    systemMenuItemActiveText
    systemMenuActiveText
    systemMenuItemSelectedText
    systemMenuDisabled
    systemMenuItemDisabledText
    systemPopupLabelActiveText
    systemPopupLabelInactiveText
    systemTabFrontActiveText
    systemTabNonFrontActiveText
    systemTabNonFrontPressedText
    systemTabFrontInactiveText
    systemTabNonFrontInactiveText
    systemIconLabelSelectedText
    systemBevelButtonStickyActiveText
    systemBevelButtonStickyInactiveText
    systemNotificationText
    systemSystemDetailText
*/
#endif
