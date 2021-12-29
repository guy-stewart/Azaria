

//----------------------------------------------------------------------------
// view ids
#define IDV_MAIN_PANEL	0x0001
#define IDV_INVENTORY	0x0002
#define IDV_ID			0x0003
#define IDV_MENUSYSTEM	0x0004
#define IDV_OTHERID		0x0005
#define IDV_COMPASS		0x0007
#define IDV_S_TELETYPE	0x0008
#define IDV_L_TELETYPE	0x0009

#define IDV_PLAYERNAME	0x000a
#define IDV_OTHERNAME	0x000b

#define IDV_TOPMENU		0x0020
#define IDV_CFGPLAYER1	0x0021
#define IDV_CFGPLAYER2	0x0022
#define IDV_CFGPROV		0x0023
#define IDV_CFGSESS		0x0024
#define IDV_CFGNEWSESS	0x0025
#define IDV_CFGGAME		0x0026
#define IDV_CFGAV		0x0027
#define IDV_HELP		0x0028
#define IDV_CREDITS		0x0029

#define IDV_NOTE		0x0050

#define IDV_CONTINENT	0x0060
#define IDV_V1PAN		0x0065
#define IDV_V2PAN		0x0066
#define IDV_GRID		0x0067
#define IDV_TESTLENS	0x0068

// Scene 2:
#define IDV_LOCKERR	0x0200
#define IDV_LOCKERN	0x0201
#define IDV_LOCKERW	0x0202
#define IDV_LOCKERS	0x0203

// locker closeup views. (9*sets) - sets = 3
#define IDV_LOCKERCU	0x0204
//#define IDV_LOCKERCU+1	0x0203
//#define IDV_LOCKERCU+2	0x0204
#define IDV_LOCKERCU26	IDV_LOCKERCU + 0x1A

//city surround views 0220 - 0249

#define IDV_ctya		0x0220
#define IDV_ctyb		0x0221
#define IDV_ctyc		0x0222
#define IDV_ctyd		0x0223
#define IDV_ctye		0x0224 
#define IDV_ctyf		0x0225
#define IDV_ctyg		0x0226
#define IDV_ctyh		0x0227
#define IDV_ctyi		0x0228
#define IDV_ctyj		0x0229
#define IDV_ctyk		0x022A
#define IDV_ctyl		0x022B
#define IDV_ctym		0x022C
#define IDV_ctyn		0x022D
#define IDV_ctyo		0x022E
#define IDV_ctyp		0x022F
#define IDV_ctyq		0x0230
#define IDV_ctyr		0x0231
#define IDV_ctys		0x0232
#define IDV_ctyt		0x0233

// Scene 04: City locksmith.
//city locksmith reserved 0240 -0249

#define IDV_ctysmith	0x0240
#define IDV_ckeycf01	0x0241
#define IDV_ckeycf02	0x0242
#define IDV_ckeycf03	0x0243
#define IDV_ckeycf04	0x0244
#define IDV_ckeycf05	0x0245
#define IDV_ckeycf06	0x0246
#define IDV_ckeyvend	0x0247


//city housing building 0250 - 0259

#define IDV_ctyhouse	0x0250
#define IDV_chouse01	0x0251
#define IDV_chouse02	0x0252
#define IDV_chouse03	0x0253
//city housing interiors
//#define IDV_citH1ina	0x0253
//#define IDV_citH1inb	0x0253
//#define IDV_citH1inc	0x0253
//#define IDV_citH2ina	0x0253
//#define IDV_citH2inb	0x0253
//#define IDV_citH2inc	0x0253
//#define IDV_citH3ina	0x0253
//#define IDV_citH3inb	0x0253
//#define IDV_citH3inc	0x0253
//#define IDV_citdoor1	0x0253
//city house doors
//#define IDV_citH1dra	0x0253
//#define IDV_citH1drb	0x0253
//#define IDV_citH1drc	0x0253
//#define IDV_citH2dra	0x0253
//#define IDV_citH2drb	0x0253
//#define IDV_citH2drc	0x0253
//#define IDV_citH3dra	0x0253
#define IDV_citH3drb	0x0253
#define IDV_citH3drc	0x0253

//city library buildings 0260 - 0289

#define IDV_ctychem	0x0500
#define IDV_chempan	0x0501

// scene 06:The library, and PEZ hell:
#define IDV_CTYLIB1A	0x0600
#define IDV_CTYLIB1B	0x0601
#define IDV_CTYLIB2A	0x0602
#define IDV_CTYLIB2B	0x0603

#define IDV_CLIBMON1	0x0604
#define IDV_CLIBMON2	0x0605
#define IDV_CLIBMON3	0x0606
#define IDV_CLIBMON4	0x0607
#define IDV_CLIBMON5	0x0608
#define IDV_CLIBMON6	0x0609
#define IDV_CLIBMUS1	0x0610
#define IDV_CLIBMUS2	0x0611

#define IDV_CTYPEZ1		0x0620
#define IDV_CTYPEZ2		0x0621
#define IDV_CTYPEZ3		0x0622
#define IDV_CTYPEZ4		0x0623
#define IDV_CTYPEZ5		0x0624
#define IDV_CTYPEZ6		0x0625
#define IDV_CTYPEZ7		0x0626

// scene 7
#define IDV_ctytrade	0x0700


// Scene 8:
#define IDV_MANPAN		0x0800
#define IDV_MANREP1		0x0801
#define IDV_MANREP2		0x0802
#define IDV_MANREP3		0x0803
#define IDV_MANREP4		0x0804
#define IDV_MANREP5		0x0805
#define IDV_MANREP6		0x0806
#define IDV_MANREP7		0x0807
#define IDV_MANREP8		0x0808
#define IDV_MANREP9		0x0809
#define IDV_MANREP10		0x0810
#define IDV_MANFUR		0x0811

// Scene 9: CITY STRATEGY ROOM
#define IDV_STRATPAN		0x0900
#define IDV_STRATMON1		0x0901
#define IDV_STRATMON2		0x0902
#define IDV_STRATMAP		0x0903

// Scene 10:
#define IDV_SCN10PT0	0x1000
#define IDV_SCN10PIT	0x1001

// Scene 11:
#define IDV_FA1PAN		0x1101
#define IDV_FH1PTH1		0x1102
#define IDV_FH1PTH2		0x1103
#define IDV_FH1PAN		0x1104

#define IDV_GRBDRT1		0x1105
#define IDV_GRBDRT2		0x1106

#define IDV_PATH1_1		0x1127
#define IDV_PATH1_2		0x1128
#define IDV_PATH1_3		0x1129
#define IDV_PATH1_4		0x1130
#define IDV_PATH1_5		0x1131
#define IDV_PATH1_6		0x1132
#define IDV_PATH1_7		0x1133
#define IDV_PATH1_8		0x1134

// Scene 12: TEMPLE
#define IDV_TM1PAN		0x1211
#define IDV_TM2PAN		0x1212
#define IDV_VESTPAN1	0x1213
#define IDV_VESTPAN2	0x1215
#define IDV_TM3PAN		0x1216
#define IDV_TM4PAN		0x1217
#define IDV_TM5PAN		0x1218
#define IDV_TMCU1		0x1219
#define IDV_TMCU2		0x1220
#define IDV_TMCU3		0x1221
#define IDV_TMCU4		0x1222
#define IDV_TMCUT		0x1223

// Scene 13: New Village
#define IDV_VIL2		0x1300
#define IDV_VIL3		0x1301
#define IDV_VIL4		0x1302
#define IDV_VIL5		0x1303
#define IDV_VIL6		0x1304
#define IDV_VIL7		0x1305
#define IDV_VIL8		0x1306
#define IDV_VIL9		0x1307
#define IDV_VILA		0x1308
#define IDV_VILB		0x1309
#define IDV_VILC		0x1310
#define IDV_VILD		0x1311
#define IDV_VILE		0x1312
#define IDV_VILF		0x1313
#define IDV_VILG		0x1314
#define IDV_VILH		0x1315
#define IDV_VILI		0x1316
#define IDV_VILJ		0x1317
#define IDV_VILK		0x1318
#define IDV_VILL		0x1319
#define IDV_VILM		0x1320
#define IDV_VILN		0x1321
#define IDV_VILO		0x1322
#define IDV_VILP		0x1323
#define IDV_VILQ		0x1324
#define IDV_VILR		0x1325


// Scene 15:
#define IDV_TMPLPTH1		0x1501
#define IDV_TMPLPTH2		0x1502
#define IDV_TMPLPTH3		0x1503
#define IDV_TMPLPTH4		0x1504
#define IDV_TMPLPTH5		0x1505
#define IDV_TMPLPTH6		0x1506
#define IDV_TMPLPTH7		0x1507
#define IDV_TMPLPTH8		0x1508
#define IDV_TMPLPTH9		0x1509

// Scene 16: Grand central Stations
#define IDV_GRNDCNT1		0x1601

// Scene 17: Mine Quarry
#define IDV_QUARSA		0x1701
#define IDV_QUARSB		0x1702

#define IDV_aMINEPAN	0x1703
#define IDV_aCV1aPAN	0x1704
#define IDV_aCV1bPAN	0x1705
#define IDV_bMINEPAN	0x1706
#define IDV_bCV1aPAN	0x1707
#define IDV_bCV1bPAN	0x1708
#define IDV_cMINEPAN	0x1709
#define IDV_cCV1aPAN	0x170a
#define IDV_cCV1bPAN	0x170b
#define IDV_dMINEPAN	0x170c
#define IDV_dCV1aPAN	0x170d
#define IDV_dCV1bPAN	0x170e
#define IDV_eMINEPAN	0x170f
#define IDV_eCV1aPAN	0x1710
#define IDV_eCV1bPAN	0x1711

// Scene 21: Nature scene 1
#define IDV_NAT1a		0x2101
#define IDV_NAT1b		0x2102
#define IDV_NAT1c		0x2103
#define IDV_NAT1d		0x2104
#define IDV_NAT1e		0x2105
#define IDV_NAT1f		0x2106
#define IDV_NAT1g		0x2107
#define IDV_NAT1h		0x2108
#define IDV_NAT1i		0x2109
#define IDV_NAT1j		0x2110
#define IDV_NAT1k		0x2111
#define IDV_NAT1l		0x2112
#define IDV_NAT1m		0x2113
#define IDV_NAT1n		0x2114
#define IDV_NAT1o		0x2115
#define IDV_NAT1p		0x2116

// Scene 22: SPELL SCROLLS
#define IDV_PARCHPAN	0x2201

// Scene 24: EYE O GOD

#define IDV_EYEA		0x2401
#define IDV_EYEB		0x2402
#define IDV_EYEC1P		0x2403

// Scene 25: WALL RUINS

#define IDV_WR1			0x2501
#define IDV_WR2			0x2502
#define IDV_WR3			0x2503
#define IDV_W4LOCK		0x2504
#define IDV_WALL1EN		0x2505
#define IDV_WALL1IN		0x2506
#define IDV_WALL2EN		0x2507
#define IDV_WALL2IN		0x2508
#define IDV_PL1P		0x2509
#define IDV_PL2P		0x2510
#define IDV_HIDDENR1		0x2511
#define IDV_HIDDENR2		0x2512

//SCENE 26: BANISHMENT
#define IDV_BANISH		0x2612

// Scene 60: GAME BOARD 
// * keep this last in all the tables for quick testing of database
#define IDV_BOARD		0x6000
