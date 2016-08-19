// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2015 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See http://swift.org/LICENSE.txt for license information
// See http://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//


/*	CFStream.h
	Copyright (c) 2000 - 2015 Apple Inc. and the Swift project authors
*/

#if !defined(__COREFOUNDATION_CFUNITFORMATTER__)
#define __COREFOUNDATION_CFUNITFORMATTER__ 1

#include <CoreFoundation/CFBase.h>
#include <CoreFoundation/CFString.h>
#include <CoreFoundation/CFDictionary.h>
#include <CoreFoundation/CFURL.h>
#include <CoreFoundation/CFRunLoop.h>
#include <CoreFoundation/CFSocket.h>
#include <CoreFoundation/CFError.h>
#include <CoreFoundation/CFNumberFormatter.h>
#if __HAS_DISPATCH__
#include <dispatch/dispatch.h>
#endif



CF_IMPLICIT_BRIDGING_ENABLED
CF_EXTERN_C_BEGIN


struct CFMeasure{
    double value;
    
};
typedef struct CF_BRIDGED_MUTABLE_TYPE(id) __CFUnitFormatter *CFUnitFormatterRef;

CF_EXPORT
CFTypeID CFUnitFormatterGetTypeID(void);

typedef CF_ENUM(CFIndex, CFUnitFormatterStyle) {	// number format styles
    kCFUnitFormatterNumericStyle = 0,
    kCFUnitFormatterShortStyle = 1,
    kCFUnitFormatterMediumStyle = 2,
    kCFUnitFormatterLongStyle = 3,
};

CF_EXPORT
CFUnitFormatterRef CFUnitFormatterCreate(CFAllocatorRef allocator,
                                         CFLocaleRef locale,
                                         CFUnitFormatterStyle  ustyle,
                                         CFNumberFormatterStyle cfnfms);
CF_EXPORT
CFStringRef CGUnitFormatterPluralUnitMetric(CFUnitFormatterRef formatter,
                                            struct CFMeasure*  measures);

CF_EXTERN_C_END
CF_IMPLICIT_BRIDGING_DISABLED

#endif /* ! __COREFOUNDATION_CFUNITFORMATTER__ */
