#ifndef PCI_H
#define PCI_H

unsigned int pci_read(unsigned int bus, unsigned int slot, unsigned int func, unsigned int offset);
void pci_scan(void);
unsigned short pci_get_vendor(unsigned char bus, unsigned char device, unsigned char function);
unsigned short pci_get_device(unsigned char bus, unsigned char device, unsigned char function);
unsigned char pci_get_class(unsigned char bus, unsigned char device, unsigned char function);
unsigned char pci_get_subclass(unsigned char bus, unsigned char device, unsigned char function);
unsigned char pci_get_prog_if(unsigned char bus, unsigned char device, unsigned char function);
unsigned int pci_get_bar0(unsigned char bus, unsigned char device, unsigned char function);
unsigned int pci_get_header_type(unsigned char bus, unsigned char device, unsigned char function);
void pci_print(unsigned char bus, unsigned char device, unsigned char function, unsigned short vendor, unsigned short device_id, unsigned char class_code, unsigned char subclass, unsigned char prog_if, unsigned int bar0);


#endif // PCI_H