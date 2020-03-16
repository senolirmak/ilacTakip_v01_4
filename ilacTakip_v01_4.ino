// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
	Name:       ilac_01_3.ino
	Created:	17.01.2020 22:05:38
	Author:     DESKTOP-K64EUL3\senol
*/

//#define _TASK_SLEEP_ON_IDLE_RUN
#define _TASK_WDT_IDS 
#ifndef LED_BUILTIN
#define LED_BUILTIN D0    // define appropriate pin for your board
#endif
#define _mBACK_PIN D8
#define _mOK_PIN D7
#define _mNEXT_PIN D6
#define _DURATION 1000

//++++++++++++++
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "SSD1306Wire.h" 
#include "images.h"
#include "OLEDDisplayUi.h"
// Include custom images


// Initialize the OLED display using Wire library
SSD1306Wire  display(0x3c, D3, D5);
OLEDDisplayUi ui(&display);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 10800, 60000);

int screenW = 128;
int screenH = 64;
int CenterX = screenW / 2;
int CenterY = ((screenH - 16) / 2) + 16;   // top yellow part is 16 px height
//Wifi ssid ve password
const char *ssid = "wifi ssid";
const char *password = "wifi password";
//++++++++++++++

#include <RTClib.h>
//#include <Stepper.h>
#include <TaskScheduler.h>
#include <Bounce2.h>

Scheduler ilacTakvim;

Bounce mBack = Bounce();
Bounce mOK = Bounce();
Bounce mNext = Bounce();

const int stepsPerRevolution = 200;

unsigned int aktifTask_id = 0;
unsigned int nwake = 0, awake = 0;
unsigned int MenuGoster = 2;
int frameCount = 2;
//int overlaysCount = 1;

//Stepper ilacStepper(stepsPerRevolution, D1, D2, D4, D9);

void wrapperCallback();
bool islemOnEnable();
void islemOnDisable();
void LEDOff();
void LEDOn();
//void clockOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {}
//OverlayCallback overlays[] = { clockOverlay };

// Tasks

Task _govde(_DURATION*2, TASK_ONCE, NULL,false); //0, -1, NULL, false);&wrapperCallback    <--tetik
Task _islem(_DURATION/2, TASK_FOREVER, false); // TASK_SECOND, TASK_FOREVER, NULL, false);<--islem
bool setupCalistirildi = true;

bool mTaskBayrak[] = { 1,1,1,1,1,1 };
uint8_t idTask = 0;
const char* cdays[7] = { "Cum", "Cmt","Paz","Pzt", "Sal", "Car","Per" };
TimeSpan uisaat[] = { 21600, 32400 , 43200 , 50400 , 57600 ,72000  };
//TimeSpan uisaat[] = { 85380 , 85500 , 86220 , 86280 , 85980 , 86040};
const int uzunluk = sizeof(uisaat) / sizeof(TimeSpan);

class lcdGoster {
public:

	String EkranikiOncem(String _str) {
		int _uzn = 0;
		String _ustr = _str;
		_uzn = _ustr.length();
		while (_uzn < 2) {
			_ustr = " " + _ustr;
			_uzn = _ustr.length();
		}
		return _ustr;
	}
	String EkranikiSonra(String _str) {
		int _uzn = 0;
		String _ustr = _str;
		_uzn = _ustr.length();
		while (_uzn < 2) {
			_ustr = _ustr + " ";
			_uzn = _ustr.length();
		}
		return _ustr;
	}
	String EkranTamSaga(String _str) {
		int _uzn = 0;
		_uzn = _str.length();
		while (_uzn < 16) {
			_str = " " + _str;
			_uzn = _str.length();
		}
		return _str;
	}
	uint8_t xKoordinat(String _str) {
		int _uzn = 0;
		_uzn = _str.length();
		return 16 - _uzn;
	}
	void EkranBekleCalis(TimeSpan& ts, uint8_t _id) {
		String _text = "";
		_text = this->baslik[_id];
		_text += EkranikiOncem(String(ts.hours())) + ":" + EkranikiSonra(String(ts.minutes()));
		this->lcdEkranaYaz(_text, 1);
	}
	void lcd_Clear(void) {
		display.clear();
	}
	void lcd_SetCursor(uint8_t _x, uint8_t _y) {
		this->x_sutun = _x;
		this->y_satir = _y;
		//lcd.setCursor(_x,_y);
	}
	void lcd_Print(String _txt) {
		display.drawString(x_sutun,y_satir,_txt);
		display.display();
	}
	void lcdStringPrint(String _txt, uint8_t _sutun = 0, uint8_t _satir = 0) {
		this->lcd_SetCursor(_sutun, _satir);
		this->lcd_Print(_txt);
	}
	void lcdEkranaYaz(String _txt, uint8_t _satir = 0) {
		this->lcdStringPrint(_txt,this->xKoordinat(_txt),_satir);
	}
	void lcdEkranVanaOnOFF(uint8_t _interV, bool _On = true) {
		display.setTextAlignment(TEXT_ALIGN_LEFT);
		display.setFont(ArialMT_Plain_16);
		this->lcd_SetCursor(0, 16);
		display.drawString(x_sutun + (_interV + 1) * 16, y_satir, String(_interV + 1));
		this->lcd_SetCursor(0, 34);
		display.drawString(x_sutun, y_satir, "V:");
		(_On) ? display.drawString(x_sutun+ (_interV + 1) * 16, y_satir,String(_interV + 1)) : display.drawString(x_sutun + (_interV + 1) * 16, y_satir, "_");
	}
	void EkranGunSaat(DateTime _time0) {
		//this->lcd_Clear();
		this->time0 = _time0;
		String _saat = "", _gunu = "";
		_saat = " " + EkranikiOncem(String(this->time0.hour())) + ":" + EkranikiSonra(String(this->time0.minute()));
		uint8_t _gun = this->time0.dayOfTheWeek();
		_gunu = cdays[_gun];
		_gunu += _saat;
		_gunu = this->EkranTamSaga(_gunu);
		this->lcdEkranaYaz(_gunu);
	}

protected:
	uint8_t x_sutun, y_satir;
	DateTime time0;
	const char* aktifpasif[3] = { "pasif","aktif","iptal" };
	String baslik[1] = {}; //"Saat->" ,"Calisma Saat:Dak","Saat Grubu->" };const char*
};

class taskEkran : public lcdGoster
{
public:
	uint8_t hid;
	taskEkran() {
		this->hid = 0;
	}

	/*int mOkRead() { return this->mOk.read(); }
	bool mOkUpdate() { return this->mOk.update(); }
	long mOkDuration() { return this->mOk.duration(); }*/
	~taskEkran() { ; }
	template <typename T, unsigned S>
	inline unsigned arraysize(const T(&v)[S]) { return S; }

	DateTime TimeUpdate() {
		display.clear();
		timeClient.update();
		//String senol = String(timeClient.getFormattedTime());
		//senol = senol.substring(0, 5);
		DateTime t0 =DateTime(2020,1,timeClient.getDay(),timeClient.getHours(),timeClient.getMinutes(),timeClient.getSeconds());
		return t0;
	}
	void KrulumEkranGunSaat() {
		this->EkranGunSaat(TimeUpdate());
	}
	void DegistirEkran(uint8_t _dmid) {
		this->lcd_Clear();
		TimeSpan ts;
		uint8_t deg;
		switch (_dmid)
		{
		case 0:
			deg = (int)this->iu0.ilkdeger;
			this->lcdEkranaYaz(this->baslik[0] + String(deg + 1), 1);
			break;
		case 1:
			deg = (int)this->iu0.ilkdeger;
			this->lcdEkranaYaz(this->baslik[0] + this->aktifpasif[deg], 1);
			break;
		case 2:
			ts = TimeSpan(0, this->ssk, this->ddk, 0);
			this->EkranBekleCalis(ts, 0);
			break;
		default:
			break;
		}
	}	
	uint8_t iDegistir(uint8_t _mid, uint8_t _ss) {
		int _xbas = this->xKoordinat(this->baslik[0]);
		this->lcd_SetCursor(_xbas,20); // this->iu0.satir);
		this->DegistirEkran(_mid);
		while (1) {
			if (mBack.update()) {
				if (mBack.read() == HIGH && (this->iu0.ilkdeger)) {
					if ((this->iu0.ilkdeger - this->iu0.artim) > this->iu0.artim) { this->iu0.ilkdeger -= this->iu0.artim; }
					else { this->iu0.ilkdeger = this->iu0.altlimit; }
					(_ss == 0) ? this->ssk = this->iu0.ilkdeger : this->ddk = this->iu0.ilkdeger;
					this->DegistirEkran(_mid);
				}
			}
			if (mNext.update()) {
				if (mNext.read() == HIGH && (this->iu0.ilkdeger <= this->iu0.ustlimit)) {
					this->iu0.ilkdeger += this->iu0.artim;
					if (this->iu0.ilkdeger >= this->iu0.ustlimit) this->iu0.ilkdeger = this->iu0.ustlimit;
					(_ss == 0) ? this->ssk = this->iu0.ilkdeger : this->ddk = this->iu0.ilkdeger;
					this->DegistirEkran(_mid);
				}
			}
			if (mOK.update()) {
				if (mOK.read() == HIGH) {
					this->lcd_Clear();
					return this->iu0.ilkdeger;
				}
			}
		}
	}

	//0 --ilacTakvim Seç
	//1 --ilacTakvim Aktif/Pasif/Ýptal(Ekran string(int))
	//2 --Bekle Zamaný(Ekran TimeSpan)
	//3 --Calis Zamaný(Ekran TimeSpan)

	uint8_t Kurulum(void) {
		this->baslik[0] = "Saat Grubu->";
		this->iu0.ilkdeger = 0;
		this->iu0.altlimit = 0;
		this->iu0.ustlimit = uzunluk - 1;
		this->iu0.artim = 1;
		this->hid = this->iDegistir(0, 2);
		this->_indis = this->hid;
		this->KurulumAktifPasif();
	}	
	void KurulumAktifPasif(void) {
		this->baslik[0]= String(this->_indis + 1) + ".Grub->";
		this->iu0.ilkdeger = mTaskBayrak[this->_indis] ? 1 : 0;
		this->iu0.altlimit = 0;
		this->iu0.ustlimit = this->arraysize(this->aktifpasif) - 1;
		this->iu0.artim = 1;
		this->id0 = this->iDegistir(1, 2);
		switch (this->id0) {
		case 1:
			mTaskBayrak[this->_indis] = 1;
			this->KurulumBekleCalis();
			break;
		case 0:
			mTaskBayrak[this->_indis] = 0;
			break;
		default:
			break;
		}

	}	
	void KurulumBekleCalis(void) {

		this->ddk = uisaat[this->_indis].minutes();
		this->ssk = uisaat[this->_indis].hours();
		this->baslik[0] = "Saat->";
		this->iu0.ilkdeger = this->ssk;
		this->iu0.altlimit = 0;
		this->iu0.ustlimit = 23;
		this->iu0.artim = 1;
		this->ssk = this->iDegistir(2, 0);

		this->baslik[0] = "Dakika->";
		this->iu0.ilkdeger = this->ddk;
		this->iu0.altlimit = 0;
		this->iu0.ustlimit = 55;
		this->iu0.artim = 5;
		this->ddk = this->iDegistir(2, 1);
		this->baslik[0] = "Saat->";
		TimeSpan _uisaat = TimeSpan(0, this->ssk, this->ddk, 0);
		uisaat[this->_indis] = _uisaat.totalseconds();

	}

protected:
	uint8_t ssk = 0;
	uint8_t ddk = 0;
	uint8_t _indis = 0;
	uint8_t id0;
	struct iBir {
		unsigned long ilkdeger,
			altlimit,
			ustlimit,
			artim;
	} iu0;

};

class itaskInterval {
public:

	itaskInterval() {}
	~itaskInterval() {}
	const int _uzunluk = sizeof(uisaat) / sizeof(TimeSpan);

	DateTime TimeUpdate() {
		timeClient.update();
		DateTime t0 = DateTime(2020, 1, timeClient.getDay(), timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds());
		return t0;
	}

	uint32_t ilacSaatTimeSpan(uint8_t _idx = 0) {
		DateTime u = this->TimeUpdate();
		TimeSpan saat= uisaat[_idx];
		DateTime _tsi (u.year(), u.month(), u.day(),saat.hours(),saat.minutes(),0);
		DateTime _tsinow(u.year(), u.month(), u.day(), u.hour(), u.minute(), u.second());

		if (_tsi.unixtime()<_tsinow.unixtime())  {
			_tsi = _tsi + TimeSpan(1, 0, 0, 0);
		}
		TimeSpan _ilkItr = _tsi-_tsinow;
		return _ilkItr.totalseconds();//_ilkItr.hours()*3600+_ilkItr.minutes()*60;//_ilkItr.totalseconds();
	}

	uint8_t getIndex(uint8_t size = 6) {
		uint8_t minIndex = 0;
		this->ilacTsDizin();
		uint32_t min = this->_tDizin[0];
		for (int _idx = 0; _idx < size; _idx++) {
			if (this->_tDizin[_idx] < min) {
				min = this->_tDizin[_idx];
				minIndex = _idx;
				//break;
			}
		}
		return minIndex;
	}

	uint32_t getMin(uint8_t size = 6) {
		uint8_t minIndex = 0;
		this->ilacTsDizin();
		uint32_t min = this->_tDizin[minIndex];
		for (int _idx = 0; _idx < size; _idx++) {
			if (this->_tDizin[_idx] < min) {
				min = this->_tDizin[_idx];
			}
		}
		return min;
	}

	void ilacTsDizin(uint8_t size = 6) {
		for (uint8_t _idx = 0; _idx < size; _idx++) {
			this->_tDizin[_idx] = this->ilacSaatTimeSpan(_idx);
		}
	}

private:
	uint32_t _tDizin[6] = {};

};

class govdeTask :public itaskInterval {
public:
	Task govde;
	govdeTask() {
		_islem.setOnDisable(&LEDOff);
		_islem.disable();
		ilacTakvim.addTask(_islem);
		_govde.setOnEnable(&islemOnEnable);
		_govde.setOnDisable(&islemOnDisable);
		_govde.disable();
		ilacTakvim.addTask(_govde);
	}

	void govdeSetup(uint8_t pin_id) {
		this->govde.disable();
		this->govde.setId(pin_id);
		this->govde.setInterval(TASK_SECOND*(this->ilacSaatTimeSpan(pin_id)));
		this->govde.setIterations(TASK_FOREVER);
		this->govde.setCallback(&wrapperCallback);
		this->govde.enable();
		ilacTakvim.addTask(this->govde);
		this->govde.restartDelayed();
	}

	~govdeTask() {}

};

void drawImageDemo() {
	display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
}

taskEkran te;
govdeTask  mv0;
govdeTask  mv1;
govdeTask  mv2;
govdeTask  mv3;
govdeTask  mv4;
govdeTask  mv5;

void clockEkran(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
	timeClient.update();
	//Serial.println(timeClient.getFormattedTime());  
	String senol = String(timeClient.getFormattedTime());
	senol = senol.substring(0, 5);
	display->setTextAlignment(TEXT_ALIGN_CENTER);
	display->setFont(ArialMT_Plain_24);
	display->drawString(CenterX + x, 24, senol);
}

void ilacEkran(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
	te.lcdEkranVanaOnOFF(aktifTask_id,false);
}

FrameCallback frames[] = { clockEkran,ilacEkran };

void setup()
{

	Serial.begin(115200);
	Serial.println("Setup");

	WiFi.begin(ssid, password);
	timeClient.begin();
	ui.setTargetFPS(60);
	ui.setActiveSymbol(activeSymbol);
	ui.setInactiveSymbol(inactiveSymbol);
	ui.setIndicatorPosition(TOP);
	ui.setIndicatorDirection(LEFT_RIGHT);
	ui.setFrameAnimation(SLIDE_LEFT);
	ui.setFrames(frames, frameCount);
	//ui.setOverlays(overlays, overlaysCount);
	ui.init();
	display.init();
	display.flipScreenVertically();
	display.setFont(ArialMT_Plain_16);

	while (WiFi.status() != WL_CONNECTED) {
		delay(10);
	}

	display.clear();

	mBack.attach(_mBACK_PIN);
	mBack.interval(50);

	mOK.attach(_mOK_PIN);
	mOK.interval(50);

	mNext.attach(_mNEXT_PIN);
	mNext.interval(50);

	awake = mOK.read();
	nwake = mBack.read();
	ilacTakvim.enableAll();
	//ilacStepper.setSpeed(60);
	pinMode(LED_BUILTIN, OUTPUT);
	
}

uint8_t aktifSaatSayisi() {
	uint8_t _say = 0;
	for (int i = 0; i < uzunluk - 1; i++) {
		if (mTaskBayrak[i] == 1) _say++;
	}
	return _say;
}

int remainingTimeBudget;
void loop()
{
	ilacTakvim.execute();
	remainingTimeBudget = 0;
	remainingTimeBudget = ui.update();
	if (remainingTimeBudget > 0) {
		if (setupCalistirildi) {
			mv0.govdeSetup(0);
			mv1.govdeSetup(1);
			mv2.govdeSetup(2);
			mv3.govdeSetup(3);
			mv4.govdeSetup(4);
			mv5.govdeSetup(5);
			setupCalistirildi = false;
		}
		delay(remainingTimeBudget);
		display.clear();
	}
	if (mOK.read() != awake && (mOK.duration() > 3000)) {
		awake = mOK.read();
		if (awake) {
			te.lcd_Clear();
			MenuGoster = 1;
			while (awake) {

				mOK.update();
				awake = mOK.read();
			}
		}
	}

	if (mBack.read() != awake && (mBack.duration() > 3000)) {
		nwake = mBack.read();
		if (nwake) {
			te.lcd_Clear();
			MenuGoster = 0;
			while (nwake) {

				mBack.update();
				nwake = mBack.read();
			}
		}
	}

	switch (MenuGoster)
	{
	case 0:
		te.lcd_Clear();
		te.lcd_SetCursor(0,0);
		//te.Kurulum();
		_islem.setCallback(&LEDOff);
		mv0.~govdeTask();
		MenuGoster = 2;
		break;
	case 1:
		te.lcd_Clear();
		te.lcd_SetCursor(0, 0);
		te.Kurulum();

		if (MenuGoster == 1) {
			setupCalistirildi = true;
			//Debug 
		}

		MenuGoster = 2;
		break;
	default:
		mOK.update();
		mBack.update();
		break;
	}

}

void wrapperCallback() {
	Task &t = ilacTakvim.currentTask();
	aktifTask_id = t.getId();
	te.lcdEkranVanaOnOFF(aktifTask_id);
	_govde.restartDelayed();
	unsigned long ig = t.getInterval();
	if (_islem.isEnabled()) {
		if (ig != 86400 * TASK_SECOND) {
			t.disable();
			t.setInterval(86400 * TASK_SECOND);
			t.restartDelayed();
		}
		}
}

bool islemOnEnable() {
	_islem.setCallback(&LEDOn);
	_islem.enable();
	return true;
}

void islemOnDisable() {
	_islem.disable();
}

void LEDOff() {
	digitalWrite(LED_BUILTIN, LOW);
	//te.lcdEkranVanaOnOFF(aktifTask_id, false);
	_islem.setCallback(&LEDOff);

}

void LEDOn() {
	digitalWrite(LED_BUILTIN, HIGH);
	_islem.setCallback(&LEDOff);

}
