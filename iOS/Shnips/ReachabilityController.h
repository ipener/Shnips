// Copyright © 2018 Igor Pener. All rights reserved.

#import <Foundation/Foundation.h>

@interface ReachabilityController : NSObject

- (instancetype)initWithURL:(NSString *)url andAction:(void(^)(BOOL, BOOL))action;

@end
