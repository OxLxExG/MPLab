
        ResH = 25
        ResL = 24
        XH = 27
        XL = 26
		CntAllh = 23
		CntAll = 22
        Cnt8 = 18
        PolyH = 19
        PolyL = 20
        data = 21
		data_next = 22
        
        .set CRC_POLYNOME,0xA001  
        
.global crc16Long
.type crc16Long,@function
crc16Long:
		movw             XL,r24
		add				CntAll,r24
		adc				CntAllh,r25
		ldi				PolyL, lo8(CRC_POLYNOME)
  		ldi				PolyH, hi8(CRC_POLYNOME)
        ser             ResH
        ser             ResL
CRCl_Loop1:
        ld              data, X+
        eor             ResL, data
		ldi		Cnt8, 0x08
CRCl_Loop2:
		lsr		ResH
		ror		ResL
		brcc	CRCl_SkipEor
		eor		ResL, PolyL
		eor		ResH, PolyH
CRCl_SkipEor:
		dec		Cnt8
		brne	CRCl_Loop2  
		cp		XL,CntAll
		cpc		XH,CntAllh
		brne	CRCl_Loop1  
		ret

.global crc16
.type crc16,@function
crc16:
		movw             XL,r24
		ldi				PolyL, lo8(CRC_POLYNOME)
  		ldi				PolyH, hi8(CRC_POLYNOME)
        ser             ResH
        ser             ResL
CRC_Loop1:
        ld              data, X+
        eor             ResL, data
		ldi		Cnt8, 0x08
CRC_Loop2:
		lsr		ResH
		ror		ResL
		brcc	CRC_SkipEor
		eor		ResL, PolyL
		eor		ResH, PolyH
CRC_SkipEor:
		dec		Cnt8
		brne	CRC_Loop2  
		dec		CntAll
		brne	CRC_Loop1  
		ret

.global crc16next
.type crc16next,@function
crc16next:
		ldi				PolyL, lo8(CRC_POLYNOME)
  		ldi				PolyH, hi8(CRC_POLYNOME)
        eor             ResL, data_next
		ldi		Cnt8, 0x08
CRC_Loopnext2:
		lsr		ResH
		ror		ResL
		brcc	CRC_SkipEornext
		eor		ResL, PolyL
		eor		ResH, PolyH
CRC_SkipEornext:
		dec		Cnt8
		brne	CRC_Loopnext2  
		ret



; eof crc.S