//
//  adrenaline_creator.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 6/29/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#include "adrenaline_creator.hpp"
//#include "adrenaline_windows.hpp"


void* create_instance( int mAdrenalineObject_id )
{
    void* ptr = NULL;
    switch(mAdrenalineObject_id)
    {
/*        case 1 : ptr = new Button();
            break;
        case 2 : ptr = new Control();
            break;
        case 3 : ptr = new Bitmap();
            break;
        case 4 : ptr = new CAN_Msg_View();
            break;
        case 5 : ptr = new Checkbox();
            break;
        case 6 : ptr = new DirectoryListBox();
            break;
        case 7 : ptr = new DropDownBox();
            break;
        case 8 : ptr = new EditBox();
            break;
        case 9 : ptr = new FrameWindow();
            break;
        case 10 : ptr = new GroupBox();
            break;
        case 11 : ptr = new GyroView();
            break;
        case 12 : ptr = new HorizontalMenu();
            break;
        case 13 : ptr = new IconView();
            break;
        case 14 : ptr = new Leveler();
            break;
        case 15 : ptr = new VerticalLineGroup();
            break;
        case 16 : ptr = new IconListBox();
            break;
        case 17 : ptr = new ListBox();
            break;
        case 18 : ptr = new Map2D();
            break;
        case 19 : ptr = new MarqueeView();
            break;
        case 20 : ptr = new MultiListBox();
            break;
        case 21 : ptr = new PowerLevel();
            break;
        case 22 : ptr = new ProgressBar();
            break;
        case 23 : ptr = new RadioButton();
            break;
        case 24 : ptr = new Rectangle();
            break;
        case 25 : ptr = new ScrollBar();
            break;
        case 26 : ptr = new ScrollControl();
            break;
        case 27 : ptr = new SliderControl();
            break;
            
        case 28 : ptr = new SpinnerControl();
            break;
        case 29 : ptr = new StickLeg();
            break;
        case 30 : ptr = new TabControl();
            break;
        case 31 : ptr = new TableWindow();
            break;
        case 32 : ptr = new TabularListBox();
            break;
        case 33 : ptr = new TextView();
            break;
        case 34 : ptr = new TextFile();
            break;
        case 35 : ptr = new VerticalMenu();
            break;
        case 36 : ptr = new WaveView();
            break;
        case 37 : ptr = new Wave();
            break;
        case 38 : ptr = new Window();
            break;

        case 39 : ptr = new StackedBarGraph();
            break;
        case 40 : ptr = new BarGraph();
            break;
        case 41 : ptr = new DataSet();
            break;
        case 42 : //ptr = new Spectogram();
            break;

        case 43 : ptr = new Graphbase();
            break;
        case 44 : ptr = new Histogram();
            break;
        case 45 : ptr = new LineGraph();
            break;
        case 46 : ptr = new ScatterGraph();
            break;
            
        case 47 : ptr = new NetworkView();
            break;
        case 48 : ptr = new Application();
            break;
        case 49 : //ptr = new AudioControl();
            break;

        case 50 : ptr = new ButtonArray();
            break;
        case 51 : ptr = new Calendar();
            break;
        case 52 : ptr = new CANMessages();
            break;
        case 53 : ptr = new FileBrowser();
            break;
        case 54 : ptr = new FileEditor();
            break;
        case 55 : ptr = new FileOpen();
            break;
        case 56 : ptr = new FlowChart();
            break;
        case 57 : ptr = new FrequencyView();
            break;
        case 58 : ptr = new IconBar();
            break;
        case 59 : ptr = new ImageGallery();
            break;
            
        case 60 : ptr = new LabelEditBox();
            break;
        case 61 : ptr = new StatusList();
            break;
        case 62 : ptr = new SideBar();
            break;
        case 63 : ptr = new StereoPowerLevels();
            break;
        case 64 : ptr = new StickFigure();
            break;
        case 65 : ptr = new SystemBar();
            break;
        case 66 : ptr = new DisplayManager();
            break;
        case 67 : ptr = new Keyboard();
            break;
        case 68 : //ptr = new main_window();
            break;
        case 69 : //ptr = new mouse();
            break;
            
        case 70 : ptr = new resource_manager();
            break;
        case 71 : ptr = new SystemStatusBar();
            break;
        case 72 : ptr = new DraggerGesture();
            break; */
        default:
            break;
    }
    return ptr;
}

