/*
 * Copyright (C) 2011 Google, Inc.
 * Copyright (C) 2010 Samsung Electronics.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef __MODEM_LINK_DEVICE_DPRAM_H__
#define __MODEM_LINK_DEVICE_DPRAM_H__

#include <linux/wakelock.h>
#include <linux/workqueue.h>
#include <linux/timer.h>
#include <linux/platform_data/modem.h>

/* for DPRAM hostboot */
#define CMC22x_AP_BOOT_DOWN_DONE	0x54329876
#define CMC22x_CP_REQ_MAIN_BIN		0xA5A5A5A5
#define CMC22x_CP_REQ_NV_DATA		0x5A5A5A5A
#define CMC22x_CP_DUMP_MAGIC		0xDEADDEAD

#define CMC22x_HOST_DOWN_START		0x1234
#define CMC22x_HOST_DOWN_END		0x4321
#define CMC22x_REG_NV_DOWN_END		0xABCD
#define CMC22x_CAL_NV_DOWN_END		0xDCBA

#define CMC22x_1ST_BUFF_READY		0xAAAA
#define CMC22x_2ND_BUFF_READY		0xBBBB
#define CMC22x_1ST_BUFF_FULL		0x1111
#define CMC22x_2ND_BUFF_FULL		0x2222

#define CMC22x_CP_RECV_NV_END		0x8888
#define CMC22x_CP_CAL_OK		0x4F4B
#define CMC22x_CP_CAL_BAD		0x4552
#define CMC22x_CP_DUMP_END		0xFADE

#define CMC22x_DUMP_BUFF_SIZE		8192	/* 8 KB */
#define CMC22x_DUMP_WAIT_TIMEOVER	1	/* 1 ms */

/* interrupt masks.*/
#define INT_MASK_VALID			0x0080
#define INT_MASK_CMD			0x0040
#define INT_VALID(x)			((x) & INT_MASK_VALID)
#define INT_CMD_VALID(x)		((x) & INT_MASK_CMD)
#define INT_NON_CMD(x)			(INT_MASK_VALID | (x))
#define INT_CMD(x)			(INT_MASK_VALID | INT_MASK_CMD | (x))

#define INT_CMD_MASK(x)			((x) & 0xF)
#define INT_CMD_INIT_START		0x1
#define INT_CMD_INIT_END		0x2
#define INT_CMD_REQ_ACTIVE		0x3
#define INT_CMD_RES_ACTIVE		0x4
#define INT_CMD_REQ_TIME_SYNC		0x5
#define INT_CMD_PHONE_START		0x8
#define INT_CMD_ERR_DISPLAY		0x9
#define INT_CMD_PHONE_DEEP_SLEEP	0xA
#define INT_CMD_NV_REBUILDING		0xB
#define INT_CMD_EMER_DOWN		0xC
#define INT_CMD_PIF_INIT_DONE		0xD
#define INT_CMD_SILENT_NV_REBUILDING	0xE
#define INT_CMD_NORMAL_POWER_OFF	0xF

/* special interrupt cmd indicating modem boot failure. */
#define INT_POWERSAFE_FAIL              0xDEAD

#define GOTA_CMD_VALID(x)		(((x) & 0xA000) == 0xA000)
#define GOTA_RESULT_FAIL		0x2
#define GOTA_RESULT_SUCCESS		0x1
#define GOTA_CMD_MASK(x)		(((x) >> 8) & 0xF)
#define GOTA_CMD_RECEIVE_READY		0x1
#define GOTA_CMD_DOWNLOAD_START_REQ	0x2
#define GOTA_CMD_DOWNLOAD_START_RESP	0x3
#define GOTA_CMD_IMAGE_SEND_REQ		0x4
#define GOTA_CMD_IMAGE_SEND_RESP	0x5
#define GOTA_CMD_SEND_DONE_REQ		0x6
#define GOTA_CMD_SEND_DONE_RESP		0x7
#define GOTA_CMD_STATUS_UPDATE		0x8
#define GOTA_CMD_UPDATE_DONE		0x9
#define GOTA_CMD_EFS_CLEAR_RESP		0xB
#define GOTA_CMD_ALARM_BOOT_OK		0xC
#define GOTA_CMD_ALARM_BOOT_FAIL	0xD

#define CMD_DL_START_REQ		0x9200
#define CMD_IMG_SEND_REQ		0x9400
#define CMD_DL_SEND_DONE_REQ		0x9600
#define CMD_UL_RECEIVE_RESP		0x9601
#define CMD_UL_RECEIVE_DONE_RESP	0x9801

#define START_INDEX			0x7F
#define END_INDEX			0x7E

#define DP_MAGIC_DMDL			0x4445444C
#define DP_MAGIC_UMDL			0x4445444D
#define DP_DPRAM_SIZE			0x4000
#define DP_DEFAULT_WRITE_LEN		8168
#define DP_DEFAULT_DUMP_LEN		16366
#define DP_DUMP_HEADER_SIZE		7

#define GOTA_TIMEOUT			(50 * HZ)
#define GOTA_SEND_TIMEOUT		(200 * HZ)
#define DUMP_TIMEOUT			(30 * HZ)
#define DUMP_START_TIMEOUT		(100 * HZ)

enum cmc22x_boot_mode {
	CMC22x_BOOT_MODE_NORMAL,
	CMC22x_BOOT_MODE_DUMP,
};

enum dpram_init_status {
	DPRAM_INIT_STATE_NONE,
	DPRAM_INIT_STATE_READY,
};

struct dpram_boot_img {
	char *addr;
	int size;
	enum cmc22x_boot_mode mode;
};

#define MAX_PAYLOAD_SIZE 0x2000
struct dpram_boot_frame {
	unsigned request;	/* AP to CP Message */
	unsigned response;	/* CP to AP Response */
	ssize_t len;		/* request size*/
	unsigned offset;	/* offset to write */
	char data[MAX_PAYLOAD_SIZE];
};

/* buffer type for modem image */
struct dpram_firmware {
	char *firmware;
	int size;
	int is_delta;
};

struct dpram_dump_arg {
	char *buff;
	int buff_size;	/* AP->CP: Buffer size */
};

enum dpram_link_mode {
	DPRAM_LINK_MODE_INVALID = 0,
	DPRAM_LINK_MODE_IPC,
	DPRAM_LINK_MODE_BOOT,
	DPRAM_LINK_MODE_DLOAD,
	DPRAM_LINK_MODE_ULOAD,
};

struct dpram_boot_map {
	u8  __iomem *buff;
	u32 __iomem *req;
	u32 __iomem *resp;
	u32          size;
};

struct dpram_dload_map {
	u32 __iomem *magic;
	u8  __iomem *buff;
};

struct dpram_uload_map {
	u32 __iomem *magic;
	u8  __iomem *buff;
};

struct dpram_ota_header {
	u8 start_index;
	u16 nframes;
	u16 curframe;
	u16 len;

} __packed;

struct ul_header {
	u8  bop;
	u16 total_frame;
	u16 curr_frame;
	u16 len;
};

#define DP_BOOT_REQ_OFFSET	0
#define DP_BOOT_RESP_OFFSET	8
#define DP_DLOAD_BUFF_OFFSET	4
#define DP_ULOAD_BUFF_OFFSET	4

struct dpram_link_device {
	struct link_device ld;

	/* The mode of this DPRAM link device */
	enum dpram_link_mode mode;

	/* DPRAM address and size */
	u32         dp_size;	/* DPRAM size                  */
	u8 __iomem *dp_base;	/* DPRAM virtual base address  */

	/* DPRAM IRQ from CP */
	int irq;

	/* Link to DPRAM control functions dependent on each platform */
	int max_ipc_dev;
	struct modemlink_dpram_control *dpctl;

	/* Physical configuration -> logical configuration */
	struct dpram_boot_map  bt_map;
	struct dpram_dload_map dl_map;
	struct dpram_uload_map ul_map;

	/* Wakelock for DPRAM device */
	struct wake_lock dpram_wake_lock;

	/* For booting */
	struct completion dpram_init_cmd;
	struct completion modem_pif_init_done;

	/* For GOTA */
	struct completion gota_start_complete;
	struct completion gota_send_done;
	struct completion gota_recv_done;
	struct completion gota_update_done;

	/* For CP RAM dump */
	struct completion dump_start_complete;
	struct completion dump_recv_done;
	struct timer_list dump_timer;
	int dump_rcvd;		/* Count of dump packets received */

	/* For efficient receive process */
	struct work_struct   rx_work;
	struct io_device    *iod[MAX_DEV_FORMAT];
	struct sk_buff_head  skb_rxq[MAX_DEV_FORMAT];

	/* Multi-purpose miscellaneous buffer */
	u8 *buff;

	/* DPRAM IPC initialization status */
	int dpram_init_status;

	/* For exact timestamp */
	struct timespec ts_base;
	struct timespec ts_now;
};

/* converts from struct link_device* to struct xxx_link_device* */
#define to_dpram_link_device(linkdev) \
			container_of(linkdev, struct dpram_link_device, ld)

#endif
