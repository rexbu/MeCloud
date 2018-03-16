/**
 * file :	McZip.mm
 * author :	bushaofeng
 * create :	2016-08-25 17:20
 * func : 
 * history:
 */

#include "McZip.h"
#include "unzip.h"
#include "zip.h"
#import "zlib.h"
#import "zconf.h"
#include "Common.h"
#import <Foundation/Foundation.h>

using namespace mc;
static NSDate* _dateWithMSDOSFormat(UInt32 msdosDateTime);

bool Zip::unzip(const char* upath, const char* udestination, ZipCallback* callback, bool overwrite ,
    const char* upassword, ZipArchiveCallback* delegate){
    NSString* path = [NSString stringWithUTF8String:upath];
    NSString* destination = [NSString stringWithUTF8String:udestination];
    NSString* password = nil;
    if (upassword!=NULL) {
        password = [NSString stringWithUTF8String:upassword];
    }
    
	zipFile zip = unzOpen((const char*)[path UTF8String]);
    if (zip == NULL)
    {
        const char* err = "failed to open zip file";
        err_log("%s: %s", err, upath);
        if (callback) {
            callback->complete(NULL, false, err);
        }
        
        return false;
    }
    
    NSDictionary * fileAttributes = [[NSFileManager defaultManager] attributesOfItemAtPath:path error:nil];
    unsigned long long fileSize = [fileAttributes[NSFileSize] unsignedLongLongValue];
    unsigned long long currentPosition = 0;
    
    unz_global_info  globalInfo = {0ul, 0ul};
    unzGetGlobalInfo(zip, &globalInfo);
    
    // Begin unzipping
    if (unzGoToFirstFile(zip) != UNZ_OK)
    {
        const char* err = "failed to open first file in zip file";
        err_log("%s", err);
        if (callback)
        {
            callback->complete(nil, NO, err);
        }
        return NO;
    }
    
    BOOL success = YES;
    BOOL canceled = NO;
    int ret = 0;
    int crc_ret =0;
    unsigned char buffer[4096] = {0};
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSMutableSet *directoriesModificationDates = [[NSMutableSet alloc] init];
    
    if (delegate!=NULL) {
        delegate->willUnzip(upath, globalInfo);
        delegate->progress(currentPosition, fileSize);
    }
    
    uint32_t currentFileNumber = 0;
    do {
        @autoreleasepool {
            if ([password length] == 0) {
                ret = unzOpenCurrentFile(zip);
            } else {
                ret = unzOpenCurrentFilePassword(zip, [password cStringUsingEncoding:NSASCIIStringEncoding]);
            }
            
            if (ret != UNZ_OK) {
                success = NO;
                break;
            }
            
            // Reading data and write to file
            unz_file_info fileInfo;
            memset(&fileInfo, 0, sizeof(unz_file_info));
            
            ret = unzGetCurrentFileInfo(zip, &fileInfo, NULL, 0, NULL, 0, NULL, 0);
            if (ret != UNZ_OK) {
                success = NO;
                unzCloseCurrentFile(zip);
                break;
            }
            
            currentPosition += fileInfo.compressed_size;
            
            // Message delegate
            if (delegate != NULL) {
                if(!delegate->shouldUnzip(currentFileNumber, globalInfo.number_entry, upath, fileInfo)){
                    success = NO;
                    canceled = YES;
                    break;
                }
                delegate->willUnzip(currentFileNumber, globalInfo.number_entry, upath, fileInfo);
                delegate->progress(currentPosition, fileSize);
            }
            
            char *filename = (char *)malloc(fileInfo.size_filename + 1);
            if (filename == NULL)
            {
                return NO;
            }
            
            unzGetCurrentFileInfo(zip, &fileInfo, filename, fileInfo.size_filename + 1, NULL, 0, NULL, 0);
            filename[fileInfo.size_filename] = '\0';
            
            //
            // Determine whether this is a symbolic link:
            // - File is stored with 'version made by' value of UNIX (3),
            //   as per http://www.pkware.com/documents/casestudies/APPNOTE.TXT
            //   in the upper byte of the version field.
            // - BSD4.4 st_mode constants are stored in the high 16 bits of the
            //   external file attributes (defacto standard, verified against libarchive)
            //
            // The original constants can be found here:
            //    http://minnie.tuhs.org/cgi-bin/utree.pl?file=4.4BSD/usr/include/sys/stat.h
            //
            const uLong ZipUNIXVersion = 3;
            const uLong BSD_SFMT = 0170000;
            const uLong BSD_IFLNK = 0120000;
            
            BOOL fileIsSymbolicLink = NO;
            if (((fileInfo.version >> 8) == ZipUNIXVersion) && BSD_IFLNK == (BSD_SFMT & (fileInfo.external_fa >> 16))) {
                fileIsSymbolicLink = NO;
            }
            
            // Check if it contains directory
            NSString *strPath = @(filename);
            BOOL isDirectory = NO;
            if (filename[fileInfo.size_filename-1] == '/' || filename[fileInfo.size_filename-1] == '\\') {
                isDirectory = YES;
            }
            free(filename);
            
            // Contains a path
            if ([strPath rangeOfCharacterFromSet:[NSCharacterSet characterSetWithCharactersInString:@"/\\"]].location != NSNotFound) {
                strPath = [strPath stringByReplacingOccurrencesOfString:@"\\" withString:@"/"];
            }
            
            NSString *fullPath = [destination stringByAppendingPathComponent:strPath];
            NSError *err = nil;
            NSDate *modDate = _dateWithMSDOSFormat(fileInfo.dosDate);
            NSDictionary *directoryAttr = @{NSFileCreationDate: modDate, NSFileModificationDate: modDate};
            
            if (isDirectory) {
                [fileManager createDirectoryAtPath:fullPath withIntermediateDirectories:YES attributes:directoryAttr  error:&err];
            } else {
                [fileManager createDirectoryAtPath:[fullPath stringByDeletingLastPathComponent] withIntermediateDirectories:YES attributes:directoryAttr error:&err];
            }
            if (nil != err) {
                err_log("MeZip Error: %s", [err.localizedDescription UTF8String]);
            }
            
            if(!fileIsSymbolicLink)
                [directoriesModificationDates addObject: @{@"path": fullPath, @"modDate": modDate}];
            
            if ([fileManager fileExistsAtPath:fullPath] && !isDirectory && !overwrite) {
                //FIXME: couldBe CRC Check?
                unzCloseCurrentFile(zip);
                ret = unzGoToNextFile(zip);
                continue;
            }
            
            if (!fileIsSymbolicLink) {
                FILE *fp = fopen((const char*)[fullPath UTF8String], "wb");
                while (fp) {
                    int readBytes = unzReadCurrentFile(zip, buffer, 4096);
                    
                    if (readBytes > 0) {
                        fwrite(buffer, readBytes, 1, fp );
                    } else {
                        break;
                    }
                }
                
                if (fp) {
                    if ([[[fullPath pathExtension] lowercaseString] isEqualToString:@"zip"]) {
                        NSLog(@"Unzipping nested .zip file:  %@", [fullPath lastPathComponent]);
                        if (unzip([fullPath UTF8String], [[fullPath stringByDeletingLastPathComponent] UTF8String], NULL, overwrite, upassword)) {
                            [[NSFileManager defaultManager] removeItemAtPath:fullPath error:nil];
                        }
                    }
                    
                    fclose(fp);
                    
                    // Set the original datetime property
                    if (fileInfo.dosDate != 0) {
                        NSDate *orgDate = _dateWithMSDOSFormat(fileInfo.dosDate);
                        NSDictionary *attr = @{NSFileModificationDate: orgDate};
                        
                        if (attr) {
                            if ([fileManager setAttributes:attr ofItemAtPath:fullPath error:nil] == NO) {
                                // Can't set attributes
                                NSLog(@"[SSZipArchive] Failed to set attributes - whilst setting modification date");
                            }
                        }
                    }
                    
                    // Set the original permissions on the file
                    uLong permissions = fileInfo.external_fa >> 16;
                    if (permissions != 0) {
                        // Store it into a NSNumber
                        NSNumber *permissionsValue = @(permissions);
                        
                        // Retrieve any existing attributes
                        NSMutableDictionary *attrs = [[NSMutableDictionary alloc] initWithDictionary:[fileManager attributesOfItemAtPath:fullPath error:nil]];
                        
                        // Set the value in the attributes dict
                        attrs[NSFilePosixPermissions] = permissionsValue;
                        
                        // Update attributes
                        if ([fileManager setAttributes:attrs ofItemAtPath:fullPath error:nil] == NO) {
                            // Unable to set the permissions attribute
                            NSLog(@"[SSZipArchive] Failed to set attributes - whilst setting permissions");
                        }
                        
#if !__has_feature(objc_arc)
                        [attrs release];
#endif
                    }
                }
            }
            else
            {
                // Assemble the path for the symbolic link
                NSMutableString* destinationPath = [NSMutableString string];
                int bytesRead = 0;
                while((bytesRead = unzReadCurrentFile(zip, buffer, 4096)) > 0)
                {
                    buffer[bytesRead] = (int)0;
                    [destinationPath appendString:@((const char*)buffer)];
                }
                
                // Create the symbolic link (making sure it stays relative if it was relative before)
                int symlinkError = symlink([destinationPath cStringUsingEncoding:NSUTF8StringEncoding],
                                           [fullPath cStringUsingEncoding:NSUTF8StringEncoding]);
                
                if(symlinkError != 0)
                {
                    NSLog(@"Failed to create symbolic link at \"%@\" to \"%@\". symlink() error code: %d", fullPath, destinationPath, errno);
                }
            }
            
            crc_ret = unzCloseCurrentFile( zip );
            if (crc_ret == UNZ_CRCERROR) {
                //CRC ERROR
                success = NO;
                break;
            }
            ret = unzGoToNextFile( zip );
            
            // Message delegate
            if (delegate!=NULL) {
                delegate->didUnzip(currentFileNumber, (uint32_t)globalInfo.number_entry, upath, fileInfo);
                delegate->didUnzip(currentFileNumber, (uint32_t)globalInfo.number_entry, upath, [fullPath UTF8String]);
            }
            
            currentFileNumber++;
            if (callback!=NULL)
            {
                callback->progress([strPath UTF8String], fileInfo, currentFileNumber, globalInfo.number_entry);
            }
        }
    } while(ret == UNZ_OK && ret != UNZ_END_OF_LIST_OF_FILE);
    
    // Close
    unzClose(zip);
    
    // The process of decompressing the .zip archive causes the modification times on the folders
    // to be set to the present time. So, when we are done, they need to be explicitly set.
    // set the modification date on all of the directories.
    NSError * err = nil;
    for (NSDictionary * d in directoriesModificationDates) {
        if (![[NSFileManager defaultManager] setAttributes:@{NSFileModificationDate: d[@"modDate"]} ofItemAtPath:d[@"path"] error:&err]) {
            NSLog(@"[SSZipArchive] Set attributes failed for directory: %@.", d[@"path"]);
        }
        if (err) {
            NSLog(@"[SSZipArchive] Error setting directory file modification date attribute: %@",err.localizedDescription);
        }
    }
    
#if !__has_feature(objc_arc)
    [directoriesModificationDates release];
#endif
    
    // Message delegate
    if (success && delegate!=NULL) {
        delegate->didUnzip(upath, globalInfo, udestination);
        delegate->progress(fileSize, fileSize);
    }
    
    const char *retErr = NULL;
    if (crc_ret == UNZ_CRCERROR)
    {
        retErr = "crc check failed for file";
    }
    
    if (callback!=NULL) {
        callback->complete(upath, success, retErr);
    }
    
    return success;
}

NSDate* _dateWithMSDOSFormat(UInt32 msdosDateTime){
    static const UInt32 kYearMask = 0xFE000000;
    static const UInt32 kMonthMask = 0x1E00000;
    static const UInt32 kDayMask = 0x1F0000;
    static const UInt32 kHourMask = 0xF800;
    static const UInt32 kMinuteMask = 0x7E0;
    static const UInt32 kSecondMask = 0x1F;
    
    static NSCalendar *gregorian;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
#if defined(__IPHONE_8_0) || defined(__MAC_10_10)
        gregorian = [[NSCalendar alloc] initWithCalendarIdentifier:NSCalendarIdentifierGregorian];
#else
        gregorian = [[NSCalendar alloc] initWithCalendarIdentifier:NSGregorianCalendar];
#endif
    });
    
    NSDateComponents *components = [[NSDateComponents alloc] init];
    
    //NSAssert(0xFFFFFFFF == (kYearMask | kMonthMask | kDayMask | kHourMask | kMinuteMask | kSecondMask), @"[SSZipArchive] MSDOS date masks don't add up");
    
    [components setYear:1980 + ((msdosDateTime & kYearMask) >> 25)];
    [components setMonth:(msdosDateTime & kMonthMask) >> 21];
    [components setDay:(msdosDateTime & kDayMask) >> 16];
    [components setHour:(msdosDateTime & kHourMask) >> 11];
    [components setMinute:(msdosDateTime & kMinuteMask) >> 5];
    [components setSecond:(msdosDateTime & kSecondMask) * 2];
    
    NSDate *date = [NSDate dateWithTimeInterval:0 sinceDate:[gregorian dateFromComponents:components]];
    
#if !__has_feature(objc_arc)
    [components release];
#endif
    
    return date;
}
