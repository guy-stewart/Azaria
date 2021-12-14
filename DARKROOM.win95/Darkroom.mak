# Microsoft Developer Studio Generated NMAKE File, Format Version 4.10
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=DarkRoom - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to DarkRoom - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "DarkRoom - Win32 Release" && "$(CFG)" !=\
 "DarkRoom - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Darkroom.mak" CFG="DarkRoom - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DarkRoom - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DarkRoom - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "DarkRoom - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "DarkRoom - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\temp\darkroom\Release"
# PROP Intermediate_Dir "\temp\darkroom\Release"
# PROP Target_Dir ""
OUTDIR=\temp\darkroom\Release
INTDIR=\temp\darkroom\Release

ALL : ".\Darkroom.exe" "$(OUTDIR)\Darkroom.pch"

CLEAN : 
	-@erase "$(INTDIR)\ChildFrm.obj"
	-@erase "$(INTDIR)\DarkRoom.obj"
	-@erase "$(INTDIR)\Darkroom.pch"
	-@erase "$(INTDIR)\DarkRoom.res"
	-@erase "$(INTDIR)\imgapi.obj"
	-@erase "$(INTDIR)\ImgDoc.obj"
	-@erase "$(INTDIR)\ImgView.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\Myfile.obj"
	-@erase "$(INTDIR)\OneDoc.obj"
	-@erase "$(INTDIR)\onefile.obj"
	-@erase "$(INTDIR)\OneView.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase ".\Darkroom.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /G5 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /c
# SUBTRACT CPP /YX /Yc /Yu
CPP_PROJ=/nologo /G5 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)/" /c 
CPP_OBJS=\temp\darkroom\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/DarkRoom.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Darkroom.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /out:"Darkroom.exe"
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Darkroom.pdb" /machine:I386 /out:"Darkroom.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ChildFrm.obj" \
	"$(INTDIR)\DarkRoom.obj" \
	"$(INTDIR)\DarkRoom.res" \
	"$(INTDIR)\imgapi.obj" \
	"$(INTDIR)\ImgDoc.obj" \
	"$(INTDIR)\ImgView.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\Myfile.obj" \
	"$(INTDIR)\OneDoc.obj" \
	"$(INTDIR)\onefile.obj" \
	"$(INTDIR)\OneView.obj" \
	"$(INTDIR)\StdAfx.obj"

".\Darkroom.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DarkRoom - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\temp\darkroom\Debug"
# PROP Intermediate_Dir "\temp\darkroom\Debug"
# PROP Target_Dir ""
OUTDIR=\temp\darkroom\Debug
INTDIR=\temp\darkroom\Debug

ALL : "$(OUTDIR)\Darkroom.exe" "$(OUTDIR)\Darkroom.bsc"\
 "$(OUTDIR)\Darkroom.pch"

CLEAN : 
	-@erase "$(INTDIR)\ChildFrm.obj"
	-@erase "$(INTDIR)\ChildFrm.sbr"
	-@erase "$(INTDIR)\DarkRoom.obj"
	-@erase "$(INTDIR)\Darkroom.pch"
	-@erase "$(INTDIR)\DarkRoom.res"
	-@erase "$(INTDIR)\DarkRoom.sbr"
	-@erase "$(INTDIR)\imgapi.obj"
	-@erase "$(INTDIR)\imgapi.sbr"
	-@erase "$(INTDIR)\ImgDoc.obj"
	-@erase "$(INTDIR)\ImgDoc.sbr"
	-@erase "$(INTDIR)\ImgView.obj"
	-@erase "$(INTDIR)\ImgView.sbr"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrm.sbr"
	-@erase "$(INTDIR)\Myfile.obj"
	-@erase "$(INTDIR)\Myfile.sbr"
	-@erase "$(INTDIR)\OneDoc.obj"
	-@erase "$(INTDIR)\OneDoc.sbr"
	-@erase "$(INTDIR)\onefile.obj"
	-@erase "$(INTDIR)\onefile.sbr"
	-@erase "$(INTDIR)\OneView.obj"
	-@erase "$(INTDIR)\OneView.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\Darkroom.bsc"
	-@erase "$(OUTDIR)\Darkroom.exe"
	-@erase "$(OUTDIR)\Darkroom.ilk"
	-@erase "$(OUTDIR)\Darkroom.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# SUBTRACT CPP /YX /Yc /Yu
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=\temp\darkroom\Debug/
CPP_SBRS=\temp\darkroom\Debug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/DarkRoom.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Darkroom.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ChildFrm.sbr" \
	"$(INTDIR)\DarkRoom.sbr" \
	"$(INTDIR)\imgapi.sbr" \
	"$(INTDIR)\ImgDoc.sbr" \
	"$(INTDIR)\ImgView.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\Myfile.sbr" \
	"$(INTDIR)\OneDoc.sbr" \
	"$(INTDIR)\onefile.sbr" \
	"$(INTDIR)\OneView.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\Darkroom.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/Darkroom.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/Darkroom.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ChildFrm.obj" \
	"$(INTDIR)\DarkRoom.obj" \
	"$(INTDIR)\DarkRoom.res" \
	"$(INTDIR)\imgapi.obj" \
	"$(INTDIR)\ImgDoc.obj" \
	"$(INTDIR)\ImgView.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\Myfile.obj" \
	"$(INTDIR)\OneDoc.obj" \
	"$(INTDIR)\onefile.obj" \
	"$(INTDIR)\OneView.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\Darkroom.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "DarkRoom - Win32 Release"
# Name "DarkRoom - Win32 Debug"

!IF  "$(CFG)" == "DarkRoom - Win32 Release"

!ELSEIF  "$(CFG)" == "DarkRoom - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "DarkRoom - Win32 Release"

!ELSEIF  "$(CFG)" == "DarkRoom - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DarkRoom.cpp
DEP_CPP_DARKR=\
	".\DarkRoom.h"\
	".\imgapi.h"\
	".\ImgDoc.h"\
	".\ImgView.h"\
	".\MainFrm.h"\
	".\Onedoc.h"\
	".\OneView.h"\
	".\StdAfx.h"\
	{$(INCLUDE)}"\one.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\XFile.h"\
	

!IF  "$(CFG)" == "DarkRoom - Win32 Release"


"$(INTDIR)\DarkRoom.obj" : $(SOURCE) $(DEP_CPP_DARKR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DarkRoom - Win32 Debug"


"$(INTDIR)\DarkRoom.obj" : $(SOURCE) $(DEP_CPP_DARKR) "$(INTDIR)"

"$(INTDIR)\DarkRoom.sbr" : $(SOURCE) $(DEP_CPP_DARKR) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "DarkRoom - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /G5 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Darkroom.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Darkroom.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DarkRoom - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /G5 /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/Darkroom.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\StdAfx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Darkroom.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MainFrm.cpp
DEP_CPP_MAINF=\
	".\DarkRoom.h"\
	".\MainFrm.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "DarkRoom - Win32 Release"


"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DarkRoom - Win32 Debug"


"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"

"$(INTDIR)\MainFrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ChildFrm.cpp
DEP_CPP_CHILD=\
	".\ChildFrm.h"\
	".\DarkRoom.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "DarkRoom - Win32 Release"


"$(INTDIR)\ChildFrm.obj" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DarkRoom - Win32 Debug"


"$(INTDIR)\ChildFrm.obj" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"

"$(INTDIR)\ChildFrm.sbr" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ImgDoc.cpp
DEP_CPP_IMGDO=\
	".\DarkRoom.h"\
	".\imgapi.h"\
	".\ImgDoc.h"\
	".\StdAfx.h"\
	{$(INCLUDE)}"\one.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\XFile.h"\
	

!IF  "$(CFG)" == "DarkRoom - Win32 Release"


"$(INTDIR)\ImgDoc.obj" : $(SOURCE) $(DEP_CPP_IMGDO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DarkRoom - Win32 Debug"


"$(INTDIR)\ImgDoc.obj" : $(SOURCE) $(DEP_CPP_IMGDO) "$(INTDIR)"

"$(INTDIR)\ImgDoc.sbr" : $(SOURCE) $(DEP_CPP_IMGDO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ImgView.cpp
DEP_CPP_IMGVI=\
	".\DarkRoom.h"\
	".\imgapi.h"\
	".\ImgDoc.h"\
	".\ImgView.h"\
	".\MainFrm.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "DarkRoom - Win32 Release"


"$(INTDIR)\ImgView.obj" : $(SOURCE) $(DEP_CPP_IMGVI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DarkRoom - Win32 Debug"


"$(INTDIR)\ImgView.obj" : $(SOURCE) $(DEP_CPP_IMGVI) "$(INTDIR)"

"$(INTDIR)\ImgView.sbr" : $(SOURCE) $(DEP_CPP_IMGVI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DarkRoom.rc
DEP_RSC_DARKRO=\
	".\RES\DIBDOC.ICO"\
	".\RES\DIBLOOK.ICO"\
	".\res\icon1.ico"\
	".\res\Toolbar.bmp"\
	

"$(INTDIR)\DarkRoom.res" : $(SOURCE) $(DEP_RSC_DARKRO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\imgapi.cpp
DEP_CPP_IMGAP=\
	".\imgapi.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "DarkRoom - Win32 Release"


"$(INTDIR)\imgapi.obj" : $(SOURCE) $(DEP_CPP_IMGAP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DarkRoom - Win32 Debug"


"$(INTDIR)\imgapi.obj" : $(SOURCE) $(DEP_CPP_IMGAP) "$(INTDIR)"

"$(INTDIR)\imgapi.sbr" : $(SOURCE) $(DEP_CPP_IMGAP) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Myfile.cpp
DEP_CPP_MYFIL=\
	".\imgapi.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "DarkRoom - Win32 Release"


"$(INTDIR)\Myfile.obj" : $(SOURCE) $(DEP_CPP_MYFIL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DarkRoom - Win32 Debug"


"$(INTDIR)\Myfile.obj" : $(SOURCE) $(DEP_CPP_MYFIL) "$(INTDIR)"

"$(INTDIR)\Myfile.sbr" : $(SOURCE) $(DEP_CPP_MYFIL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\OneDoc.cpp
DEP_CPP_ONEDO=\
	".\DarkRoom.h"\
	".\imgapi.h"\
	".\ImgDoc.h"\
	".\Onedoc.h"\
	".\StdAfx.h"\
	{$(INCLUDE)}"\one.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\XFile.h"\
	

!IF  "$(CFG)" == "DarkRoom - Win32 Release"


"$(INTDIR)\OneDoc.obj" : $(SOURCE) $(DEP_CPP_ONEDO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DarkRoom - Win32 Debug"


"$(INTDIR)\OneDoc.obj" : $(SOURCE) $(DEP_CPP_ONEDO) "$(INTDIR)"

"$(INTDIR)\OneDoc.sbr" : $(SOURCE) $(DEP_CPP_ONEDO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\OneView.cpp
DEP_CPP_ONEVI=\
	".\DarkRoom.h"\
	".\imgapi.h"\
	".\ImgDoc.h"\
	".\ImgView.h"\
	".\MainFrm.h"\
	".\Onedoc.h"\
	".\OneView.h"\
	".\StdAfx.h"\
	{$(INCLUDE)}"\one.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\XFile.h"\
	

!IF  "$(CFG)" == "DarkRoom - Win32 Release"


"$(INTDIR)\OneView.obj" : $(SOURCE) $(DEP_CPP_ONEVI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "DarkRoom - Win32 Debug"


"$(INTDIR)\OneView.obj" : $(SOURCE) $(DEP_CPP_ONEVI) "$(INTDIR)"

"$(INTDIR)\OneView.sbr" : $(SOURCE) $(DEP_CPP_ONEVI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\home\gas\Source\OneFile\onefile.cpp
DEP_CPP_ONEFI=\
	{$(INCLUDE)}"\one.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\XFile.h"\
	

!IF  "$(CFG)" == "DarkRoom - Win32 Release"


"$(INTDIR)\onefile.obj" : $(SOURCE) $(DEP_CPP_ONEFI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DarkRoom - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\onefile.obj" : $(SOURCE) $(DEP_CPP_ONEFI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\onefile.sbr" : $(SOURCE) $(DEP_CPP_ONEFI) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
