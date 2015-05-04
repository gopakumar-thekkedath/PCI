#ifndef _PCI_H_
#define _PCI_H_

struct cap_hdr {
	uint8_t cap_id;
	uint8_t next_ptr;
}__attribute__((packed));

struct ext_cap_hdr {
	uint16_t cap_id;
	uint16_t offset_ver;
}__attribute__((packed));

struct pcie_cap {
	struct cap_hdr hdr;
	uint16_t pcie_caps_reg;
	uint32_t dev_cap;
	uint16_t dev_ctrl;
	uint16_t dev_stat;
	uint32_t link_cap;
	uint16_t link_ctrl;
	uint16_t link_stat;
	uint32_t slot_cap;
	uint16_t slot_ctrl;
	uint16_t slot_stat;
	uint16_t root_ctrl;
	uint16_t root_cap;
	uint32_t root_stat;
	uint32_t dev_cap2;
	uint16_t dev_ctrl2;
	uint16_t dev_stat2;
	uint32_t link_cap2;
	uint16_t link_ctrl2;
	uint16_t link_stat2;
	uint32_t slot_cap2;
	uint16_t slot_ctrl2;
	uint16_t slot_stat2;
}__attribute__((packed));

struct msi_cap_common {
	struct cap_hdr hdr;
	uint16_t msg_ctrl;
}__attribute__((packed));

struct msi_cap32 {
	struct msi_cap_common common;
	uint32_t msg_addr;
	uint16_t msg_data;
};

struct msi_cap64 {
	struct msi_cap_common common;
	uint32_t msg_addr_low;
	uint32_t msg_addr_high;
	uint16_t msg_data;
};

struct aer_cap {
	struct ext_cap_hdr hdr;
};

enum pci_regs {
	VID_DID 	= 0,
	CMD_STATUS 	= 4,
	BRDG_CTRL 	= 0x3e,
	CAP_PTR		= 0x34,
};

enum cap_ids {
	PCIE_CAP 	= 0x10,
	MSI_CAP		= 0x5,
	AER_CAP		= 0x1,
};

enum pcie_devtype {
	ENDPOINT = 0x0,
	LEGACY_ENDPOINT = 0x1,
	ROOT_PORT = 0x4,
	US_PORT = 0x5,
	DS_PORT = 0x6,
	PCIE_PCI_BRIDGE = 0x7,
	PCI_PCIE_BRIDGE = 0x8,
	RC_INTEGRATED_EP = 0x9,
	RC_EVENT_COLLECTOR = 0xa,
};

#define MEM_SPACE 		(1 << 1U)
#define SECBUS_RESET 	(1 << 6U)
#define CAP_LIST		(1 << 4U)
#endif /* _PCI_H_ */
