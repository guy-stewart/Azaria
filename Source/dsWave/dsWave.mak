# Microsoft Developer Studio Generated NMAKE File, Format Version 4.10
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

!IF "$(CFG)" == ""
CFG=dsWave - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to dsWave - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dsWave - Win32 Release" && "$(CFG)" != "dsWave - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "dsWave.mak" CFG="dsWave - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dsWave - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "dsWave - Win32 Debug" (based on "Win32 (x86) Static Library")
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
CPP=cl.exe

!IF  "$(CFG)" == "dsWave - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\dsWave.lib"

CLEAN : 
	-@erase "$(INTDIR)\dsWave.obj"
	-@erase "$(INTDIR)\dsWaveIO.obj"
	-@erase "$(INTDIR)\dsWaveTm.obj"
	-@erase "$(OUTDIR)\dsWave.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/dsWave.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dsWave.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/dsWave.lib" 
LIB32_OBJS= \
	"$(INTDIR)\dsWave.obj" \
	"$(INTDIR)\dsWaveIO.obj" \
	"$(INTDIR)\dsWaveTm.obj"

"$(OUTDIR)\dsWave.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dsWave - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\dsWave.lib"

CLEAN : 
	-@erase "$(INTDIR)\dsWave.obj"
	-@erase "$(INTDIR)\dsWaveIO.obj"
	-@erase "$(INTDIR)\dsWaveTm.obj"
	-@erase "$(OUTDIR)\dsWave.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/dsWave.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dsWave.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/dsWave.lib" 
LIB32_OBJS= \
	"$(INTDIR)\dsWave.obj" \
	"$(INTDIR)\dsWaveIO.obj" \
	"$(INTDIR)\dsWaveTm.obj"

"$(OUTDIR)\dsWave.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
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

# Name "dsWave - Win32 Release"
# Name "dsWave - Win32 Debug"

!IF  "$(CFG)" == "dsWave - Win32 Release"

!ELSEIF  "$(CFG)" == "dsWave - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\dsWaveTm.cpp
DEP_CPP_DSWAV=\
	".\dsConst.h"\
	{$(INCLUDE)}"\dsound.h"\
	{$(INCLUDE)}"\dsWave.h"\
	

"$(INTDIR)\dsWaveTm.obj" : $(SOURCE) $(DEP_CPP_DSWAV) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dsWaveIO.cpp
DEP_CPP_DSWAVE=\
	".\dsConst.h"\
	{$(INCLUDE)}"\dsound.h"\
	{$(INCLUDE)}"\dsWave.h"\
	

"$(INTDIR)\dsWaveIO.obj" : $(SOURCE) $(DEP_CPP_DSWAVE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dsWave.cpp
DEP_CPP_DSWAVE_=\
	".\dsConst.h"\
	{$(INCLUDE)}"\dsound.h"\
	{$(INCLUDE)}"\dsWave.h"\
	

"$(INTDIR)\dsWave.obj" : $(SOURCE) $(DEP_CPP_DSWAVE_) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
