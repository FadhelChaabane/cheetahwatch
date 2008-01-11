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

@interface CWHistorySupport : NSObject {
	NSManagedObjectModel *model;
	NSPersistentStoreCoordinator *coordinator;
	NSError *coreDataError;
	NSManagedObjectContext *managedObjectContext;
	
	BOOL firstRunSinceStartup;
	NSManagedObject *activeConnection;

	NSObject *mainController; 

	NSTimer *cheezburgerWatch;
	NSString *theUptime;
	
	int cachedSent, cachedRecv, cachedTotalSent, cachedTotalRecv;
}

-(void)setMainController:(NSObject*)cont;
-(void)iCanHasCheezburger:(NSTimer*)timer;
-(void)setupCoreData;
-(NSManagedObject*)doWeHaveAnUnclosedConnection;
-(NSManagedObject*)newConnection:(NSNumber*)connectedTime;
-(void)markConnectionAsClosed;
-(void)flowReportSeconds:(NSNumber*)connected withTransmitRate:(NSNumber*)transmit receiveRate:(NSNumber*)receive totalSent:(NSNumber*)sent andTotalReceived:(NSNumber*)received;
-(void)calculateTotalUsage;
-(void)calculateTotalUsageForCaching;
-(int)cachedTotalSent;
-(int)cachedTotalRecv;

-(void)clearHistory;

@end
