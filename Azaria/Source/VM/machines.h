
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//****************************************************************************
//
//  MACHINE IMPLEMENTATIONS.
//
#define THE_BARD		0x0001
#define THE_EARTH		0x0002
#define THE_FIRE		0x0003
#define THE_WIND		0x0004
#define THE_WATER		0x0005

#define THE_FISH		0x0006
#define THE_BIRDS		0x0007

#define SMP_VIAL		0x0010
#define SMP_BACKBUTTON	0x0011
#define SMP_MAPBUTTON	0x0012
#define SMP_COMPASS		0x0013
#define SMP_INV_LEFT	0x0014
#define SMP_INV_RIGHT	0x0015
#define SMP_TEXT_UP		0x0016
#define SMP_TEXT_DOWN	0x0017
#define SMP_LISTEN1		0x0018
#define SMP_LISTEN2		0x0019
#define SMP_LISTEN3		0x001A
#define SMP_LISTEN4		0x001B
#define SMP_TALK1		0x001C
#define SMP_TALK2		0x001D
#define SMP_TALK3		0x001E
#define SMP_TALK4		0x001F

#define S00_BTN_QUIT	0x0020
#define S00_BTN_CREDITS	0x0021
#define S00_BTN_AUDIO	0x0022
#define S00_BTN_PLAYER	0x0023
#define S00_BTN_GAME	0x0024
#define S00_BTN_NETWORK	0x0025
#define S00_BTN_HELP	0x0026

#define SID_DEC_WISDOM		0x0030
#define SID_INC_WISDOM		0x0031
#define SID_DEC_STRENGTH	0x0032
#define SID_INC_STRENGTH	0x0033
#define SID_DEC_ENERGY		0x0034
#define SID_INC_ENERGY		0x0035
#define SID_DEC_KARMA		0x0036
#define SID_INC_KARMA		0x0037
#define SID_DEC_WEALTH		0x0038
#define SID_INC_WEALTH		0x0039

//-------------------------------------------
// Machine instances for local ID

#define SID_HALO	0x0040	
#define SID_SPELL	0x0041
#define SID_ID		0x0042
#define SID_PED		0x0043
#define SID_AURA	0x0044
#define SID_BADGE00	0x0045
#define SID_BADGE01	0x0046

#define SOD_SPELL	0x0047
#define SOD_ID		0x0048
#define SOD_PED		0x0049
#define SOD_AURA	0x004A // ## This is referenced in GameMan.cpp
#define SOD_BADGE00	0x004B
#define SOD_BADGE01	0x004C
#define SOD_HALO	0x004D	

//-------------------------------------------
// Scene 2 machines:
#define S02_EBIN__00	0x0200
#define S02_UNIT__01	0x0200 + 3*9

#define S02_VIAL__XX	0x0000
#define S02_LOCK__XX	0x0001
#define S02_BIN___XX	0x0002
#define S02_UP1___XX	0x0003
#define S02_UP10__XX	0x0004
#define S02_DN1___XX	0x0005
#define S02_DN10__XX	0x0006
#define S02_NUM1__XX	0x0007
#define S02_NUM10_XX	0x0008
#define S02_MACHINES_PER_UNIT 0x0009

// Scene 3 machines:
// WARNING: scene 3 is overrun by scene 2.

// Scene 4 machines:
#define S04_VIAL		0x0400
#define S04_BIN			0x0401

#define S04_KEYCLAMP_1	0x0402
#define S04_REDSLIDER_1	0x0403
#define S04_GRNSLIDER_1	0x0404
#define S04_BLUSLIDER_1	0x0405
#define S04_KEYCLAMP_2	0x0406
#define S04_REDSLIDER_2	0x0407
#define S04_GRNSLIDER_2	0x0408
#define S04_BLUSLIDER_2	0x0409
#define S04_KEYCLAMP_3	0x040a
#define S04_REDSLIDER_3	0x040b
#define S04_GRNSLIDER_3	0x040c
#define S04_BLUSLIDER_3	0x040d

// Scene 5 machines:
#define S05_UP			0x0501
#define S05_DOWN		0x0502
#define S05_ICON		0x0503
#define S05_NUM1		0x0504
#define S05_NUM10		0x0505
#define S05_VIAL		0x0506
#define S05_CHEM		0x0507
#define S05_BUBBLE		0x0508

// Scene 6 machines.
#define S06_TRI1		0x0600
#define S06_TRI2		0x0601
#define S06_TRI3		0x0602
#define S06_TRI4		0x0603
#define S06_TEMPHOLD	0x0604
#define S06_LAST_MON	0x0636 //0x0614 +30 

#define S06_PEZPOP		0x0640
// .... 35 PEZPOP machine instances ...
#define S06_PEZPOP_LAST	0x0663

// Scene 08: Manufacturing:
#define INSTS_PER_MANREP 4
#define NUMBER_OF_MANREPS 10
#define S08_TEMPHOLD	0x0800
#define S08_HOPPER		0x0801
#define S08_BEEM		0x0802
#define S08_BIN			0x0803
// ......
#define S08_LAST_MAN	0x0800 + INSTS_PER_MANREP * (NUMBER_OF_MANREPS+1)

// Scene 10 machines:
#define S10_DRYPIT		0x1000
#define S10_FLOWER		0x1001
#define S10_FOPEN		0x1002
#define S10_FCLOSE		0x1003

// Scene 11 machines:
#define S11_FIRE		0x1110
#define S11_GRILL		0x1111
#define S11_ASCENT		0x1112

#define S11_BAITSTATION		0x1120
#define S11_POLESTANDa		0x1121
#define S11_POLESTANDb		0x1122
#define S11_POLESTANDc		0x1123 
#define S11_POLESTANDd		0x1124

#define S11_FISHSTATIONa	0x1130
#define S11_FISHSTATIONb	0x1131
#define S11_FISHSTATIONc	0x1132
#define S11_FISHSTATIONd	0x1133
#define S11_FISHSTATIONe	0x1134
#define S11_FISHSTATIONf	0x1135
#define S11_FISHSTATIONg	0x1136
#define S11_FISHSTATIONh	0x1137

#define S11_LOGBINA			0x1150
#define S11_LOGBINB			0x1151

#define S11_GRUBBIN0		0x1160

#define S11_BUCKETS			0x1161

#define S12_SCROLL01		0x1201
#define S12_SCROLL02		0x1202
#define S12_SCROLL03		0x1203
#define S12_SCROLL04		0x1204
#define S12_SCROLL05		0x1205
#define S12_SCROLL06		0x1206
#define S12_SCROLL07		0x1207
#define S12_SCROLL08		0x1208
#define S12_SCROLL09		0x1209
#define S12_SCROLL10		0x120a
#define S12_SCROLL11		0x120b
#define S12_SCROLL12		0x120c
#define S12_SCROLL13		0x120d
#define S12_SCROLL14		0x120e
#define S12_SCROLL15		0x120f
#define S12_SCROLL16		0x1210
#define S12_SCROLL17		0x1211
#define S12_SCROLL18		0x1212
#define S12_SCROLL19		0x1213
#define S12_SCROLL20		0x1214

#define S12_TCANDLE1		0x1222
#define S12_TCANDLE2		0x1223
#define S12_TCANDLE3		0x1224
#define S12_TCANDLE4		0x1225

#define S12_SPRING1			0x1226
#define S12_WATER1			0x1227
#define S12_CANDLE1			0x1228
#define S12_ASHSHELF1		0x1229
#define S12_SPRING2			0x122a
#define S12_WATER2			0x122b
#define S12_CANDLE2			0x122c
#define S12_ASHSHELF2		0x122d

#define S12_aCANDLE			0x1240
#define S12_aSCROLL			0x1241
#define S12_aPLANT1			0x1242
#define S12_aPLANT2			0x1243
#define S12_aPLANT3			0x1244
#define S12_aFISH1			0x1245
#define S12_aFISH2			0x1246
#define S12_aFISH3			0x1247
#define S12_aNYHOPPER		0x1248

#define S17_aLOCKSOCKET		0x1700
#define S17_aLOCKBURN		0x1701
#define S17_aDOORWAY		0x1702
#define S17_aFFIELD1		0x1703
#define S17_aFFIELD2		0x1704
#define S17_aFFIELD3		0x1705
#define S17_aMINE			0x1706
#define S17_aFOULWIND		0x1707

#define S17_bLOCKSOCKET		0x1708
#define S17_bLOCKBURN		0x1709
#define S17_bDOORWAY		0x170a
#define S17_bFFIELD1		0x170b
#define S17_bFFIELD2		0x170c
#define S17_bFFIELD3		0x170d
#define S17_bMINE			0x170e
#define S17_bFOULWIND		0x170f

#define S17_cLOCKSOCKET		0x1710
#define S17_cLOCKBURN		0x1711
#define S17_cDOORWAY		0x1712
#define S17_cFFIELD1		0x1713
#define S17_cFFIELD2		0x1714
#define S17_cFFIELD3		0x1715
#define S17_cMINE			0x1716
#define S17_cFOULWIND		0x1717

#define S17_dLOCKSOCKET		0x1718
#define S17_dLOCKBURN		0x1719
#define S17_dDOORWAY		0x171a
#define S17_dFFIELD1		0x171b
#define S17_dFFIELD2		0x171c
#define S17_dFFIELD3		0x171d
#define S17_dMINE			0x171e
#define S17_dFOULWIND		0x171f

#define S17_eLOCKSOCKET		0x1720
#define S17_eLOCKBURN		0x1721
#define S17_eDOORWAY		0x1722
#define S17_eFFIELD1		0x1723
#define S17_eFFIELD2		0x1724
#define S17_eFFIELD3		0x1725
#define S17_eMINE			0x1726
#define S17_eFOULWIND		0x1727

// Scene 21: Pickin' and a Grinin'
#define S21_l1BERRYTREE		0x2100
#define S21_m1BERRYTREE		0x2100
#define S21_m2BERRYTREE		0x2100
#define S21_m3BERRYTREE		0x2100
#define S21_m4BERRYTREE		0x2100
#define S21_m5BERRYTREE		0x2100
#define S21_m6BERRYTREE		0x2100

#define S21_o1BERRYTREE		0x2100
#define S21_o2BERRYTREE		0x2100
#define S21_o3BERRYTREE		0x2100
#define S21_o4BERRYTREE		0x2100
#define S21_o5BERRYTREE		0x2100
#define S21_o6BERRYTREE		0x2100

#define S21_p1BERRYTREE		0x2100
#define S21_p2BERRYTREE		0x2100
#define S21_p3BERRYTREE		0x2100
#define S21_p4BERRYTREE		0x2100

// Scene 22: Scroll me
#define S22_SCROLLMAN		0x2200
#define S22_SHIELD			0x2201
#define S22_WORD			0x2202
#define S22_PLANT1			0x2203
#define S22_PLANT2			0x2204
#define S22_PLANT3			0x2205
#define S22_FISH1			0x2206
#define S22_FISH2			0x2207
#define S22_FISH3			0x2208
#define S22_NYSTROM			0x2209

// Scene 24: Eye o' God
#define S24_EYEPLATE1		0x2400
#define S24_EYEPLATE2		0x2401
#define S24_EYEPLATE3		0x2402

#define S24_EYESPELL1		0x2403
#define S24_EYESPELL2		0x2404
#define S24_EYESPELL3		0x2405

#define S24_BIGEYE			0x2406
#define S24_EYETEXT			0x2407

//-------------------------------------------
// Scene 25: Rock & Roll in the Hidden Cave:
#define S25_RUMBLE			0x2500
#define S25_ROLL			0x2501
#define S25_SCATTER			0x2502

#define S25_WALLCU1E		0x2503
#define S25_WALLCU2E		0x2504
#define S25_OPN1			0x2505
#define S25_OPN2			0x2506

//----------------------------
// Scene 26:
#define S26_BANISH			0x2600
#define S26_BAN1			0x2601
#define S26_BAN2			0x2602
#define S26_BAN3			0x2603
#define S26_BAN4			0x2604

//-------------------------------------------
// Checker/Chess board:

// white chess piece bins:
#define S00_WROOK	0x7000
#define S00_WKNIGHT	0x7001
#define S00_WBISHOP	0x7002
#define S00_WQUEEN	0x7003
#define S00_WKING	0x7004
#define S00_WPAWN	0x7005
#define S00_WKINGME	0x7006
// black chess piece bins:
#define S00_BROOK	0x7007
#define S00_BKNIGHT	0x7008
#define S00_BBISHOP	0x7009
#define S00_BQUEEN	0x700A
#define S00_BKING	0x700B
#define S00_BPAWN	0x700C
#define S00_BKINGME	0x700D

#define S00_CLEAR	0x7010
#define S00_BSETUP	0x7011
#define S00_WSETUP	0x7012

#define S00_REVERSIA 0x7020
#define S00_REVERSIB 0x7021
#define S00_GOBIN_B  0x7022 // butterfly.
#define S00_GOBIN_D  0x7023 // dragon

// 1st square:
#define S00_SQUARE00	0x7100
// ... rest of board:RESERVED RANGE !!!
#define S00_SQUARE48	0x7100 + 48
#define S00_SQUARE56	0x7100 + 56
// 64th square
#define S00_SQUARE63	0x713F

//----------------------------------------------------------------------------
//****************************************************************************
// machine definitions:
#define M_BARD		0x0001
#define M_EARTH		0x0002
#define M_FIRE		0x0003
#define M_WIND		0x0004
#define M_WATER		0x0005
#define M_FISH		0x0006
#define M_BIRDS		0x0007

// INTERFACE MACHINES:
#define M_VIAL			0x0010
#define M_BACKBUTTON	0x0011
#define M_MAPBUTTON		0x0012
#define M_COMPASS		0x0013
#define M_INV_LEFT		0x0014
#define M_INV_RIGHT		0x0015
#define M_TEXT_UP		0x0016
#define M_TEXT_DOWN		0x0017
#define M_LISTEN		0x0018
//#define M_LISTEN2		0x0019
//#define M_LISTEN3		0x001A	// m_listen is reused in place of these.
//#define M_LISTEN4		0x001B
#define M_TALK			0x001C

#define M_DEC_WISDOM	0x0030
#define M_INC_WISDOM	0x0031
#define M_DEC_STRENGTH	0x0032
#define M_INC_STRENGTH	0x0033
#define M_DEC_ENERGY	0x0034
#define M_INC_ENERGY	0x0035
#define M_DEC_KARMA		0x0036
#define M_INC_KARMA		0x0037
#define M_DEC_WEALTH	0x0038
#define M_INC_WEALTH	0x0039

#define M_IDSPELL		0x0040
#define M_ID			0x0041
#define M_PED			0x0042
#define M_AURA			0x0043
#define M_BADGE00		0x0044
#define M_HALO			0x0046

#define M_O_IDSPELL		0x0047
#define M_O_ID			0x0048
#define M_O_PED			0x0049
#define M_O_AURA		0x004A
#define M_O_BADGE00		0x004B
#define M_O_HALO		0x004D

#define M_OBJECTBIN		0x0050
#define M_PLANTBIN		0x0051
#define M_OBJECTMAP		0x0052  // map a drag'd object to something surprisingly new!
#define M_BTN_1phase	0x0053
#define M_BTN_2phase	0x0054

//----------------------------
// Scene 2 machines:
#define M02_VIAL		0x0200
#define M02_LOCK		0x0201
#define M02_BIN			0x0202
#define M02_EBIN		0x0203
#define M02_UP1			0x0204
#define M02_UP10		0x0205
#define M02_DN1			0x0206
#define M02_DN10		0x0207
#define M02_NUM1		0x0208
#define M02_NUM10		0x0209

// scene 2 runs over into scene 3 ...
// Scene 3:
//
// WARNING: scene 3 is overrun with scene 2 machines.

//----------------------------
// Scene 4 machines:
#define M04_KEYCLAMP	0x0400
#define M04_SLIDER		0x0401

#define M04_VIAL		0x0402
#define M04_BIN			0x0403

//----------------------------
// Scene 5 machines:
#define M05_UP			0x0501
#define M05_DOWN		0x0502
#define M05_ICON		0x0503
#define M05_NUM1		0x0504
#define M05_NUM10		0x0505
#define M05_VIAL		0x0506
#define M05_CHEM		0x0507
#define M05_BUBBLE		0x0508

//----------------------------
// Scene 6 machines:
#define M06_PEZPOP		0x0600
#define M06_TRI1		0x0610
#define M06_TRI2		0x0611
#define M06_TRI3		0x0612
#define M06_TRI4		0x0613
#define M06_TEMPHOLD	0x0614

//----------------------------
// Scene 8 manufacturing:
#define M08_TEMPHOLD	0x0800
#define M08_HOPPER		0x0801
#define M08_BEEM		0x0802
#define M08_BIN			0x0803

//----------------------------
// Scene 10 machines:
#define M10_DRYPIT		0x1000
#define M10_FLOWER		0x1001
#define M10_FOPEN		0x1002
#define M10_FCLOSE		0x1003

//----------------------------
// Scene 11 machines:
#define M11_FIRE		0x1110
#define M11_GRILL		0x1111
#define M11_ASCENT		0x1112

#define M_BAITSTATION	0x1120
#define M_FISHSTATION	0x1121
#define M_POLESTAND		0x1122

#define M12_WATER		0x1206
#define M12_CANDLE		0x1207
#define M12_ASHSHELF	0x1208
#define M12_SPRING		0x1209

#define M12_xCANDLE			0x1230
#define M12_xSCROLL			0x1231
#define M12_xPLANT			0x1232
#define M12_xFISH	  		0x1233
#define M12_xNYSTROM		0x1234

#define M17_LOCKSOCKET		0x1700
#define M17_LOCKBURN		0x1701
#define M17_DOORWAY			0x1702
#define M17_FFIELD			0x1703
#define M17_MINE			0x1704
#define M17_FOULWIND		0x1705

//----------------------------
// Scene 22: Scroll me
#define M22_SCROLLMAN		0x2200
#define M22_SHIELD			0x2201
#define M22_WORD			0x2202
#define M22_PLANT1			0x2203
#define M22_PLANT2			0x2204
#define M22_PLANT3			0x2205
#define M22_FISH1			0x2206
#define M22_FISH2			0x2207
#define M22_FISH3			0x2208
#define M22_NYSTROM			0x2209

//----------------------------
// Scene 24: Eye o' God
#define M24_EYEPLATE		0x2400
#define M24_EYESPELL		0x2403
#define M24_BIGEYE			0x2406
#define M24_EYETEXT			0x2407

//----------------------------
// Scene 25: Rock & Roll at the Hidden Cave
#define M25_RUMBLE			0x2500
#define M25_ROLL			0x2501
#define M25_SCATTER			0x2502

#define M25_OPNDOOR			0x2505


//----------------------------
// Scene 26:
#define M26_BANISH			0x2600

//----------------------------
// checker board:
#define M_SQUARE		0x7000

#define M_WROOK			0x7001
#define M_WKNIGHT		0x7002
#define M_WBISHOP		0x7003
#define M_WQUEEN		0x7004
#define M_WKING			0x7005
#define M_WPAWN			0x7006
#define M_WKINGME		0x7007

#define M_BROOK			0x7008
#define M_BKNIGHT		0x7009
#define M_BBISHOP		0x700A
#define M_BQUEEN		0x700B
#define M_BKING			0x700C
#define M_BPAWN			0x700D
#define M_BKINGME		0x700E

#define M_CLEAR			0x700F	// setup the chess board.
#define M_CHESS			0x7010	// setup the chess board.

