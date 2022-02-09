// Copyright © 2018 Igor Pener. All rights reserved.

#import "AppDelegate.h"

@interface AppDelegate ()

@end

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {

    if (self.window == nil) {
        self.window = [[UIWindow alloc] initWithFrame:UIScreen.mainScreen.bounds];
        self.window.contentScaleFactor = UIScreen.mainScreen.scale;
        self.window.clipsToBounds = YES;
        self.window.rootViewController = [UIViewController new];
        [self.window makeKeyAndVisible];

        NSArray *glyphs = @[
            @"!", @"%", @"&", @"'", @"+", @"-", @",", @".", @"/", @"?", @"—", @"×", @"$", @"£", @"€",
            @"0", @"1", @"2", @"3", @"4", @"5", @"6", @"7", @"8", @"9",
            @"A", @"B", @"C", @"D", @"E", @"F", @"G", @"H", @"I", @"J", @"K", @"L", @"M",
            @"N", @"O", @"P", @"Q", @"R", @"S", @"T", @"U", @"V", @"W", @"X", @"Y", @"Z",
            @"Ä", @"Ö", @"Ü", @"ß"
        ];

        NSMutableString *map = [NSMutableString new];
        [map appendString:@"const std::unordered_map<wchar_t, const ui_font::character_uv &> font {\n"];

        __block const CGSize size = {2048.0, 2048.0};
        __block NSFileManager *fileManager = [NSFileManager defaultManager];

        CGRect(^drawGlyph)(UIView *, UIColor *, NSString *, CGPoint *, UIFontWeight) = ^CGRect(UIView *view, UIColor *color, NSString *glyph, CGPoint *origin, UIFontWeight weight) {
            UILabel *label = [UILabel new];
            label.font = [UIFont systemFontOfSize:214.5f weight:weight];
            label.textColor = color;
            label.textAlignment = NSTextAlignmentCenter;
            label.text = glyph;
            label.backgroundColor = [UIColor clearColor];
            [label sizeToFit];

            CGRect frame = label.frame;
            frame.origin = *origin;
            origin->x += frame.size.width;

            if (origin->x > size.width) {
                origin->x = 0.f;
                origin->y += frame.size.height;
                frame.origin = *origin;
                origin->x += frame.size.width;
            }
            label.frame = frame;

            [view addSubview:label];

            return CGRectMake(frame.origin.x / size.width, frame.origin.y / size.height, frame.size.width / size.width, frame.size.height / size.height);
        };

        void(^saveImage)(UIView *, NSString *) = ^(UIView *view, NSString *name) {
            view.backgroundColor = [UIColor clearColor];

            UIGraphicsBeginImageContextWithOptions(size, NO, 1.f);
            [view.layer renderInContext:UIGraphicsGetCurrentContext()];
            UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
            UIGraphicsEndImageContext();

            [fileManager createFileAtPath:[NSString stringWithFormat:@OUTPUT_DIR "%@.png", name]
                                 contents:UIImagePNGRepresentation(image)
                               attributes:nil];
        };

        UIView *view0 = [[UIView alloc] initWithFrame:CGRectMake(0.f, 0.f, size.width, size.height)];
        UIView *view1 = [[UIView alloc] initWithFrame:CGRectMake(0.f, 0.f, size.width, size.height)];
        UIView *view2 = [[UIView alloc] initWithFrame:CGRectMake(0.f, 0.f, size.width, size.height)];
        CGPoint origin0 = CGPointZero, origin1 = CGPointZero, origin2 = CGPointZero;

        for (NSString *glyph in glyphs) {
            CGRect rect0 = drawGlyph(view0, [UIColor redColor], glyph, &origin0, UIFontWeightUltraLight);
            CGRect rect1 = drawGlyph(view1, [UIColor greenColor], glyph, &origin1, UIFontWeightLight);
            CGRect rect2 = drawGlyph(view2, [UIColor blueColor], glyph, &origin2, UIFontWeightMedium);

            [map appendString:
             [NSString stringWithFormat:
              @"    {L'%@', tex_coords({{%ff, %ff}, {%ff, %ff}}, {{%ff, %ff}, {%ff, %ff}}, {{%ff, %ff}, {%ff, %ff}})},\n",
              [glyph characterAtIndex:0] == '\'' ? @"\\'" : glyph,
              rect0.origin.x, rect0.origin.y, rect0.size.width, rect0.size.height,
              rect1.origin.x, rect1.origin.y, rect1.size.width, rect1.size.height,
              rect2.origin.x, rect2.origin.y, rect2.size.width, rect2.size.height]];
        }

        saveImage(view0, @"thin");
        saveImage(view1, @"regular");
        saveImage(view2, @"thick");

        [map replaceCharactersInRange:NSMakeRange(map.length - 2, 1) withString:@"\n};"];
        [fileManager createFileAtPath:@OUTPUT_DIR @"texture_atlas.cpp"
                             contents:[map dataUsingEncoding:NSUTF8StringEncoding]
                           attributes:nil];
    }

    return YES;
}

@end
