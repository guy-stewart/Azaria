//
//  platform.h
//  Azaria
//
//  Created by guy on 11/11/17.
//  Copyright © 2017 guy. All rights reserved.
//

#ifndef platform_h
#define platform_h

#define IOS

#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
const GUID  name = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

#define DECLARE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
extern const GUID FAR name



#define stricmp strcasecmp

#endif /* platform_h */
