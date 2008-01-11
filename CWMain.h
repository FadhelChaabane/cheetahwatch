/* CheetahWatch, v1.2
 * Copyright (c) 2007-2008 Patrick Quinn-Graham
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>

#include <IOKit/IOKitLib.h>
#include <IOKit/serial/IOSerialKeys.h>
#include <IOKit/IOBSD.h>
#include <IOKit/IOMessage.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/usb/IOUSBLib.h>
#import <Sparkle/Sparkle.h>

#include <mach/mach.h>
#include <unistd.h>
#include <termios.h>

#include <SystemConfiguration/SCNetworkConnection.h>

//#import "StyledWindow.h"

#import "CWHistorySupport.h"

#define BAUDRATE B9600
#define MODEMUIDEV "/dev/tty.HUAWEIMobile-Pcui"
#define BUFSIZE 256

#define kMyVendorID		4817
#define kMyProductID	4099
 
enum
{
    kNumRetries = 3
};

typedef struct MyPrivateData {
    io_object_t			notification;
    IOUSBDeviceInterface *	*deviceInterface;
    CFStringRef			deviceName;
    UInt32			locationID;
} MyPrivateData;

@interface CWMain : NSObject
{
    IBOutlet id mode;
    IBOutlet id signal;
    IBOutlet id speedReceive;
    IBOutlet id speedTransmit;
    IBOutlet id status;	
	IBOutlet id theWindow;
    IBOutlet id transferReceive;
    IBOutlet id transferTransmit;
    IBOutlet id uptime;
	IBOutlet id appMenu;	
	IBOutlet id totalReceived;
	IBOutlet id totalTransmitted;	
	IBOutlet id menuStoreUsageHistory;
	IBOutlet id menuClearUsageHistory;
	IBOutlet id firstRunWindow;
	IBOutlet id firstRunWebkit;
	IBOutlet id statusItemMenu;
	IBOutlet id statusItemConectedFor;
	IBOutlet id statusItemConnect;
	IBOutlet id statusItemDisconnect;
	IBOutlet SUUpdater *sparkler;
	
	IBOutlet id modemInfoWindow;
	IBOutlet id modemInfoHWVersion;
	IBOutlet id modemInfoNetwork;
	IBOutlet id modemInfoAPN;
	IBOutlet id modemInfoIMEI;
	IBOutlet id modemInfoIMSI;
	
	IBOutlet id carrierSeperator;
	IBOutlet id carrierInMenu;
	
	bool weHaveAModem;
	NSStatusItem *statusItem;

 	CWHistorySupport *cwh;
	
	NSNumber *currentUptime;
	NSNumber *currentSpeedReceive;
	NSNumber *currentSpeedTransmit;
	NSNumber *currentTransmitted;
	NSNumber *currentReceived;
	
	// Relating to PPP connect/disconnecting	
	SCNetworkConnectionRef scncRef;
	CFDictionaryRef userOptions;		
	SCNetworkConnectionContext gScncCtx;
	SCNetworkConnectionStatus gStat;
	
	BOOL waitingOnCarrierName;
}

-(void)setupDialing;

-(void)showFirstRun;

-(void)startMonitor:(id)sender;
-(void)clearAllUI;
-(void)clearConnectionUI;
-(void)updateHistory;
-(void)makeMenuMatchStorageHistory;

-(BOOL)storeUsageHistory;
-(void)toggleStoreUsageHistory;

-(void)changeStatusImageTo:(NSString*)which;

-(NSString*)prettyDataAmount:(int)bytes;

-(void)noModem:(id)sender;
-(void)haveModem;

-(void)storeUsageHistory:(id)sender;
-(void)clearUsageHistory:(id)sender;

-(void)connectNetwork:(id)sender;
-(void)disconnectNetwork:(id)sender;

-(void)showModemInfo:(id)sender;
-(void)clickMenu:(id)sender;
-(void)showAbout:(id)sender;
-(void)checkUpdates:(id)sender;

-(void)signalStrength:(char*)buff;
-(void)modeChange:(char*)buff;
-(void)flowReport:(char*)buff;
-(NSString*)GetATResult:(NSString*)command forDev:(int)dev;


-(void)setAPN:(NSString*)theApn;
-(void)setHardwareVersion:(NSString*)theVersion;


-(void)gotHWVersion:(char*)buff;

-(void)gotAPN:(char*)buff;
-(void)gotCarrier:(char*)buff;

-(void)setIMEI:(NSString*)theIMEI;

-(NSString*)GetATResult:(NSString*)command forDev:(int)dev;
-(void)sendATCommand:(NSString*)command toDevice:(int)dev;

-(void)sendATCommandsTimerAction:(id)thing;
-(void)sendATCommandsTimer:(id)thing;

#pragma mark Modem interface thread

+(void)MyRunner:(id)mainController;

-(void)doSetSignalStrength:(int)z_signal;
-(void)signalStrengthFromCSQ:(char*)buff;

+(void)USBFinder:(id)mainController;

void DeviceAdded(void *refCon, io_iterator_t iterator);

void calloutProc (SCNetworkConnectionRef connection, SCNetworkConnectionStatus status, void *info );

@end


static IONotificationPortRef	gNotifyPort;
static io_iterator_t	gAddedIter;
static CFRunLoopRef		gRunLoop;
static CWMain			*gCWMain;
static fd; // the connection to the modem