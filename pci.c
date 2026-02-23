#include "inputs.h"
#include "output.h"    
#include "screen.h"
#include "pci.h"

unsigned int pci_read(unsigned int bus, unsigned int slot, unsigned int func, unsigned int offset) {
    unsigned int address = (1 << 31) | (bus << 16) | (slot << 11) | (func << 8) | (offset & 0xFC);
    outl(0xCF8, address);
    return inl(0xCFC);
} //The function that actually reads from the PCI configuration space. used by pci_scan.

void pci_scan(void) {   //scans all PCI devices. Can be modified to find a specific PCI device.
    for(unsigned int bus = 0; bus < 256; bus++) {
        for(unsigned char device = 0;device < 32; device++) {
            for(unsigned char function = 0; function < 8; function++) {
                unsigned short vendor = pci_get_vendor(bus, device, function);
                if (vendor != 0xFFFF) {
                    unsigned short device_id = pci_get_device(bus, device, function);
                    unsigned char class_code = pci_get_class(bus, device, function);
                    unsigned char subclass = pci_get_subclass(bus, device, function);
                    unsigned char prog_if = pci_get_prog_if(bus, device, function);
                    unsigned int bar0 = pci_get_bar0(bus, device, function);
                    //if(class_code == 0x0C) { // USB controller
                        print("Found pci port : \n");
                        pci_print(bus, device, function, vendor, device_id, class_code, subclass, prog_if, bar0);
                        newLine();
                    //}
                }
            }
        }
    }
}

unsigned short pci_get_vendor(unsigned char bus, unsigned char device, unsigned char function) {
    unsigned int data = pci_read(bus, device, function, 0);
    return data & 0xFFFF;
}   // The vendor ID is in the lower 16 bits of the data read from offset 0. 

unsigned short pci_get_device(unsigned char bus, unsigned char device, unsigned char function) {
    unsigned int data = pci_read(bus, device, function, 0);
    return (data >> 16) & 0xFFFF;
}  // The device ID is in the upper 16 bits of the data read from offset 0.

unsigned char pci_get_class(unsigned char bus, unsigned char device, unsigned char function) {
    unsigned int data = pci_read(bus, device, function, 8);
    return (data >> 24) & 0xFF;
} // The class code is in the upper 8 bits of the data read from offset 8.

unsigned char pci_get_subclass(unsigned char bus, unsigned char device, unsigned char function) {
    unsigned int data = pci_read(bus, device, function, 8);
    return (data >> 16) & 0xFF;
} // The subclass code is in the middle 8 bits of the data read from offset 8.

unsigned char pci_get_prog_if(unsigned char bus, unsigned char device, unsigned char function) {
    unsigned int data = pci_read(bus, device, function, 8);
    return (data >> 8) & 0xFF;
} // The programming interface is in the lower 8 bits of the data read from offset 8. 

unsigned int pci_get_bar0(unsigned char bus, unsigned char device, unsigned char function) {
    unsigned int data = pci_read(bus, device, function, 0x10);
    return data;
} // The first Base Address Register (BAR0) is in the lower 8 bits of the data read from offset 16.

unsigned int pci_get_header_type(unsigned char bus, unsigned char device, unsigned char function) {
    unsigned int data = pci_read(bus, device, function, 0x0C);
    return (data >> 16) & 0xFF;
} // The header type is in the middle 8 bits of the data read from offset 12.

void pci_print(unsigned char bus, unsigned char device, unsigned char function, unsigned short vendor, unsigned short device_id, unsigned char class_code, unsigned char subclass, unsigned char prog_if, unsigned int bar0) {
    print("Bus: ");
    printNumber(bus);
    newLine();
    print("Device: ");
    printNumber(device);
    newLine();
    print("Function: ");
    printNumber(function);
    newLine();
    print("Vendor ID: ");
    printHex16(vendor);
    newLine();
    print("Device ID: ");
    printHex16(device_id);
    newLine();
    print("Class Code: ");
    printHex8(class_code);
    newLine();
    print("Subclass: ");
    printHex8(subclass);
    newLine();
    print("Programming Interface: ");
    printHex8(prog_if);
    newLine();
    print("BAR0: ");
    printHex(bar0);
    newLine();
    newLine();
} // A helper function to print the details of a PCI device. 