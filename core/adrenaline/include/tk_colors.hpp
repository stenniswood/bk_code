#ifndef _COLOR_
#define _COLOR_

// See:  http://wiki.tcl.tk/16166 
// For some samples 
#include "global.h"
//#include "bk_system_defs.h"


class Color
{
public: 
	Color		(byte Red, byte Green, byte Blue);
	long int	get_color(				);
	float		get_alpha(				) { return (float)m_alpha / 255.;  };
	void		set_alpha( float mAlpha ) ;
	void		set_alpha_byte( byte mAlpha ) ;

	long int 	combo;
	byte m_red;
	byte m_green;
	byte m_blue;
	byte m_alpha;
};

// Predefined colors - symbolic color names
//Tk recognizes many symbolic color names (e.g., red) when specifying colors. 
//Name ;       Red  Green  Blue
extern Color alice_blue  ;      
extern Color AliceBlue  ;       
extern Color antique_white  ;   
extern Color AntiqueWhite  ;    
extern Color AntiqueWhite1  ;   
extern Color AntiqueWhite2  ;   
extern Color AntiqueWhite3  ;   
extern Color AntiqueWhite4  ;   
extern Color aquamarine  ;      
extern Color aquamarine1  ;     
extern Color aquamarine2  ;     
extern Color aquamarine3  ;     
extern Color aquamarine4  ;     
extern Color azure ;   
extern Color azure1 ;  
extern Color azure2 ;  
extern Color azure3 ;  
extern Color azure4 ;  
extern Color beige ;   
extern Color bisque ;  
extern Color bisque1 ; 
extern Color bisque2 ; 
extern Color bisque3 ; 
extern Color bisque4 ; 
extern Color black ;   
extern Color blanched_almond  ; 
extern Color BlanchedAlmond  ;  
extern Color blue ;   
extern Color blue_violet  ;     
extern Color blue1 ;   
extern Color blue2 ;   
extern Color blue3 ;   
extern Color blue4 ;   
extern Color BlueViolet  ;      
extern Color brown ;   
extern Color brown1 ;  
extern Color brown2 ;  
extern Color brown3 ;  
extern Color brown4 ;  
extern Color burlywood  ;       
extern Color burlywood1  ;      
extern Color burlywood2  ;      
extern Color burlywood3  ;      
extern Color burlywood4  ;      
extern Color cadet_blue  ;      
extern Color CadetBlue  ;       
extern Color CadetBlue1  ;      
extern Color CadetBlue2  ;      
extern Color CadetBlue3  ;      
extern Color CadetBlue4  ;      
extern Color chartreuse  ;      
extern Color chartreuse1  ;     
extern Color chartreuse2  ;     
extern Color chartreuse3  ;     
extern Color chartreuse4  ;     
extern Color chocolate  ;       
extern Color chocolate1  ;      
extern Color chocolate2  ;      
extern Color chocolate3  ;      
extern Color chocolate4  ;      
extern Color coral ;   
extern Color coral1 ;  
extern Color coral2 ;  
extern Color coral3 ;  
extern Color coral4 ;  
extern Color cornflower_blue  ; 
extern Color CornflowerBlue  ;  
extern Color cornsilk ;
extern Color cornsilk1  ;       
extern Color cornsilk2  ;       
extern Color cornsilk3  ;       
extern Color cornsilk4  ;       
extern Color cyan ;    
extern Color cyan1 ;   
extern Color cyan2 ;   
extern Color cyan3 ;   
extern Color cyan4 ;   
extern Color dark_blue  ;       
extern Color dark_cyan  ;       
extern Color dark_goldenrod  ;  
extern Color dark_gray  ;       
extern Color dark_green  ;      
extern Color dark_grey  ;       
extern Color dark_khaki  ;      
extern Color dark_magenta  ;    
extern Color dark_olive_green  ;
extern Color dark_orange  ;     
extern Color dark_orchid  ;     
extern Color dark_red ;
extern Color dark_salmon  ;     
extern Color dark_sea_green  ;  
extern Color dark_slate_blue  ; 
extern Color dark_slate_gray  ; 
extern Color dark_slate_grey  ; 
extern Color dark_turquoise  ;  
extern Color dark_violet  ;     
extern Color DarkBlue ;
extern Color DarkCyan ;
extern Color DarkGoldenrod  ;   
extern Color DarkGoldenrod1  ;  
extern Color DarkGoldenrod2  ;  
extern Color DarkGoldenrod3  ;  
extern Color DarkGoldenrod4  ;  
extern Color DarkGray ;
extern Color DarkGreen  ;       
extern Color DarkGrey ;
extern Color DarkKhaki  ;       
extern Color DarkMagenta  ;     
extern Color DarkOliveGreen  ;  
extern Color DarkOliveGreen1  ; 
extern Color DarkOliveGreen2  ; 
extern Color DarkOliveGreen3  ; 
extern Color DarkOliveGreen4  ; 
extern Color DarkOrange  ;      
extern Color DarkOrange1  ;     
extern Color DarkOrange2  ;     
extern Color DarkOrange3  ;     
extern Color DarkOrange4  ;     
extern Color DarkOrchid  ;      
extern Color DarkOrchid1  ;     
extern Color DarkOrchid2  ;     
extern Color DarkOrchid3  ;    ;
extern Color DarkOrchid4  ;    
extern Color DarkRed ;
extern Color DarkSalmon  ;     
extern Color DarkSeaGreen  ;   
extern Color DarkSeaGreen1  ;  
extern Color DarkSeaGreen2  ;  
extern Color DarkSeaGreen3  ;  
extern Color DarkSeaGreen4  ;  
extern Color DarkSlateBlue  ;  
extern Color DarkSlateGray  ;  
extern Color DarkSlateGray1  ; 
extern Color DarkSlateGray2  ; 
extern Color DarkSlateGray3  ; 
extern Color DarkSlateGray4  ; 
extern Color DarkSlateGrey  ;  
extern Color DarkTurquoise  ;  
extern Color DarkViolet  ;     
extern Color deep_pink  ;      
extern Color deep_sky_blue  ;  
extern Color DeepPink  ;       
extern Color DeepPink1  ;      
extern Color DeepPink2  ;      
extern Color DeepPink3  ;      
extern Color DeepPink4  ;      
extern Color DeepSkyBlue  ;    
extern Color DeepSkyBlue1  ;   
extern Color DeepSkyBlue2  ;   
extern Color DeepSkyBlue3  ;   
extern Color DeepSkyBlue4  ;   
extern Color dim_gray  ;       
extern Color dim_grey  ;       
extern Color DimGray ;
extern Color DimGrey ;
extern Color dodger_blue  ;    
extern Color DodgerBlue  ;     
extern Color DodgerBlue1  ;    
extern Color DodgerBlue2  ;    
extern Color DodgerBlue3  ;    
extern Color DodgerBlue4  ;    
extern Color firebrick  ;      
extern Color firebrick1  ;     
extern Color firebrick2  ;     
extern Color firebrick3  ;     
extern Color firebrick4  ;     
extern Color floral_white  ;   
extern Color FloralWhite  ;    
extern Color forest_green  ;   
extern Color ForestGreen  ;    
extern Color gainsboro  ;      
extern Color ghost_white  ;    
extern Color GhostWhite  ;     
extern Color gold ;   
extern Color gold1 ;  
extern Color gold2 ;  
extern Color gold3 ;  
extern Color gold4 ;  
extern Color goldenrod  ;      
extern Color goldenrod1  ;     
extern Color goldenrod2  ;     
extern Color goldenrod3  ;     
extern Color goldenrod4  ;     
extern Color gray ;   
extern Color gray0 ;  
extern Color gray1 ;  
extern Color gray2 ;  
extern Color gray3 ;  
extern Color gray4 ;  
extern Color gray5 ;  
extern Color gray6 ;  
extern Color gray7 ;  
extern Color gray8 ;  
extern Color gray9 ;  
extern Color gray10 ; 
extern Color gray11 ; 
extern Color gray12 ; 
extern Color gray13 ; 
extern Color gray14 ; 
extern Color gray15 ; 
extern Color gray16 ; 
extern Color gray17 ; 
extern Color gray18 ; 
extern Color gray19 ; 
extern Color gray20 ; 
extern Color gray21 ; 
extern Color gray22 ; 
extern Color gray23 ; 
extern Color gray24 ; 
extern Color gray25 ; 
extern Color gray26 ; 
extern Color gray27 ; 
extern Color gray28 ; 
extern Color gray29 ; 
extern Color gray30 ; 
extern Color gray31 ; 
extern Color gray32 ; 
extern Color gray33 ; 
extern Color gray34 ; 
extern Color gray35 ; 
extern Color gray36 ; 
extern Color gray37 ; 
extern Color gray38 ; 
extern Color gray39 ; 
extern Color gray40 ; 
extern Color gray41 ; 
extern Color gray42 ; 
extern Color gray43 ; 
extern Color gray44 ; 
extern Color gray45 ; 
extern Color gray46 ; 
extern Color gray47 ; 
extern Color gray48 ; 
extern Color gray49 ; 
extern Color gray50 ; 
extern Color gray51 ; 
extern Color gray52 ; 
extern Color gray53 ; 
extern Color gray54 ; 
extern Color gray55 ; 
extern Color gray56 ; 
extern Color gray57 ; 
extern Color gray58 ; 
extern Color gray59 ; 
extern Color gray60 ; 
extern Color gray61 ; 
extern Color gray62 ; 
extern Color gray63 ; 
extern Color gray64 ; 
extern Color gray65 ; 
extern Color gray66 ; 
extern Color gray67 ; 
extern Color gray68 ; 
extern Color gray69 ; 
extern Color gray70 ; 
extern Color gray71 ; 
extern Color gray72 ; 
extern Color gray73 ; 
extern Color gray74 ; 
extern Color gray75 ; 
extern Color gray76 ; 
extern Color gray77 ; 
extern Color gray78 ; 
extern Color gray79 ; 
extern Color gray80 ; 
extern Color gray81 ; 
extern Color gray82 ; 
extern Color gray83 ; 
extern Color gray84 ; 
extern Color gray85 ; 
extern Color gray86 ; 
extern Color gray87 ; 
extern Color gray88 ; 
extern Color gray89 ; 
extern Color gray90 ; 
extern Color gray91 ; 
extern Color gray92 ; 
extern Color gray93 ; 
extern Color gray94 ; 
extern Color gray95 ; 
extern Color gray96 ; 
extern Color gray97 ; 
extern Color gray98 ; 
extern Color gray99 ; 
extern Color gray100 ;
extern Color green ; 
extern Color green_yellow  ;   
extern Color green1 ; 
extern Color green2 ; 
extern Color green3 ; 
extern Color green4 ; 
extern Color GreenYellow  ;    
extern Color grey ;   
extern Color grey0 ;  
extern Color grey1 ;  
extern Color grey2 ;  
extern Color grey3 ;  
extern Color grey4 ;  
extern Color grey5 ;  
extern Color grey6 ;  
extern Color grey7 ;  
extern Color grey8 ;  
extern Color grey9 ;  
extern Color grey10 ; 
extern Color grey11 ; 
extern Color grey12 ; 
extern Color grey13 ; 
extern Color grey14 ; 
extern Color grey15 ; 
extern Color grey16 ; 
extern Color grey17 ; 
extern Color grey18 ; 
extern Color grey19 ; 
extern Color grey20 ; 
extern Color grey21 ; 
extern Color grey22 ; 
extern Color grey23 ; 
extern Color grey24 ; 
extern Color grey25 ; 
extern Color grey26 ; 
extern Color grey27 ; 
extern Color grey28 ; 
extern Color grey29 ; 
extern Color grey30 ; 
extern Color grey31 ; 
extern Color grey32 ; 
extern Color grey33 ; 
extern Color grey34 ; 
extern Color grey35 ; 
extern Color grey36 ; 
extern Color grey37 ; 
extern Color grey38 ; 
extern Color grey39 ; 
extern Color grey40 ; 
extern Color grey41 ; 
extern Color grey42 ; 
extern Color grey43 ; 
extern Color grey44 ; 
extern Color grey45 ; 
extern Color grey46 ; 
extern Color grey47 ; 
extern Color grey48 ; 
extern Color grey49 ; 
extern Color grey50 ; 
extern Color grey51 ; 
extern Color grey52 ; 
extern Color grey53 ; 
extern Color grey54 ; 
extern Color grey55 ; 
extern Color grey56 ; 
extern Color grey57 ; 
extern Color grey58 ; 
extern Color grey59 ; 
extern Color grey60 ; 
extern Color grey61 ; 
extern Color grey62 ; 
extern Color grey63 ; 
extern Color grey64 ; 
extern Color grey65 ; 
extern Color grey66 ; 
extern Color grey67 ; 
extern Color grey68 ; 
extern Color grey69 ; 
extern Color grey70 ; 
extern Color grey71 ; 
extern Color grey72 ; 
extern Color grey73 ; 
extern Color grey74 ; 
extern Color grey75 ; 
extern Color grey76 ; 
extern Color grey77 ; 
extern Color grey78 ; 
extern Color grey79 ; 
extern Color grey80 ; 
extern Color grey81 ; 
extern Color grey82 ; 
extern Color grey83 ; 
extern Color grey84 ; 
extern Color grey85 ; 
extern Color grey86 ; 
extern Color grey87 ; 
extern Color grey88 ; 
extern Color grey89 ; 
extern Color grey90 ; 
extern Color grey91 ; 
extern Color grey92 ; 
extern Color grey93 ; 
extern Color grey94 ; 
extern Color grey95 ; 
extern Color grey96 ; 
extern Color grey97 ; 
extern Color grey98 ; 
extern Color grey99 ; 
extern Color grey100 ;
extern Color honeydew  ;       
extern Color honeydew1  ;      
extern Color honeydew2  ;      
extern Color honeydew3  ;      
extern Color honeydew4  ;      
extern Color hot_pink  ;       
extern Color HotPink ;
extern Color HotPink1  ;       
extern Color HotPink2  ;       
extern Color HotPink3  ;       
extern Color HotPink4  ;       
extern Color indian_red  ;     
extern Color IndianRed  ;      
extern Color IndianRed1  ;     
extern Color IndianRed2  ;     
extern Color IndianRed3  ;     
extern Color IndianRed4  ;     
extern Color ivory ;  
extern Color ivory1 ; 
extern Color ivory2 ; 
extern Color ivory3 ; 
extern Color ivory4 ; 
extern Color khaki ;  
extern Color khaki1 ; 
extern Color khaki2 ; 
extern Color khaki3 ; 
extern Color khaki4 ; 
extern Color lavender  ;       
extern Color lavender_blush  ; 
extern Color LavenderBlush  ;  
extern Color LavenderBlush1  ; 
extern Color LavenderBlush2  ; 
extern Color LavenderBlush3  ; 
extern Color LavenderBlush4  ; 
extern Color lawn_green  ;     
extern Color LawnGreen  ;      
extern Color lemon_chiffon  ;  
extern Color LemonChiffon  ;   
extern Color LemonChiffon1  ;  
extern Color LemonChiffon2  ;  
extern Color LemonChiffon3  ;  
extern Color LemonChiffon4  ;  
extern Color light_blue  ;     
extern Color light_coral  ;    
extern Color light_cyan  ;     
extern Color light_goldenrod  ;
extern Color light_goldenrod_yellow ;
extern Color light_gray  ;
extern Color light_green  ;    
extern Color light_grey  ;     
extern Color light_pink  ;     
extern Color light_salmon  ;   
extern Color light_sea_green  ;
extern Color light_sky_blue  ; 
extern Color light_slate_blue  ;     
extern Color light_slate_gray  ;    
extern Color light_slate_grey  ;   
extern Color light_steel_blue  ;
extern Color light_yellow  ;   
extern Color LightBlue  ;      
extern Color LightBlue1  ;     
extern Color LightBlue2  ;     
extern Color LightBlue3  ;     
extern Color LightBlue4  ;     
extern Color LightCoral  ;     
extern Color LightCyan  ;      
extern Color LightCyan1  ;     
extern Color LightCyan2  ;     
extern Color LightCyan3  ;     
extern Color LightCyan4  ;     
extern Color LightGoldenrod  ; 
extern Color LightGoldenrod1  ;
extern Color LightGoldenrod2  ;
extern Color LightGoldenrod3  ;
extern Color LightGoldenrod4  ;
extern Color LightGoldenrodYellow   ;
extern Color LightGray  ;      
extern Color LightGreen  ;     
extern Color LightGrey  ;      
extern Color LightPink  ;      
extern Color LightPink1  ;     
extern Color LightPink2  ;     
extern Color LightPink3  ;     
extern Color LightPink4  ;     
extern Color LightSalmon  ;    
extern Color LightSalmon1  ;   
extern Color LightSalmon2  ;   
extern Color LightSalmon3  ;   
extern Color LightSalmon4  ;   
extern Color LightSeaGreen  ;  
extern Color LightSkyBlue  ;   
extern Color LightSkyBlue1  ;  
extern Color LightSkyBlue2  ;  
extern Color LightSkyBlue3  ;  
extern Color LightSkyBlue4  ;  
extern Color LightSlateBlue  ; 
extern Color LightSlateGray  ; 
extern Color LightSlateGrey  ; 
extern Color LightSteelBlue  ; 
extern Color LightSteelBlue1  ;
extern Color LightSteelBlue2  ;
extern Color LightSteelBlue3  ;
extern Color LightSteelBlue4  ;
extern Color LightYellow  ;    
extern Color LightYellow1  ;   
extern Color LightYellow2  ;   
extern Color LightYellow3  ;   
extern Color LightYellow4  ;   
extern Color lime_green  ;     
extern Color LimeGreen  ;      
extern Color linen ;  
extern Color magenta ;
extern Color magenta1  ;       
extern Color magenta2  ;       
extern Color magenta3  ;       
extern Color magenta4  ;       
extern Color maroon ; 
extern Color maroon1 ;
extern Color maroon2 ;
extern Color maroon3 ;
extern Color maroon4 ;
extern Color medium_aquamarine    ;  
extern Color medium_blue  ;    
extern Color medium_orchid  ;  
extern Color medium_purple  ;  
extern Color medium_sea_green       ;
extern Color medium_slate_blue      ;
extern Color medium_spring_green    ;
extern Color medium_turquoise       ;
extern Color medium_violet_red      ;
extern Color MediumAquamarine       ;
extern Color MediumBlue  ; 
extern Color MediumOrchid  ;   
extern Color MediumOrchid1  ;  
extern Color MediumOrchid2  ;  
extern Color MediumOrchid3  ;  
extern Color MediumOrchid4  ;  
extern Color MediumPurple  ;   
extern Color MediumPurple1  ;  
extern Color MediumPurple2  ;  
extern Color MediumPurple3  ;  
extern Color MediumPurple4  ;  
extern Color MediumSeaGreen  ; 
extern Color MediumSlateBlue  ;
extern Color MediumSpringGreen ;     
extern Color MediumTurquoise  ;
extern Color MediumVioletRed  ;
extern Color midnight_blue  ;  
extern Color MidnightBlue  ;   
extern Color mint_cream  ;     
extern Color MintCream  ;      
extern Color misty_rose  ;     
extern Color MistyRose  ;      
extern Color MistyRose1  ;     
extern Color MistyRose2  ;     
extern Color MistyRose3  ;     
extern Color MistyRose4  ;     
extern Color moccasin  ;       
extern Color navajo_white  ;   
extern Color NavajoWhite  ;    
extern Color NavajoWhite1  ;   
extern Color NavajoWhite2  ;   
extern Color NavajoWhite3  ;   
extern Color NavajoWhite4  ;   
extern Color navy ;   
extern Color navy_blue  ;      
extern Color NavyBlue  ;       
extern Color old_lace  ;       
extern Color OldLace ;
extern Color olive_drab  ;     
extern Color OliveDrab  ;      
extern Color OliveDrab1  ;     
extern Color OliveDrab2  ;     
extern Color OliveDrab3  ;     
extern Color OliveDrab4  ;     
extern Color orange ; 
extern Color orange_red  ;     
extern Color orange1 ;
extern Color orange2 ;
extern Color orange3 ;
extern Color orange4 ;
extern Color OrangeRed  ;      
extern Color OrangeRed1  ;     
extern Color OrangeRed2  ;     
extern Color OrangeRed3  ;     
extern Color OrangeRed4  ;     
extern Color orchid ; 
extern Color orchid1 ;
extern Color orchid2 ;
extern Color orchid3 ;
extern Color orchid4 ;
extern Color pale_goldenrod  ; 
extern Color pale_green  ;     
extern Color pale_turquoise  ; 
extern Color pale_violet_red  ;
extern Color PaleGoldenrod  ;  
extern Color PaleGreen  ;      
extern Color PaleGreen1  ;     
extern Color PaleGreen2  ;     
extern Color PaleGreen3  ;     
extern Color PaleGreen4  ;     
extern Color PaleTurquoise  ;  
extern Color PaleTurquoise1  ; 
extern Color PaleTurquoise2  ; 
extern Color PaleTurquoise3  ; 
extern Color PaleTurquoise4  ; 

extern Color PaleVioletRed  ;  
extern Color PaleVioletRed1  ; 
extern Color PaleVioletRed2  ; 
extern Color PaleVioletRed3  ; 
extern Color PaleVioletRed4  ; 
extern Color papaya_whip  ;    
extern Color PapayaWhip  ;     
extern Color peach_puff  ;     
extern Color PeachPuff  ;      
extern Color PeachPuff1  ;     
extern Color PeachPuff2  ;     
extern Color PeachPuff3  ;     
extern Color PeachPuff4  ;     
extern Color peru ;   
extern Color pink ;   
extern Color pink1 ;  
extern Color pink2 ;  
extern Color pink3 ;  
extern Color pink4 ;  
extern Color plum ;   
extern Color plum1 ;  
extern Color plum2 ;  
extern Color plum3 ;  
extern Color plum4 ;  
extern Color powder_blue  ;    
extern Color PowderBlue  ;     
extern Color purple ; 
extern Color purple1 ;
extern Color purple2 ;
extern Color purple3 ;
extern Color purple4 ;
extern Color red ;   
extern Color red1 ;   
extern Color red2 ;   
extern Color red3 ;   
extern Color red4 ;   
extern Color rosy_brown  ;     
extern Color RosyBrown  ;      
extern Color RosyBrown1  ;     
extern Color RosyBrown2  ;     
extern Color RosyBrown3  ;     
extern Color RosyBrown4  ;     
extern Color royal_blue  ;     
extern Color RoyalBlue  ;      
extern Color RoyalBlue1  ;     
extern Color RoyalBlue2  ;     
extern Color RoyalBlue3  ;     
extern Color RoyalBlue4  ;     
extern Color saddle_brown  ;   
extern Color SaddleBrown  ;    
extern Color salmon ; 
extern Color salmon1 ;
extern Color salmon2 ;
extern Color salmon3 ;
extern Color salmon4 ;
extern Color sandy_brown  ;    
extern Color SandyBrown  ;     
extern Color sea_green  ;      
extern Color SeaGreen  ;       
extern Color SeaGreen1  ;      
extern Color SeaGreen2  ;      
extern Color SeaGreen3  ;      
extern Color SeaGreen4  ;      
extern Color seashell  ;       
extern Color seashell1  ;      
extern Color seashell2  ;      
extern Color seashell3  ;      
extern Color seashell4  ;      
extern Color sienna ; 
extern Color sienna1 ;
extern Color sienna2 ;
extern Color sienna3 ;
extern Color sienna4 ;
extern Color sky_blue  ;       
extern Color SkyBlue ;
extern Color SkyBlue1  ;       
extern Color SkyBlue2  ;       
extern Color SkyBlue3  ;       
extern Color SkyBlue4  ;       
extern Color slate_blue  ;     
extern Color slate_gray  ;     
extern Color slate_grey  ;     
extern Color SlateBlue  ;      
extern Color SlateBlue1  ;     
extern Color SlateBlue2  ;     
extern Color SlateBlue3  ;     
extern Color SlateBlue4  ;     
extern Color SlateGray  ;      
extern Color SlateGray1  ;     
extern Color SlateGray2  ;     
extern Color SlateGray3  ;     
extern Color SlateGray4  ;     
extern Color SlateGrey  ;      
extern Color snow ;   
extern Color snow1 ;  
extern Color snow2 ;  
extern Color snow3 ;  
extern Color snow4 ;  
extern Color spring_green  ;   
extern Color SpringGreen  ;    
extern Color SpringGreen1  ;   
extern Color SpringGreen2  ;   
extern Color SpringGreen3  ;   
extern Color SpringGreen4  ;   
extern Color steel_blue  ;     
extern Color SteelBlue  ;      
extern Color SteelBlue1  ;     
extern Color SteelBlue2  ;     
extern Color SteelBlue3  ;     
extern Color SteelBlue4  ;     
extern Color tan0 ;   
extern Color tan1 ;   
extern Color tan2 ;   
extern Color tan3 ;   
extern Color tan4 ;   
extern Color thistle ;
extern Color thistle1  ;       
extern Color thistle2  ;       
extern Color thistle3  ;       
extern Color thistle4  ;       
extern Color tomato ; 
extern Color tomato1 ;
extern Color tomato2 ;
extern Color tomato3 ;
extern Color tomato4 ;
extern Color turquoise  ;      
extern Color turquoise1  ;     
extern Color turquoise2  ;     
extern Color turquoise3  ;     
extern Color turquoise4  ;     
extern Color violet ; 
extern Color violet_red  ;     
extern Color VioletRed  ;      
extern Color VioletRed1  ;     
extern Color VioletRed2  ;     
extern Color VioletRed3  ;     
extern Color VioletRed4  ;     
extern Color wheat ;  
extern Color wheat1 ; 
extern Color wheat2 ; 
extern Color wheat3 ; 
extern Color wheat4 ; 
extern Color white ;  
extern Color white_smoke  ;    
extern Color WhiteSmoke  ;     
extern Color yellow ; 
extern Color yellow_green  ;   
extern Color yellow1 ;
extern Color yellow2 ;
extern Color yellow3 ;
extern Color yellow4 ;
extern Color YellowGreen  ;    

/*extern Color PORTABILITY ISSUES
 Mac OS X
extern Color     On Mac OS X, the following additional system colors are available 
(note that the actual color values depend on the currently active OS theme, 
and typically many of these will in fact be patterns rather than pure colors):
extern Color 
    systemTransparent
    systemBlack
    systemWhite
    systemHighlight
    systemPrimaryHighlightextern Color
    systemHighlightSecondary
    systemSecondaryHighlightextern Color
    systemHighlightAlternate
    systemAlternatePrimaryHighlightextern Color
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
