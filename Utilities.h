#import <UIKit/UIKit.h>
#import <RemoteLog.h> // For debugging
#import <rootless.h>
#import <version.h>

#define PREF_CHANGED_NOTIF "SNMessenger/prefChanged"

extern BOOL isDarkMode;
extern NSBundle *tweakBundle;

static inline NSString *localizedStringForKey(NSString *key) {
    return [tweakBundle localizedStringForKey:key value:nil table:nil];
}

static inline NSString *getSettingsPlistPath() {
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    NSString *plistPath = [documentsDirectory stringByAppendingPathComponent:@"SNMessenger.plist"];
    return plistPath;
}

static inline NSMutableDictionary *getCurrentSettings() {
    return [[NSMutableDictionary alloc] initWithContentsOfFile:getSettingsPlistPath()] ?: [@{} mutableCopy];
}

static inline NSMutableDictionary *compareDictionaries(NSDictionary *oldDict, NSDictionary *newDict) {
    NSMutableDictionary *result = [NSMutableDictionary dictionary];

    [newDict enumerateKeysAndObjectsUsingBlock:^(NSString *key, id newDictObj, BOOL *stop) {
        id oldDictObj = oldDict[key];

        if (!oldDictObj || ![newDictObj isEqual:oldDictObj]) {
            result[key] = newDictObj;
        }
    }];

    return result;
}

static inline UIImage *getImage(NSString *name) {
    NSString *path = [tweakBundle pathForResource:name ofType:@"png"];
    return [UIImage imageWithContentsOfFile:path];
}

static inline UIImage *getTemplateImage(NSString *name) {
    return [getImage(name) imageWithRenderingMode:UIImageRenderingModeAlwaysTemplate];
}

static inline CGFloat colorComponentFrom(NSString *string, NSUInteger start, NSUInteger length) {
    NSString *substring = [string substringWithRange:NSMakeRange(start, length)];
    NSString *fullHex = length == 2 ? substring : [NSString stringWithFormat: @"%@%@", substring, substring];
    unsigned int hexComponent;
    [[NSScanner scannerWithString:fullHex] scanHexInt:&hexComponent];

    return hexComponent / 255.0f;
}

static inline UIColor *colorWithHexString(NSString *hexString) {
    NSString *colorString = [hexString uppercaseString];

    CGFloat alpha, red, blue, green = 0.0f;

    // #RGBA
    red   = colorComponentFrom(colorString, 1, 2);
    green = colorComponentFrom(colorString, 3, 2);
    blue  = colorComponentFrom(colorString, 5, 2);
    alpha = [hexString length] == 9 ? colorComponentFrom(colorString, 7, 2) : 1.0f;

    return [UIColor colorWithRed:red green:green blue:blue alpha:alpha];
}

#pragma clang diagnostic ignored "-Wunguarded-availability-new" // THEOS should handle this by default
#pragma clang diagnostic push

static inline BOOL hasNotchOrDynamicIsland() {
    UIWindow *keyWindow = [UIApplication sharedApplication].keyWindow;

    if (IS_IOS_OR_NEWER(iOS_13_0)) {
        UIWindowScene *windowScene = (UIWindowScene *)[[[UIApplication sharedApplication].connectedScenes allObjects] firstObject];
        keyWindow = [windowScene.windows firstObject];
    }

    return keyWindow.safeAreaInsets.bottom > 0 || keyWindow.safeAreaInsets.top >= 51;
}

#pragma clang diagnostic pop