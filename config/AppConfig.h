#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#ifdef ACTIVATION_MODE_OTAA
// format: lsb 
#define TTN_APPLICATION_EUI {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

// format: lsb 
#define TTN_DEVICE_EUI {DEVICE_ID, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

// format: msb
#define TTN_APP_KEY {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#endif


#ifdef ACTIVATION_MODE_ABP
#define TTN_DEVICE_ADDRESS 0x00000000
// msb
#define TTN_NETWORK_SESSION_KEY {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
// msb
#define TTN_APP_SESSION_KEY {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#endif

#endif
