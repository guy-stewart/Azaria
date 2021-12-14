//
//  ViewController.m
//  Azaria
//
//  Created by guy on 11/11/17.
//  Copyright © 2017 guy. All rights reserved.
//

#import "ImageHelper.h"
#import "ddraw_ios.h"

#import "ViewController.h"
#include "azaria.h"


RGBA8Image *surface = nil;

void paintBuffer(RGBA8Image * _surface)
{
    surface = _surface;
    printf("paintBuffer(%p)\n", surface);
}

@interface ViewController () {
    UIImageView *imageViewBackground;
}
@end

@implementation ViewController

- (const char *) getResourcePath
{
    NSBundle* mainBundle;
    mainBundle = NSBundle.mainBundle;
    NSBundle *thisBundle = [NSBundle bundleForClass:[self class]];
    NSString * path;
    path = [thisBundle pathForResource:@"Data" ofType:@""];
    NSLog(@"%@", path);
    return [path UTF8String];
}

+ (UIImage *) blankImage
{
    int width = 100;
    int height = 100;
    UIGraphicsBeginImageContext(CGSizeMake(width, height));
    CGContextAddRect(UIGraphicsGetCurrentContext(), CGRectMake(0, 0, width, height)); // this may not be necessary
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return image;
}

+ (UIImage *) cityImage
{
    UIImage *originalImage = [UIImage imageNamed:@"GUI.JPG"];
    return originalImage;
}

+ (UIImage *) madeImage
{
    unsigned char * buffer;
    int width=640;
    int height=480;
    buffer = malloc(width*height*4);
    memset(buffer, 128, width*height*4);
    UIImage *image = [ImageHelper convertBitmapRGBA8ToUIImage:buffer withWidth:width withHeight:height];
    return image;
}

- (NSTimer *)startTimedTask
{
    NSTimer *timer = [NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(performBackgroundTask) userInfo:nil repeats:YES];
    return timer;
}

- (void)performBackgroundTask
{
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        WinLoop();
        dispatch_async(dispatch_get_main_queue(), ^{
            // TODO: Update UI
            if (surface) {
                UIImage *image = [ImageHelper convertBitmapRGBA8ToUIImage:surface->bitmapData withWidth:surface->width withHeight:surface->height];
                imageViewBackground.image = image;
                [imageViewBackground setNeedsDisplay];
                // [self.view addSubview:imageViewBackground];
            }
            //UIImage * image = getUiImage();
        });
    });
}

+ (RGBA8Image *) flipBitmap:(RGBA8Image*)bitmap
{
    int row;
    RGBA8Image *rgba8 = malloc(sizeof(RGBA8Image));
    rgba8->bitmapData = malloc(bitmap->height*bitmap->bytesPerRow);
    memset(rgba8->bitmapData, 0x1f, bitmap->height*bitmap->bytesPerRow);
    rgba8->height = bitmap->height;
    rgba8->width = bitmap->width;
    rgba8->bytesPerRow = bitmap->bytesPerRow;

    unsigned char * src = bitmap->bitmapData;
    unsigned char * dst = rgba8->bitmapData;
    for (row=0; row<bitmap->height; row++)
        memcpy(dst+(bitmap->bytesPerRow*row),
               src+(bitmap->bytesPerRow*(bitmap->height-(row+1))),
               bitmap->bytesPerRow);

    return rgba8;
}

- (UIImageView *)addBackground
{
        // screen width and height:
    int width = [UIScreen mainScreen].bounds.size.width;
    int height = [UIScreen mainScreen].bounds.size.height;

//    UIImage *gui = [UIImage imageNamed:@"GUI.JPG"];
//    UIImageView *imageView = [[UIImageView alloc] initWithImage:gui];
    imageViewBackground = [[UIImageView alloc] initWithFrame:CGRectMake(0, 0, width, height)];
    imageViewBackground.image = [UIImage imageNamed:@"GUI.JPG"];
        
        // you can change the content mode:
    imageViewBackground.contentMode = UIViewContentModeScaleAspectFit;

    [self.view addSubview:imageViewBackground];
    //        self.sendSubviewToBack(imageViewBackground)
    return imageViewBackground;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    UIImage * original;
    UIImage * flipped;
    
    original = [ViewController cityImage];
    
    RGBA8Image * rgba8 = [ImageHelper convertUIImageToBitmapRGBA8:original];
    RGBA8Image * flipd = rgba8; // [ViewController flipBitmap:rgba8];
    flipped = [ImageHelper convertBitmapRGBA8ToUIImage:flipd->bitmapData withWidth:flipd->width withHeight:flipd->height];

    /*
    UIImage *scaledImage =
    [UIImage imageWithCGImage:[flipped CGImage]
                        scale:(flipped.scale * 0.4)
                  orientation:(flipped.imageOrientation)];
    UIImageView *imageView = [[UIImageView alloc] initWithImage:scaledImage];
    [self.view addSubview:imageView];
*/
    
    [self addBackground];
    
    ddraw_ios_init();
    WinMain([self getResourcePath]);
    [self startTimedTask];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end

