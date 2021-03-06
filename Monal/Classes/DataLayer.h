//
//  DataLayer.h
//  SworIM
//
//  Created by Anurodh Pokharel on 3/28/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <sqlite3.h>
#import "ParsePresence.h"
#import "NSString+SqlLite.h"

#define kMonalDBQueue "im.monal.dbQueue"
#define kMonalContactQueue "im.monal.contactQueue"

@interface DataLayer : NSObject {

	NSString* dbPath;
	sqlite3* database;
    NSLock* dbversionCheck;
    
    dispatch_queue_t _dbQueue ;
    dispatch_queue_t _contactQueue ;
}


extern NSString *const kAccountID;
extern NSString *const kAccountName;
extern NSString *const kDomain;
extern NSString *const kEnabled;

extern NSString *const kServer;
extern NSString *const kPort;
extern NSString *const kResource;
extern NSString *const kSSL;
extern NSString *const kOldSSL;
extern NSString *const kSelfSigned;
extern NSString *const kOauth;

extern NSString *const kUsername;
extern NSString *const kFullName;

extern NSString *const kContactName;
extern NSString *const kCount;


+ (DataLayer* )sharedInstance;

-(void) initDB;
-(void) version;

//lowest level command handlers
-(NSObject*) executeScalar:(NSString*) query  __deprecated;
-(NSArray*) executeReader:(NSString*) query __deprecated;
-(BOOL) executeNonQuery:(NSString*) query __deprecated;

// V2 low level
-(void) executeScalar:(NSString*) query withCompletion: (void (^)(NSObject *))completion;
-(void) executeReader:(NSString*) query withCompletion: (void (^)(NSMutableArray *))completion;
-(void) executeNonQuery:(NSString*) query withCompletion: (void (^)(BOOL))completion;

// Buddy Commands
-(void) addContact:(NSString*) contact  forAccount:(NSString*) accountNo fullname:(NSString*)fullName nickname:(NSString*) nickName withCompletion: (void (^)(BOOL))completion;
-(BOOL) removeBuddy:(NSString*) buddy forAccount:(NSString*) accountNo;
-(BOOL) clearBuddies:(NSString*) accountNo; 
-(void) contactForUsername:(NSString*) username forAccount: (NSString*) accountNo withCompletion: (void (^)(NSArray *))completion;

/**
 called when an account goes offline. removes all of its contacts state info
 */
-(BOOL) resetContactsForAccount:(NSString*) accountNo;
-(BOOL) resetContacts;


-(NSArray*) searchContactsWithString:(NSString*) search;

-(void) onlineContactsSortedBy:(NSString*) sort withCompeltion: (void (^)(NSMutableArray *))completion;
-(NSArray*) resourcesForContact:(NSString*)contact ;

-(void) offlineContactsWithCompeltion: (void (^)(NSMutableArray *))completion;

#pragma mark Ver string and Capabilities
-(BOOL) setResourceVer:(ParsePresence*)presenceObj forAccount: (NSString*) accountNo;
-(NSArray*) capsforVer:(NSString*) verString;
-(NSString*)getVerForUser:(NSString*)user Resource:(NSString*) resource;

-(BOOL) checkCap:(NSString*) cap forUser:(NSString*) user accountNo:(NSString*) acctNo;

-(BOOL)setFeature:(NSString*)feature  forVer:(NSString*) ver;

#pragma mark legacy caps
-(void) clearLegacyCaps;
//-(BOOL) setLegacyCap:(NSString*)cap forUser:(presence*)presenceObj accountNo:(NSString*) acctNo;
-(BOOL) checkLegacyCap:(NSString*)cap forUser:(NSString*) user accountNo:(NSString*) acctNo;

#pragma mark  presence functions
-(void) setResourceOnline:(ParsePresence*)presenceObj forAccount: (NSString*) accountNo;
-(void) setOnlineBuddy:(ParsePresence*)presenceObj forAccount: (NSString*) accountNo;
-(BOOL) setOfflineBuddy:(ParsePresence*)presenceObj forAccount: (NSString*) accountNo;

-(void) setBuddyStatus:(ParsePresence*)presenceObj forAccount: (NSString*) accountNo;
-(NSString*) buddyStatus:(NSString*) buddy forAccount:(NSString*) accountNo;

-(void) setBuddyState:(ParsePresence*)presenceObj forAccount: (NSString*) accountNo;
-(NSString*) buddyState:(NSString*) buddy forAccount:(NSString*) accountNo;


#pragma mark Contact info

-(void) setFullName:(NSString*) fullName forContact:(NSString*) contact andAccount:(NSString*) accountNo;
-(NSString*) fullName:(NSString*) buddy forAccount:(NSString*) accountNo; 
//-(BOOL) setFileName:(NSString*) fileName forBuddy:(NSString*) buddy andAccount:(NSString*) accountNo;

-(void) setContactHash:(ParsePresence*)presenceObj forAccount: (NSString*) accountNo;
-(void) contactHash:(NSString*) contact forAccount:(NSString*) accountNo withCompeltion: (void (^)(NSString *))completion;

-(void) isBuddyOnline:(NSString*) buddy forAccount:(NSString*) accountNo withCompletion: (void (^)(BOOL))completion;

-(bool) isBuddyMuc:(NSString*) buddy forAccount:(NSString*) accountNo;


-(bool) isBuddyAdded:(NSString*) buddy forAccount:(NSString*) accountNo ;
-(bool) isBuddyRemoved:(NSString*) buddy forAccount:(NSString*) accountNo ;

/**
 Calls with YES if contact  has laredy been added to the database for this account
 */
-(void) isContactInList:(NSString*) buddy forAccount:(NSString*) accountNo withCompletion: (void (^)(BOOL))completion;


//vcard commands
-(BOOL) setNickName:(NSString*) nickName forBuddy:(NSString*) buddy andAccount:(NSString*) accountNo;

//account commands
-(void) protocolListWithCompletion: (void (^)(NSArray* result))completion;
-(void) accountListWithCompletion: (void (^)(NSArray* result))completion;
-(NSArray*) enabledAccountList;
-(BOOL) isAccountEnabled:(NSString*) accountNo;

-(NSArray*) accountVals:(NSString*) accountNo; 

-(void) updateAccounWithDictionary:(NSDictionary *) dictionary andCompletion:(void (^)(BOOL))completion;
-(void) addAccountWithDictionary:(NSDictionary *) dictionary andCompletion: (void (^)(BOOL))completion;


-(BOOL) removeAccount:(NSString*) accountNo; 

/**
 disables account
 */
-(BOOL) disableEnabledAccount:(NSString*) accountNo;

#pragma mark message Commands
/**
 returns messages with the provided local id number 
 */
-(NSArray *) messageForHistoryID:(NSInteger) historyID;

/*
 adds a specified message to the database
 */
-(void) addMessageFrom:(NSString*) from to:(NSString*) to forAccount:(NSString*) accountNo withBody:(NSString*) message actuallyfrom:(NSString*) actualfrom delivered:(BOOL) delivered unread:(BOOL) unread serverMessageId:(NSString *) messageid andOverrideDate:(NSDate *) messageDate withCompletion: (void (^)(BOOL))completion;

/**
  checks to see if there is a message with the provided messageid. will return YES if the messageid exists for this account and contact
 */
-(void) hasMessageForId:(NSString*) messageid toContact:(NSString *) contact onAccount:(NSString *) accountNo andCompletion: (void (^)(BOOL))completion;

/*
 marks a message as delivered
 */
-(void) setMessageId:(NSString*) messageid delivered:(BOOL) delivered;

-(void) clearMessages:(NSString*) accountNo;
-(void) deleteMessageHistory:(NSString*) messageNo;

#pragma mark message history
-(NSMutableArray*) messageHistory:(NSString*) buddy forAccount:(NSString*) accountNo;
-(NSArray*) messageHistoryAll:(NSString*) buddy forAccount:(NSString*) accountNo; 

-(NSArray*) messageHistoryListDates:(NSString*) buddy forAccount: (NSString*) accountNo;
-(NSArray*) messageHistoryDate:(NSString*) buddy forAccount:(NSString*) accountNo forDate:(NSString*) date;


/**
 retrieves the date of the the last message to or from this contact
 */
-(void) lastMessageDateForContact:(NSString*) contact andAccount:(NSString*) accountNo withCompletion: (void (^)(NSDate *))completion;;


-(BOOL) messageHistoryClean:(NSString*) buddy :(NSString*) accountNo;
-(BOOL) messageHistoryCleanAll;

-(NSArray*) messageHistoryBuddies:(NSString*) accountNo;
-(void) markAsReadBuddy:(NSString*) buddy forAccount:(NSString*) accountNo;
-(void) addMessageHistoryFrom:(NSString*) from to:(NSString*) to forAccount:(NSString*) accountNo withMessage:(NSString*) message actuallyFrom:(NSString*) actualfrom withId:(NSString *)messageId withCompletion:(void (^)(BOOL))completion;

#pragma mark active contacts
-(void) activeContactsWithCompletion: (void (^)(NSMutableArray *))completion;
-(void) removeActiveBuddy:(NSString*) buddyname forAccount:(NSString*) accountNo;
-(void) removeAllActiveBuddies;
-(void) addActiveBuddies:(NSString*) buddyname forAccount:(NSString*) accountNo withCompletion: (void (^)(BOOL))completion;
-(void) isActiveBuddy:(NSString*) buddyname forAccount:(NSString*) accountNo withCompletion: (void (^)(BOOL))completion;


#pragma mark count unread
-(void) countUserUnreadMessages:(NSString*) buddy forAccount:(NSString*) accountNo withCompletion: (void (^)(NSNumber *))completion;
-(void) countUnreadMessagesWithCompletion: (void (^)(NSNumber *))completion;

-(void) countUserMessages:(NSString*) buddy forAccount:(NSString*) accountNo withCompletion: (void (^)(NSNumber *))completion;


@end
