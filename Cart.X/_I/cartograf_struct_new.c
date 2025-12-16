typedef struct __attribute__((packed))
{
    float Re;
    float Im;
} complex_t;


typedef struct __attribute__((packed))
{
	float Azm;
	float Zen;
	float Aps;
} Inclinometer_t;
 
typedef struct __attribute__((packed))
{//128
	complex_t Tx_0;//8
	complex_t Geo[4];//32
	float amp_Vzz[4];//16
	float amp_Vzx[4];//16
	float ph_Vzx_Vzz[4];//16
	float dv[4];//16
	float border_angle[4];//16
	float temperature;//4
	uint32_t condition;//4
} direct_RX_t;

typedef struct __attribute__((packed))
{
	complex_t Tx_0[2];
	complex_t Rzz1[4];
	complex_t Rzz2[4];
	float temperature;
	uint32_t condition;
} undirect_RX_t;

typedef struct __attribute__((packed))
{ //RAW с напр. приемников (3,4) по номеру TX(1,2,4,5 или 1,2,3,5)
	float sector_Re[16];//Re,Im
	float sector_Im[16];//Re,Im
} _16_sector_data_t;


typedef struct __attribute__((packed))
{
	_16_sector_data_t Rx_Left[4];// Rx_Left, Rx_Right, Tx1234[4] 
	_16_sector_data_t Rx_Right[4];// Rx_Left, Rx_Right, Tx1234[4] 
} RAW_DATA_t;


//AllData_Direct_1DDS_1freq  
typedef struct __attribute__((packed))
{ // всего 480 байт.
	uint32_t signature;                                          //
	uint32_t frame;                                              //
	uint32_t dds_freq;                                           //
	direct_RX_t R_zx[2];                                    //
	undirect_RX_t R_zz;                                     //
	complex_t geo_signal_smt[4];                           //  
	float phase_smt[4];                                          //
	float rho_smt[4];                                            //
	float rho_up_smt[4];                                         // 
	float rho_down_smt[4];                                       // 
	Inclinometer_t INC;                                     //
	int32_t ApsStartAngle;                                       //
	float Wg ;                                                   // 
	uint32_t error_msg;  
	uint32_t depth;                                             
	//uint16_t expService[16];                                     
	uint32_t Tx_sector_condition[8];//
} DATA_t;

// это структура, которую отдает прибор по команде выполнить измерения
typedef struct __attribute__((packed))
{
	DATA_t data;
	RAW_DATA_t raw_data;
} ALL_DATA_1FREQ_t;

//структура метрологии и ее подструктуры
typedef struct __attribute__((packed))
{
	uint32_t MA_WINDOW_SIZE; //32
	uint32_t MLD_WINDOW_SIZE; //32
	uint32_t N; //128
	uint32_t K; //128
	uint32_t auto_delta; //1
	uint32_t INTERRUPT_BY_ANGLE_PATH; // 0
	float W_MIN; //3.14
	float W_MAX; //18.84
	float K_predict; //0.5
	float APS_DELTA; //PI/360
	float history_angle; //(20*PI)/180
	float K_delta; //2.5
} INC_SET_t;

typedef struct __attribute__((packed))
{
	uint32_t Tx[6]; // = { 0,1,2,3,4,5 };// порядок работы передатчиков в режиме ненаправленных измерений
	uint32_t Tx_by_turn[3][5][6]; // порядок работы передатчиков в режиме направленных измерений для разных скоростей вращения буровой колонны
	uint32_t tx_work_time_units[7]; // = { 5,5,6,7,8,9,0 };//время работы одного передатчика в единицах цикла опроса датчиков положения (1 мс)
} TURN_PRESETS_t;

typedef struct __attribute__((packed))
{
	uint32_t signature;
	uint32_t serial;
	uint32_t L1[4];//
	uint32_t L2[4];//
	uint32_t L_geo[5];//
	uint32_t F[2];//
	int32_t Air_zz[10];//воздух ненаправленных измерений
	int32_t Air_zz_amt[10];//минимальная амплитуда, определяющая работоспомсобность передатчика для ненаправленных измерений
	INC_SET_t inc_set;
	TURN_PRESETS_t turn_presets;
	uint32_t D_sonde_mm;//диаметр прибора	
	uint64_t unix_data;//дата в юникс эпох
	uint32_t service[95];//на будущее до 1000 байт !!!!
} METROLOGY_t;

// структура пресетов
typedef struct __attribute__((packed))
{
	uint16_t Signature = 0x5AA5; // 0x5AA5 - для определения занятого места во флэше
	uint16_t TBTIndex;  // индекс в массиве Tx_by_turn - выводить
	uint16_t MinRotSpeed; // минимальная скорость вращения для направленных измерений (ниже - переходим на ненаправленные) - выводить
	uint16_t DebugSession; // отладочная сессия (да/нет) - выводить
	uint64_t DateTime;// - выводить
} SET_OF_PRESETS_t;
