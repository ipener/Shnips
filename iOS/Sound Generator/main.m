// Copyright © 2018 Igor Pener. All rights reserved.

#import <AudioToolbox/AudioToolbox.h>
#import <Foundation/Foundation.h>

#include <memory.h>

void generateSound(const char *filename) {
    NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:@ (RESOURCES_DIR "%s" ".wav"), filename]];

    AudioFileID fileId;
    OSStatus status;

    if ((status = AudioFileOpenURL((__bridge CFURLRef)url, kAudioFileReadPermission, 0, &fileId)))
        return NSLog(@"Failed to open %s, status: %d", filename, status);

    UInt64 propertyData = 0;
    UInt32 size = sizeof(propertyData);

    if ((status = AudioFileGetProperty(fileId, kAudioFilePropertyAudioDataByteCount, &size, &propertyData)))
        return NSLog(@"Failed to determine the size of %s, status: %d", filename, status);

    size = (UInt32)(propertyData);
    void *data = malloc(size);

    if ((status = AudioFileReadBytes(fileId, false, 0, &size, data)))
        return NSLog(@"Failed to read data from %s, status: %d", filename, status);

    if ((status = AudioFileClose(fileId)))
        return NSLog(@"Failed to close file %s, status: %d", filename, status);

    NSData *contents = [NSData dataWithBytes:data length:size];
    NSString *path = [NSString stringWithFormat:@ (RESOURCES_DIR "%s" ".sound"), filename];

    if (![[NSFileManager defaultManager] createFileAtPath:path contents:contents attributes:nil])
        return NSLog(@"Failed to write sound file %s", filename);
}

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        generateSound("skill_shot_indirect_pass");
        generateSound("skill_shot_just_passed");
        generateSound("skill_shot_narrow_gap");
        generateSound("skill_shot_three_cushion_hit");
        generateSound("skill_shot_four_passes");
        generateSound("skill_shot_chips_close");
        generateSound("skill_shot_two_passes_no_cushion");
        generateSound("skill_shot_circumvent_chip");
        generateSound("skill_shot_hit_chips_before_pass");
        generateSound("achievement_unlocked");
        generateSound("goal_completed");
        generateSound("chip_passed");
        generateSound("chip_charged");
        generateSound("chip_hit");
        generateSound("cushion_hit");
        generateSound("cue_hit");
        generateSound("missed");
        generateSound("sliding");
    }
    return 0;
}
