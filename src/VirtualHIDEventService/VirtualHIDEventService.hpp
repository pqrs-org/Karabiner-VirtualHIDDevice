#pragma once

#include "DiagnosticMacros.hpp"
#include "VersionSignature.hpp"

BEGIN_IOKIT_INCLUDE;
#include "10.11/include/IOKit/hidevent/IOHIDEventService.h"
END_IOKIT_INCLUDE;

class VIRTUAL_HID_EVENT_SERVICE_CLASS final : public IOHIDEventService {
  OSDeclareDefaultStructors(VIRTUAL_HID_EVENT_SERVICE_CLASS);

public:
  virtual bool handleStart(IOService* provider) override;
  virtual void handleStop(IOService* provider) override;
  virtual OSArray* getReportElements(void) override { return reportElements_; }

  void dispatchKeyboardEvent(UInt32 usagePage, UInt32 usage, UInt32 value);

private:
  OSArray* reportElements_;
};
