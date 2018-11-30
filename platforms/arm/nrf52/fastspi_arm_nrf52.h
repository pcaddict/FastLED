#ifndef __INC_FASTSPI_NRF_H
#define __INC_FASTSPI_NRF_H

#ifdef NRF52

#ifndef FASTLED_FORCE_SOFTWARE_SPI
#define FASTLED_ALL_PINS_HARDWARE_SPI

// A nop/stub class, mostly to show the SPI methods that are needed/used by the various SPI chipset implementations.  Should
// be used as a definition for the set of methods that the spi implementation classes should use (since C++ doesn't support the
// idea of interfaces - it's possible this could be done with virtual classes, need to decide if i want that overhead)
template <uint8_t _DATA_PIN, uint8_t _CLOCK_PIN, uint8_t _SPI_CLOCK_DIVIDER>
class NRF52SPIOutput {

  struct saveData {
    uint32_t sck;
    uint32_t mosi;
    uint32_t miso;
    uint32_t freq;
	uint32_t config;
    uint32_t enable;
  } mSavedData;

  void saveSPIData() {
    mSavedData.sck = NRF_SPIM0->PSEL.SCK;
    mSavedData.mosi = NRF_SPIM0->PSEL.MOSI;
    mSavedData.miso = NRF_SPIM0->PSEL.MISO;
    mSavedData.freq = NRF_SPIM0->FREQUENCY;
	mSavedData.config = NRF_SPIM0->CONFIG;
    mSavedData.enable = NRF_SPIM0->ENABLE;
  }

  void restoreSPIData() {
    NRF_SPIM0->PSEL.SCK = mSavedData.sck;
    NRF_SPIM0->PSEL.MOSI = mSavedData.mosi;
    NRF_SPIM0->PSEL.MISO = mSavedData.miso;
    NRF_SPIM0->FREQUENCY = mSavedData.freq;
	NRF_SPIM0->CONFIG = mSavedData.config;
    mSavedData.enable = NRF_SPIM0->ENABLE;
  }

public:
  NRF52SPIOutput() { FastPin<_DATA_PIN>::setOutput(); FastPin<_CLOCK_PIN>::setOutput(); }
  //(Selectable *PSEL.ect) {  FastPin<_DATA_PIN>::setOutput(); FastPin<_CLOCK_PIN>::setOutput();  }

  // set the object representing the selectable
  //void setSelect(Selectable *PSEL.ect) { /* TODO */ }

  // initialize the SPI subssytem
  void init() {
	uint32_t config = SPI_CONFIG_ORDER_MsbFirst;    // Set SPI CONFIG to MSB first.
	config |= (SPI_CONFIG_CPOL_ActiveHigh << SPI_CONFIG_CPOL_Pos);    // SPI MODE0
	config |= (SPI_CONFIG_CPHA_Leading << SPI_CONFIG_CPHA_Pos);    // SPI MODE0

    FastPin<_DATA_PIN>::setOutput();
    FastPin<_CLOCK_PIN>::setOutput();
    NRF_SPIM0->PSEL.SCK = g_ADigitalPinMap[PIN_SPI_SCK];
    NRF_SPIM0->PSEL.MOSI = g_ADigitalPinMap[PIN_SPI_MOSI];
    NRF_SPIM0->PSEL.MISO = 1;
    NRF_SPIM0->FREQUENCY = 0x80000000;
	NRF_SPIM0->CONFIG = config;
    NRF_SPIM0->EVENTS_ENDRX = 0;
    NRF_SPIM0->EVENTS_ENDTX = 0;
    NRF_SPIM0->EVENTS_STARTED = 0;
    NRF_SPIM0->ENABLE = 0x00000007;
  }

  // latch the CS select
  void select() { saveSPIData(); init(); }

  // release the CS select
  void release() { shouldWait(); restoreSPIData(); }

  static bool shouldWait(bool wait = false) __attribute__((always_inline)) __attribute__((always_inline)) {
    // static bool sWait=false;
    // bool oldWait = sWait;
    // sWait = wait;
    // never going to bother with waiting since we're always running the spi clock at max speed on the rfduino
    // TODO: When we set clock rate, implement/fix waiting properly, otherwise the world hangs up
    return false;
  }
  
  // wait until all queued up data has been written
  static void waitFully() __attribute__((always_inline)){ if(shouldWait()) { while(!NRF_SPIM0->EVENTS_END); } NRF_SPIM0->EVENTS_END = 0; }
  static void wait() __attribute__((always_inline)){ if(shouldWait()) { while(!NRF_SPIM0->EVENTS_END); } NRF_SPIM0->EVENTS_END = 0; }

  // write a byte out via SPI (returns immediately on writing register)
  static void writeByte(uint8_t b) __attribute__((always_inline))
  {
      NRF_SPIM0->TXD.MAXCNT = sizeof(b);
      NRF_SPIM0->TXD.PTR = (uint32_t)&b;
      
      NRF_SPIM0->EVENTS_END = 0;
      NRF_SPIM0->TASKS_START = 1;
      while (!NRF_SPIM0->EVENTS_END);
      NRF_SPIM0->EVENTS_END = 0;
  }

  // write a word out via SPI (returns immediately on writing register)
  static void writeWord(uint16_t w) __attribute__((always_inline)){ writeByte(w>>8); writeByte(w & 0xFF);  }

  // A raw set of writing byte values, assumes setup/init/waiting done elsewhere (static for use by adjustment classes)
  static void writeBytesValueRaw(uint8_t value, int len) { while(len--) { writeByte(value);  } }

  // A full cycle of writing a value for len bytes, including select, release, and waiting
  void writeBytesValue(uint8_t value, int len) {
    select();
    while(len--) {
      writeByte(value);
    }
    waitFully();
    release();
  }

  // A full cycle of writing a raw block of data out, including select, release, and waiting
  template<class D> void writeBytes(uint8_t *data, int len) {
    uint8_t *end = data + len;
    select();
    while(data != end) {
      writeByte(D::adjust(*data++));
    }
    D::postBlock(len);
    waitFully();
    release();
  }

  void writeBytes(uint8_t *data, int len) {
    writeBytes<DATA_NOP>(data, len);
  }

  // write a single bit out, which bit from the passed in byte is determined by template parameter
  template <uint8_t BIT> inline static void writeBit(uint8_t b) {
    waitFully();
    //NRF_SPIM0->ENABLE = 0;
    //if(b & 1<<BIT) {
    //  FastPin<_DATA_PIN>::hi();
    //} else {
    //  FastPin<_DATA_PIN>::lo();
    //}
    //FastPin<_CLOCK_PIN>::toggle();
    //FastPin<_CLOCK_PIN>::toggle();
    //NRF_SPIM0->ENABLE = 1;
	  writeByte((b & (1 << BIT)) != 0);
  }

  template <uint8_t FLAGS, class D, EOrder RGB_ORDER> void writePixels(PixelController<RGB_ORDER> pixels) {
    select();
    int len = pixels.mLen;
    while(pixels.has(1)) {
      if(FLAGS & FLAG_START_BIT) {
				writeBit<0>(1);
      }
			writeByte(D::adjust(pixels.loadAndScale0()));
			writeByte(D::adjust(pixels.loadAndScale1()));
			writeByte(D::adjust(pixels.loadAndScale2()));

			pixels.advanceData();
			pixels.stepDithering();
		}
		D::postBlock(len);
		waitFully();
		release();
  }

};

#endif
#endif

#endif
