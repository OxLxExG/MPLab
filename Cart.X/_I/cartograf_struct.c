
struct Inclinometer {
	float Azm;
	float Zen;
	float Aps;
};
 
struct direct_RX {//128
	complex <float> Tx_0;//8
	complex <float> Geo[4];//32
	float amp_Vzz[4];//16
	float amp_Vzx[4];//16
	float ph_Vzx_Vzz[4];//16
	float dv[4];//16
	float border_angle[4];//16
	float temperature;//4
	uint32_t condition;//4
};

struct undirect_RX {
	complex <float> Tx_0[2];
	complex <float> Rzz1[4];
	complex <float> Rzz2[4];
	float temperature;
	uint32_t condition;
};

struct _16_sector_data { //RAW с напр. приемников (3,4) по номеру TX(1,2,4,5 или 1,2,3,5)
	float sector[2][16];//Re,Im
};


struct RAW_DATA {
	struct _16_sector_data R_zx[2][4];// Rx_Left, Rx_Right, Tx1234[4] 
};


//AllData_Direct_1DDS_1freq  
struct DATA { // всего 480 байт.
	uint32_t signature;                                          //
	uint32_t frame;                                              //
	uint32_t dds_freq;                                           //
	struct direct_RX R_zx[2];                                    //
	struct undirect_RX R_zz;                                     //
	complex <float> geo_signal_smt[4];                           //  
	float phase_smt[4];                                          //
	float rho_smt[4];                                            //
	float rho_up_smt[4];                                         // 
	float rho_down_smt[4];                                       // 
	struct Inclinometer INC;                                     //
	int32_t ApsStartAngle;                                       //
	float Wg ;                                                   // 
	uint32_t error_msg;  
	uint32_t depth;                                             
	//uint16_t expService[16];                                     
	uint32_t Tx_sector_condition[8];//
};

// это структура, которую отдает прибор по команде выполнить измерения
struct ALL_DATA_1FREQ {
	DATA data;
	RAW_DATA raw_data;
};