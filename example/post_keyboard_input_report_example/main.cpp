#include "karabiner_virtual_hid_device_methods.hpp"
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDDevice.h>
#include <IOKit/hid/IOHIDElement.h>
#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/hid/IOHIDQueue.h>
#include <IOKit/hid/IOHIDValue.h>
#include <IOKit/hidsystem/IOHIDShared.h>
#include <IOKit/hidsystem/ev_keymap.h>
#include <cmath>
#include <iostream>
#include <thread>

int main(int argc, const char* argv[]) {
  if (getuid() != 0) {
    std::cerr << "virtual_keyboard_example requires root privilege." << std::endl;
  }

  kern_return_t kr;
  io_connect_t connect = IO_OBJECT_NULL;
  auto service = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceNameMatching(pqrs::karabiner_virtual_hid_device::get_virtual_hid_root_name()));
  if (!service) {
    std::cerr << "IOServiceGetMatchingService error" << std::endl;
    goto finish;
  }

  kr = IOServiceOpen(service, mach_task_self(), kIOHIDServerConnectType, &connect);
  if (kr != KERN_SUCCESS) {
    std::cerr << "IOServiceOpen error" << std::endl;
    goto finish;
  }

  // keyboard_input

  {
    pqrs::karabiner_virtual_hid_device::properties::keyboard_initialization properties;
    properties.keyboard_type = pqrs::karabiner_virtual_hid_device::properties::keyboard_type::ansi;
    kr = pqrs::karabiner_virtual_hid_device_methods::initialize_virtual_hid_keyboard(connect, properties);
    if (kr != KERN_SUCCESS) {
      std::cerr << "initialize_virtual_hid_keyboard error" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  for (int i = 0; i < 12; ++i) {
    pqrs::karabiner_virtual_hid_device::hid_report::keyboard_input report;
    switch (i % 6) {
    case 0:
      report.keys[0] = kHIDUsage_KeyboardA;
      break;
    case 1:
      report.keys[0] = kHIDUsage_KeyboardB;
      break;
    case 2:
      report.keys[0] = kHIDUsage_KeyboardC;
      break;
    case 3:
      report.keys[0] = kHIDUsage_KeyboardD;
      break;
    case 4:
      report.keys[0] = kHIDUsage_KeyboardE;
      break;
    case 5:
      // Send empty report
      break;
    }

    kr = pqrs::karabiner_virtual_hid_device_methods::post_keyboard_input_report(connect, report);
    if (kr != KERN_SUCCESS) {
      std::cerr << "post_keyboard_input_report error" << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  // consumer_input (option+mute)

  {
    pqrs::karabiner_virtual_hid_device::hid_report::keyboard_input report;
    report.modifiers = pqrs::karabiner_virtual_hid_device::hid_report::keyboard_input::modifier::left_option;

    kr = pqrs::karabiner_virtual_hid_device_methods::post_keyboard_input_report(connect, report);
    if (kr != KERN_SUCCESS) {
      std::cerr << "post_keyboard_input_report error" << std::endl;
    }
  }
  {
    pqrs::karabiner_virtual_hid_device::hid_report::consumer_input report;
    report.keys[0] = kHIDUsage_Csmr_Mute;

    kr = pqrs::karabiner_virtual_hid_device_methods::post_consumer_input_report(connect, report);
    if (kr != KERN_SUCCESS) {
      std::cerr << "post_consumer_input_report error" << std::endl;
    }
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  // apple_vendor_top_case_input (fn+spacebar)

  {
    pqrs::karabiner_virtual_hid_device::hid_report::apple_vendor_top_case_input report;
    report.keys[0] = 0x0003; // kHIDUsage_AV_TopCase_KeyboardFn

    kr = pqrs::karabiner_virtual_hid_device_methods::post_apple_vendor_top_case_input_report(connect, report);
    if (kr != KERN_SUCCESS) {
      std::cerr << "post_apple_vendor_top_case_input_report error" << std::endl;
    }
  }
  {
    pqrs::karabiner_virtual_hid_device::hid_report::keyboard_input report;
    report.keys[0] = kHIDUsage_KeyboardSpacebar;

    kr = pqrs::karabiner_virtual_hid_device_methods::post_keyboard_input_report(connect, report);
    if (kr != KERN_SUCCESS) {
      std::cerr << "post_keyboard_input_report error" << std::endl;
    }
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  // apple_vendor_keyboard_input (command+mission_control)

  {
    pqrs::karabiner_virtual_hid_device::hid_report::keyboard_input report;
    report.modifiers = pqrs::karabiner_virtual_hid_device::hid_report::keyboard_input::modifier::left_command;

    kr = pqrs::karabiner_virtual_hid_device_methods::post_keyboard_input_report(connect, report);
    if (kr != KERN_SUCCESS) {
      std::cerr << "post_keyboard_input_report error" << std::endl;
    }
  }

  {
    pqrs::karabiner_virtual_hid_device::hid_report::apple_vendor_keyboard_input report;
    report.keys[0] = 0x0010; // kHIDUsage_AppleVendorKeyboard_Expose_All

    kr = pqrs::karabiner_virtual_hid_device_methods::post_apple_vendor_keyboard_input_report(connect, report);
    if (kr != KERN_SUCCESS) {
      std::cerr << "post_keyboard_input_report error" << std::endl;
    }
  }

  kr = pqrs::karabiner_virtual_hid_device_methods::reset_virtual_hid_keyboard(connect);
  if (kr != KERN_SUCCESS) {
    std::cerr << "reset_virtual_hid_keyboard error" << std::endl;
  }

finish:
  if (connect) {
    IOServiceClose(connect);
  }
  if (service) {
    IOObjectRelease(service);
  }

  return 0;
}
