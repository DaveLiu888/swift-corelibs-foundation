// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2015 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See http://swift.org/LICENSE.txt for license information
// See http://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//


/*	CFNumberFormatter.h
	Copyright (c) 2003 - 2015 Apple Inc. and the Swift project authors
*/

#if !defined(__COREFOUNDATION_CFNUMBERFORMATTERINTERNAL__)
#define __COREFOUNDATION_CFNUMBERFORMATTERINTERNAL__ 1

#include <CoreFoundation/CFBase.h>
#include <CoreFoundation/CFNumber.h>
#include <CoreFoundation/CFLocale.h>
#include "CFICULogging.h"
CF_EXTERN_C_BEGIN


CF_EXPORT
UNumberFormat CFNumberFormatterGetNumberFormat(CFNumberFormatterRef ref);

CF_EXTERN_C_END
#endif /* ! __COREFOUNDATION_CFNUMBERFORMATTERINTERNAL__ */

