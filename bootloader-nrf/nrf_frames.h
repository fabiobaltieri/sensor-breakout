#include <stdint.h>

#define NRF_MSG_ID_GENERIC	0x00
#define NRF_MSG_ID_POWER	0x01
#define NRF_MSG_ID_BOOT		0xff

#define NRF_POWER_VBATT_CHARGING	(1 << 15)
#define NRF_POWER_VBATT_MASK		0x7fff
struct nrf_power {
	uint16_t value[4];
	uint16_t vbatt;
	uint8_t _spare[2];
};

#define NRF_BOOT_CMD_ACK	0x00
#define NRF_BOOT_CMD_BOOT	0x01
#define NRF_BOOT_CMD_START	0x02
#define NRF_BOOT_CMD_WRITE(len)	(0x03 | (len << 4))
#define NRF_BOOT_CMD_FINALIZE	0x04
#define NRF_BOOT_CMD_MASK	0x0f
#define NRF_BOOT_SIZE(v)	(v >> 4)
struct nrf_boot {
	uint8_t cmd;
	uint8_t cksum;
	uint8_t payload[10];
};

union nrf_msg {
	uint8_t generic[12];
	struct nrf_power power;
	struct nrf_boot boot;
};

struct nrf_frame {
	uint8_t board_id;
	uint8_t msg_id;
	uint8_t len;
	uint8_t seq;
	union nrf_msg msg;
};
