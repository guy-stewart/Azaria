; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=frames
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "DarkRoom.h"
LastPage=0

ClassCount=9
Class1=CDarkroomApp
Class2=CImgDoc
Class3=CImgView
Class4=CMainFrame

ResourceCount=8
Resource1=IDR_ONETYPE
Resource2=IDR_MAINFRAME
Resource3=IDR_DARKROTYPE
Class5=CAboutDlg
Class6=CChildFrame
Class7=COneDoc
Class8=COneView
Resource4=IDR_IMGTYPE (_MAC)
Resource5=IDR_MAINFRAME (_MAC)
Resource6=IDR_IMGTYPE
Class9=frames
Resource7=IDD_ABOUTBOX
Resource8=IDD_FRAMES

[CLS:CImgDoc]
Type=0
HeaderFile=ImgDoc.h
ImplementationFile=ImgDoc.cpp
Filter=N
LastObject=IDC_COMPRESSED
BaseClass=CDocument
VirtualFilter=DC

[CLS:CImgView]
Type=0
HeaderFile=ImgView.h
ImplementationFile=ImgView.cpp
Filter=C
LastObject=IDD_FLIPDIAG
BaseClass=CScrollView
VirtualFilter=VWC

[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=CMainFrame
BaseClass=CMDIFrameWnd
VirtualFilter=fWC


[CLS:CChildFrame]
Type=0
HeaderFile=ChildFrm.h
ImplementationFile=ChildFrm.cpp
Filter=M
LastObject=CChildFrame

[CLS:CAboutDlg]
Type=0
HeaderFile=DarkRoom.cpp
ImplementationFile=DarkRoom.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_PRINT_SETUP
Command4=ID_FILE_MRU_FILE1
Command5=ID_APP_EXIT
Command6=ID_VIEW_TOOLBAR
Command7=ID_VIEW_STATUS_BAR
Command8=ID_APP_ABOUT
CommandCount=8

[TB:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
CommandCount=8
Command8=ID_APP_ABOUT

[MNU:IDR_DARKROTYPE]
Type=1
Class=CImgView
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_PRINT
Command7=ID_FILE_PRINT_PREVIEW
Command8=ID_FILE_PRINT_SETUP
Command9=ID_FILE_MRU_FILE1
Command10=ID_APP_EXIT
Command11=ID_EDIT_UNDO
Command12=ID_EDIT_CUT
Command13=ID_EDIT_COPY
Command14=ID_EDIT_PASTE
CommandCount=21
Command15=ID_VIEW_TOOLBAR
Command16=ID_VIEW_STATUS_BAR
Command17=ID_WINDOW_NEW
Command18=ID_WINDOW_CASCADE
Command19=ID_WINDOW_TILE_HORZ
Command20=ID_WINDOW_ARRANGE
Command21=ID_APP_ABOUT

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[MNU:IDR_MAINFRAME (_MAC)]
Type=1
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_PRINT_SETUP
Command4=ID_FILE_MRU_FILE1
Command5=ID_APP_EXIT
Command6=ID_VIEW_TOOLBAR
Command7=ID_VIEW_STATUS_BAR
CommandCount=7

[ACL:IDR_MAINFRAME (_MAC)]
Type=1
Command1=ID_EDIT_COPY
Command2=ID_FILE_NEW
Command3=ID_FILE_OPEN
Command4=ID_FILE_PRINT
Command5=ID_APP_EXIT
Command6=ID_FILE_SAVE
Command7=ID_EDIT_PASTE
Command8=ID_NEXT_PANE
Command9=ID_PREV_PANE
Command10=ID_FILE_CLOSE
Command11=ID_EDIT_CUT
Command12=ID_EDIT_UNDO
CommandCount=12

[MNU:IDR_IMGTYPE]
Type=1
Class=CImgDoc
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_GENERATE_PORTVECTOR
Command7=ID_GEN_BTS
Command8=IDD_CUSTOM_ONE
Command9=IDC_COMPRESSED
Command10=ID_FILE_PRINT
Command11=ID_FILE_PRINT_PREVIEW
Command12=ID_FILE_PRINT_SETUP
Command13=ID_FILE_MRU_FILE1
Command14=ID_APP_EXIT
Command15=ID_EDIT_UNDO
Command16=ID_EDIT_CUT
Command17=ID_EDIT_COPY
Command18=ID_EDIT_PASTE
Command19=IDD_ANIMATION
Command20=IDD_SUROUND
Command21=IDD_FLIPDIAG
Command22=IDD_FLIPVERT
Command23=IDD_SETSIZE
Command24=ID_VIEW_TOOLBAR
Command25=ID_VIEW_STATUS_BAR
Command26=ID_WINDOW_NEW
Command27=ID_WINDOW_CASCADE
Command28=ID_WINDOW_TILE_HORZ
Command29=ID_WINDOW_ARRANGE
Command30=ID_APP_ABOUT
Command31=IDD_SET_FRAMES
CommandCount=31

[MNU:IDR_IMGTYPE (_MAC)]
Type=1
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_PRINT_SETUP
Command7=ID_FILE_PRINT
Command8=ID_FILE_PRINT_PREVIEW
Command9=ID_FILE_MRU_FILE1
Command10=ID_APP_EXIT
Command11=ID_EDIT_UNDO
Command12=ID_EDIT_CUT
Command13=ID_EDIT_COPY
Command14=ID_EDIT_PASTE
Command15=ID_VIEW_TOOLBAR
Command16=ID_VIEW_STATUS_BAR
Command17=ID_WINDOW_NEW
Command18=ID_WINDOW_CASCADE
Command19=ID_WINDOW_TILE_HORZ
CommandCount=19

[DLG:IDD_FRAMES]
Type=1
Class=frames
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_FRAMECOUNT,edit,1350639744
Control4=IDC_STATIC,static,1342308352
Control5=IDC_FRAMEPER10SECS,edit,1350639744
Control6=IDC_STATIC,static,1342308352

[CLS:frames]
Type=0
HeaderFile=imgview.h
ImplementationFile=imgview.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_FRAMEPER10SECS

[CLS:COneView]
Type=0
HeaderFile=oneview.h
ImplementationFile=oneview.cpp
BaseClass=CScrollView
LastObject=COneView
Filter=C
VirtualFilter=VWC

[CLS:COneDoc]
Type=0
HeaderFile=onedoc.h
ImplementationFile=onedoc.cpp
BaseClass=CDocument
Filter=N
VirtualFilter=DC
LastObject=COneDoc

[MNU:IDR_ONETYPE]
Type=1
Class=COneDoc
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_GENERATE_PORTVECTOR
Command7=ID_GEN_BTS
Command8=IDD_CUSTOM_ONE
Command9=IDC_COMPRESSED
Command10=ID_FILE_PRINT
Command11=ID_FILE_PRINT_PREVIEW
Command12=ID_FILE_PRINT_SETUP
Command13=ID_FILE_MRU_FILE1
Command14=ID_APP_EXIT
Command15=ID_EDIT_UNDO
Command16=ID_EDIT_CUT
Command17=ID_EDIT_COPY
Command18=ID_EDIT_PASTE
Command19=IDD_ANIMATION
Command20=IDD_SUROUND
Command21=IDD_FLIPDIAG
Command22=IDD_FLIPVERT
Command23=IDD_SETSIZE
Command24=ID_VIEW_TOOLBAR
Command25=ID_VIEW_STATUS_BAR
Command26=ID_WINDOW_NEW
Command27=ID_WINDOW_CASCADE
Command28=ID_WINDOW_TILE_HORZ
Command29=ID_WINDOW_ARRANGE
Command30=ID_APP_ABOUT
Command31=IDD_SET_FRAMES
CommandCount=31

[CLS:CDarkroomApp]
Type=0
HeaderFile=darkroom.h
ImplementationFile=darkroom.cpp
BaseClass=CWinApp
Filter=N
LastObject=ID_FILE_SAVE_AS

